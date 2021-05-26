#include <QProcess>
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QDir>
#include <QThread>
#include "startup.h"

Startup::Startup(QString *user, QObject *parent) : QObject(parent)
{
    username = *user;

    // create config directory
    QDir configDir;
    QString configPath = "/home/"+username+"/.config/Neo/ECO/";
    if (!configDir.exists(configPath))
        configDir.mkpath(configPath);

    // create applications directory
    QDir applicationsDir;
    QString applicationsPath = "/home/"+username+"/.local/share/Neo/ECO/applications";
    if (!applicationsDir.exists(applicationsPath))
        applicationsDir.mkpath(applicationsPath);

    // create icons directory
    QDir iconsDir;
    QString iconsPath = "/home/"+username+"/.local/share/Neo/ECO/icons";
    if (!iconsDir.exists(iconsPath))
        iconsDir.mkpath(iconsPath);

    // set XDG_DATA_DIRS
    QString xdg_path = "/home/"+username+"/.local/share/Neo/ECO";
    QString xdg_dirs = getenv("XDG_DATA_DIRS");

    // set the variable to .profile
    if (!xdg_dirs.contains(xdg_path)) {
        QString profile_path = "/home/"+username+"/.profile";

        QFile profile(profile_path);
        if (!profile.open(QIODevice::ReadOnly | QIODevice::Text))
            return;
        QString profile_data = profile.readAll();
        profile.close();

        if (profile.open(QIODevice::WriteOnly | QIODevice::Append) && !profile_data.contains(xdg_path)) {
            profile.write("\nXDG_DATA_DIRS=$XDG_DATA_DIRS:");
            profile.write(xdg_path.toUtf8());
            profile.write("\n");
            profile.close();
        }
    }
}

QStringList Startup::required()
{
    QStringList Error;

    // CPU Virtualization
    QProcess process;
    process.start("egrep \"svm|vmx\" /proc/cpuinfo");
    process.waitForFinished(-1);
    QString stdout = process.readAllStandardOutput();
    if (stdout.isEmpty()) {
        Error << "Your CPU doesn't support Virtualization";
        Error << "\nWe have determined that by running the command `egrep \"svm|vmx\" /proc/cpuinfo`. "
                 "Since the output was empty the error happend.\n"
                 "\nNote: If your CPU supports Virtualization, make sure it is enable in BIOS.";

        return Error;
    }

    // Kernel kvm support
    if (!QFile::exists("/dev/kvm")) {
        Error << "Your kernel doesn't support kvm";
        Error << "\nWe have checked the `/dev/kvm` path and it doesn't exist.";

        return Error;
    }

    // Virsh program
    QProcess check;
    check.start("virsh -v");
    bool started = check.waitForStarted();
    if (!started) {
        Error << "virsh";
        Error << "\nVirsh is not installed, "
                 "We have determined that by running the command `virsh -v`, "
                 "That means you don't have the required programs...\n"
                 "\nIf you press `Install` we will run the following command to install the programs:\n"
                 "sudo apt install -y qemu qemu-kvm libvirt-daemon bridge-utils libvirt-daemon-system libvirt-clients";
    }

    // vm.xml file
    QString vmPath = "/home/"+username+"/.config/Neo/ECO/WindowsECO.xml";
    if (!QFileInfo::exists(vmPath) && !QFileInfo(vmPath).isFile())
        Error << "vm.xml not found";

    return Error;
}

QJsonObject Startup::spec()
{
    // get System CPU, RAM
    QProcess getRam;
    getRam.start("awk", QStringList() << "/MemTotal/ { print $2 }" << "/proc/meminfo");
    getRam.waitForFinished(-1);
    int memory = getRam.readAllStandardOutput().toLong() / 1000;
    getRam.close();

    return {
        {"CPU", QThread::idealThreadCount()},
        {"RAM", memory}
    };
}

void Startup::startInstall()
{
    QThread *installThread = new QThread();
    moveToThread(installThread);
    connect(installThread, &QThread::started, this, &Startup::install);
    installThread->start();
}

void Startup::startNewMachine(QJSValue rawInput)
{
    input = rawInput.toVariant().toJsonObject();
    QThread *createThread = new QThread();
    moveToThread(createThread);
    connect(createThread, &QThread::started, this, &Startup::newMachine);
    createThread->start();
}

void Startup::install()
{
    QProcess apt;
    QString stderr;

    apt.start("pkexec sh -c \"apt-get update &> /dev/null;"
              "apt-get install -y qemu qemu-kvm libvirt-daemon bridge-utils libvirt-daemon-system libvirt-clients;"
              "systemctl enable --now libvirtd\"");
    apt.waitForFinished(-1);

    stderr = apt.readAllStandardError();
    if (!stderr.isEmpty() && required().length() > 1) {
        setResult({"Error:", stderr});
        return;
    }

    setResult({"100", "Done."});
}

void Startup::newMachine()
{
    int CPU = input["CPU"].toInt();
    int RAM = input["RAM"].toInt();
    int Size = input["Size"].toInt();
    QString Path = input["Path"].toString();
    QString ISO = input["ISO"].toString();
    QString vm_xml = QString("<domain type='kvm'> <name>WindowsECO</name> <uuid>c4746ecd-7d5f-4caf-bc75-fc3c3b75524c</uuid> <memory unit='KiB'>8097152</memory> <currentMemory unit='KiB'>8097152</currentMemory> <vcpu placement='static'>8</vcpu>"
                             " \n\n<os> <type arch='x86_64' machine='pc-q35-4.2'>hvm</type> <boot dev='hd'/> <boot dev='cdrom'/> </os>\n\n"
                             " <features> <acpi/> <apic/> </features> <cpu mode='host-passthrough' check='none'> <topology sockets='1' cores='4' threads='2'/> </cpu> <clock offset='localtime'> <timer name='rtc' tickpolicy='catchup'/> <timer name='pit' tickpolicy='delay'/> <timer name='hpet' present='no'/> </clock> <on_poweroff>destroy</on_poweroff> <on_reboot>destroy</on_reboot> <on_crash>destroy</on_crash> <pm> <suspend-to-mem enabled='no'/> <suspend-to-disk enabled='no'/> </pm> <devices> <emulator>/usr/bin/qemu-system-x86_64</emulator> <disk type='file' device='disk'> <driver name='qemu' type='qcow2' cache='writeback'/>"
                             " <source file='%1'/> <target dev='sda' bus='sata'/> <address type='drive' controller='0' bus='0' target='0' unit='0'/> </disk> <disk type='file' device='cdrom'> <driver name='qemu' type='raw'/>"
                             " <source file='%2' startupPolicy='mandatory'/> <target dev='hdc' bus='sata'/> <readonly/> <address type='drive' controller='0' bus='0' target='0' unit='2'/> </disk> <controller type='usb' index='0' model='ich9-ehci1'> <address type='pci' domain='0x0000' bus='0x00' slot='0x1d' function='0x7'/> </controller> <controller type='usb' index='0' model='ich9-uhci1'> <master startport='0'/> <address type='pci' domain='0x0000' bus='0x00' slot='0x1d' function='0x0' multifunction='on'/> </controller> <controller type='usb' index='0' model='ich9-uhci2'> <master startport='2'/> <address type='pci' domain='0x0000' bus='0x00' slot='0x1d' function='0x1'/> </controller> <controller type='usb' index='0' model='ich9-uhci3'> <master startport='4'/> <address type='pci' domain='0x0000' bus='0x00' slot='0x1d' function='0x2'/> </controller> <controller type='sata' index='0'> <address type='pci' domain='0x0000' bus='0x00' slot='0x1f' function='0x2'/> </controller> <controller type='pci' index='0' model='pcie-root'/> <controller type='pci' index='1' model='pcie-root-port'> <model name='pcie-root-port'/> <target chassis='1' port='0x10'/> <address type='pci' domain='0x0000' bus='0x00' slot='0x02' function='0x0' multifunction='on'/> </controller> <controller type='pci' index='2' model='pcie-to-pci-bridge'> <model name='pcie-pci-bridge'/> <address type='pci' domain='0x0000' bus='0x01' slot='0x00' function='0x0'/> </controller> <controller type='pci' index='3' model='pcie-root-port'> <model name='pcie-root-port'/> <target chassis='3' port='0x11'/> <address type='pci' domain='0x0000' bus='0x00' slot='0x02' function='0x1'/> </controller> <controller type='pci' index='4' model='pcie-root-port'> <model name='pcie-root-port'/> <target chassis='4' port='0x12'/> <address type='pci' domain='0x0000' bus='0x00' slot='0x02' function='0x2'/> </controller> <controller type='pci' index='5' model='pcie-root-port'> <model name='pcie-root-port'/> <target chassis='5' port='0x13'/> <address type='pci' domain='0x0000' bus='0x00' slot='0x02' function='0x3'/> </controller> <controller type='virtio-serial' index='0'> <address type='pci' domain='0x0000' bus='0x03' slot='0x00' function='0x0'/> </controller> <controller type='ccid' index='0'> <address type='usb' bus='0' port='1'/> </controller> <interface type='user'> <mac address='52:54:00:d7:85:8b'/> <model type='rtl8139'/> <address type='pci' domain='0x0000' bus='0x02' slot='0x01' function='0x0'/> </interface> <smartcard mode='passthrough' type='spicevmc'> <address type='ccid' controller='0' slot='0'/> </smartcard> <serial type='pty'> <target type='isa-serial' port='0'> <model name='isa-serial'/> </target> </serial> <console type='pty'> <target type='serial' port='0'/> </console> <channel type='spicevmc'> <target type='virtio' name='com.redhat.spice.0'/> <address type='virtio-serial' controller='0' bus='0' port='1'/> </channel> <channel type='spiceport'> <source channel='org.spice-space.webdav.0'/> <target type='virtio' name='org.spice-space.webdav.0'/> <address type='virtio-serial' controller='0' bus='0' port='2'/> </channel> <input type='tablet' bus='usb'> <address type='usb' bus='0' port='2'/> </input> <input type='mouse' bus='ps2'/> <input type='keyboard' bus='ps2'/> <graphics type='spice'> <listen type='none'/> <image compression='off'/> <gl enable='no'/> </graphics> <sound model='ich9'> <address type='pci' domain='0x0000' bus='0x00' slot='0x1b' function='0x0'/> </sound> <video> <model type='qxl' ram='65536' vram='65536' vgamem='16384' heads='1' primary='yes'> <acceleration accel3d='no'/> </model> <address type='pci' domain='0x0000' bus='0x00' slot='0x01' function='0x0'/> </video> <redirdev bus='usb' type='spicevmc'> <address type='usb' bus='0' port='3'/> </redirdev> <redirdev bus='usb' type='spicevmc'> <address type='usb' bus='0' port='4'/> </redirdev> <redirdev bus='usb' type='spicevmc'> <address type='usb' bus='0' port='5'/> </redirdev> <redirdev bus='usb' type='spicevmc'> <address type='usb' bus='0' port='6'/> </redirdev> <memballoon model='virtio'> <address type='pci' domain='0x0000' bus='0x04' slot='0x00' function='0x0'/> </memballoon> </devices> </domain>").arg(Path).arg(ISO);

    // Create WindowsECO.qcow2
    if (!QFileInfo::exists(Path) && !QFileInfo(Path).isFile()) {
        QProcess qemu;
        qemu.start(QString("sh -c \"qemu-img create -f qcow2 %1 %2G\"").arg(Path).arg(Size));
        qemu.waitForFinished(-1);

        QString stderr = qemu.readAllStandardError();
        if (!stderr.isEmpty()) {
            setResult({"Qemu Error:", stderr});
            return;
        }
    }

    // Create vm.xml
    QFile save("/home/"+username+"/.config/Neo/ECO/WindowsECO.xml");
    save.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream stream(&save);
    stream << vm_xml;
    save.close();

    emit created();
}

QStringList Startup::getResult() const
{
    return result;
}

void Startup::setResult(const QStringList &newResult)
{
    result = newResult;
    emit resultChanged();
}
