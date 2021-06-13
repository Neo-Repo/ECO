import QtQuick 2.12
import QtQuick.Layouts 1.12

GridLayout {
    // Services List
    property var servicesList: [
        {
            name: "Main Display",
            icon: "\uf108",
            type: "services",
            active: serverBackend.connected
        },
        {
            name: "Windows List",
            icon: "\uf2fa",
            type: "services",
            active: serverBackend.connected
        },
        {
            name: "Programs",
            icon: "\uf37e",
            type: "services",
            active: true
        },
        {
            //name: "Notifications",
            name: "Notifications, N/A",
            icon: "\uf0f3",
            type: "services",
            //active: true
            active: false
        }
    ]

    // Services Items
    Repeater {
        model: servicesList

        MenuItem {}
    }
}
