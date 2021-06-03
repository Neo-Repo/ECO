import QtQuick 2.12
import QtQuick.Layouts 1.12

GridLayout {
    // Services List
    property var servicesList: [
        {
            name: "Main Display",
            icon: "\uf108",
            active: true
        },
        {
            name: "Windows List",
            icon: "\uf2fa",
            active: true
        },
        {
            name: "Programs",
            icon: "\uf37e",
            active: true
        },
        {
            //name: "Notifications",
            name: "Notifications, N/A",
            icon: "\uf0f3",
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
