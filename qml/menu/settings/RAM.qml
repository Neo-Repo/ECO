import QtQuick 2.12
import ".."

Item {
    Back {}

    Text {
        text: "RAM"
        width: 150
        height: 20
        anchors.centerIn: parent
        font.pixelSize: 20
        font.weight: "Light"
        color: "white"

        TextInput {
            text: "4096"
            width: parent.width - 50
            height: parent.height
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            font.pixelSize: 18
            horizontalAlignment: TextInput.AlignHCenter
            inputMethodHints: Qt.ImhDigitsOnly
            validator: IntValidator { bottom: 1024; top: spec['RAM'] }
            color: "white"
            clip: true

            onFocusChanged: {
                if (text < 1024)
                    text = 1024
                else if (text > spec['RAM'])
                    text = spec['RAM']

                inputData['RAM'] = parseInt(text)
                inputChanged()
            }

            Rectangle {
                width: parent.width
                height: 1
                anchors.bottom: parent.bottom
            }
        }

        Text {
            text: "MB"
            anchors {
                right: parent.right
                rightMargin: -35
            }
            font.pixelSize: 20
            font.weight: "Light"
            color: "white"
        }
    }
}
