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
            name: "Notifications",
            icon: "\uf0f3",
            active: true
        }
    ]

    // Services Items
    Repeater {
        model: servicesList

        MenuItem {}
    }
}
