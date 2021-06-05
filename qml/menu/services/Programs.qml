import QtQuick 2.12
import QtQuick.Controls 2.12
import ".."

Item {
    Back {}

    ScrollView {
        anchors {
            fill: parent
            top: parent.top
            bottom: parent.bottom
            topMargin: 50
            bottomMargin: 20
        }
        clip: true

        Grid {
            id: programsGrid
            anchors.left: parent.left
            anchors.leftMargin: 10
            columns: 6
            rowSpacing: 12
            columnSpacing: 12

            Repeater {
                model: 50
                delegate: program
            }
        }
    }

    Component {
        id: program

        Rectangle {
            width: 100
            height: width
            color: programMouse.containsMouse ? "#33ffffff" : "transparent"
            radius: 3
            clip: true

            Image {
                width: parent.width - 5
                height: parent.height - 15
                anchors.top: parent.top
                anchors.topMargin: 0
                anchors.horizontalCenter: parent.horizontalCenter
                source: "file:///home/silent/.local/share/Neo/ECO/icons/Photos.png"
            }

            Text {
                width: 100
                text: "text "+modelData
                color: "white"
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.bottomMargin: 3
                anchors.leftMargin: 3
                font.pixelSize: 13
            }

            MouseArea {
                id: programMouse
                anchors.fill: parent
                hoverEnabled: true
            }
        }
    }
}
