#include "settings.h"

Settings::Settings(QString *user, virConnectPtr *_conn, virDomainPtr *_domain, QObject *parent) : QObject(parent)
{
    username = *user;
    conn = *_conn;
    domain = *_domain;
}

int Settings::getRAM()
{
    QXmlStreamReader reader(getXML());
    while(!reader.atEnd() && !reader.hasError()) {
        if(reader.readNext() == QXmlStreamReader::StartElement && reader.name() == "memory") {
            return reader.readElementText().toInt();
        }
    }

    return 1024;
}

void Settings::setRAM(QString value)
{
    QFile file("/home/"+username+"/.config/Neo/ECO/WindowsECO.xml");
    file.open(QIODevice::ReadWrite | QIODevice::Text);

    QDomDocument doc("config");
    doc.setContent(&file);

    QDomElement root = doc.documentElement();
    QDomElement nodeTag = root.firstChildElement("memory");

    QDomElement newNodeTag = doc.createElement(QString("memory"));
    newNodeTag.setAttribute("unit", "MiB");
    QDomText newNodeText = doc.createTextNode(value);
    newNodeTag.appendChild(newNodeText);

    root.replaceChild(newNodeTag, nodeTag);

    file.seek(0);
    file.write(doc.toByteArray());
    file.close();

    virDomainSetMaxMemory(domain, value.toInt() * 1024);
    virDomainSetMemoryFlags(domain, value.toInt() * 1024, VIR_DOMAIN_MEM_CONFIG);
}

int Settings::getCPU()
{
    QXmlStreamReader reader(getXML());
    while(!reader.atEnd() && !reader.hasError()) {
        if(reader.readNext() == QXmlStreamReader::StartElement && reader.name() == "vcpu") {
            return reader.readElementText().toInt();
        }
    }

    return 1;
}

void Settings::setCPU(QString value)
{
    QFile file("/home/"+username+"/.config/Neo/ECO/WindowsECO.xml");
    file.open(QIODevice::ReadWrite | QIODevice::Text);

    QDomDocument doc("config");
    doc.setContent(&file);

    QDomElement root = doc.documentElement();
    QDomElement nodeTag = root.firstChildElement("vcpu");

    QDomElement newNodeTag = doc.createElement(QString("vcpu"));
    QDomText newNodeText = doc.createTextNode(value);
    newNodeTag.appendChild(newNodeText);

    root.replaceChild(newNodeTag, nodeTag);

    file.seek(0);
    file.write(doc.toByteArray());
    file.close();

    virDomainSetVcpusFlags(domain, value.toInt(), VIR_DOMAIN_AFFECT_CONFIG | VIR_DOMAIN_VCPU_MAXIMUM);
    virDomainSetVcpusFlags(domain, value.toInt(), VIR_DOMAIN_AFFECT_CURRENT);
}

void Settings::openXML()
{
    QProcess::startDetached("xdg-open /home/"+username+"/.config/Neo/ECO/WindowsECO.xml");
}

QString Settings::getXML() {
    QFile config("/home/"+username+"/.config/Neo/ECO/WindowsECO.xml");
    config.open(QIODevice::ReadOnly | QIODevice::Text);

    QString config_data = config.readAll();
    config.close();

    return config_data;
}
