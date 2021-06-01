import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import ".."

Item {
    Back {}

    ScrollView {
        anchors{
            fill: parent
            top: parent.top
            bottom: parent.bottom
            topMargin: 50
            bottomMargin: 20
        }
        clip: true

        ColumnLayout {
            x: root.width / 6
            spacing: 12

            Repeater {
                model: settingsBackend.usbDevices
                delegate: device
            }
        }
    }

    Component {
        id: device

        Rectangle {
            width: 450
            height: 40
            color: "transparent"
            border.color: "#aaffffff"
            radius: 3
            clip: true
            Layout.alignment: Qt.AlignHCenter

            Text {
                id: usbName
                text: modelData.name
                color: "white"
                font.pixelSize: 16
                anchors {
                    verticalCenter: parent.verticalCenter
                    left: parent.left
                    leftMargin: 10
               }
            }

            MouseArea {
                id: mouseUSB
                anchors.fill: parent
                onClicked: {
                    if (true) {
                        usbName.color = "black"
                        parent.color = "#aaffffff"
                        parent.border.color = "#aaffffff"
                    } else {
                        usbName.color = "white"
                        parent.color = "transparent"
                        parent.border.color = "#aaffffff"
                    }
                }
                cursorShape: Qt.PointingHandCursor
            }
        }
    }
}
