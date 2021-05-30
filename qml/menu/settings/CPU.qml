import QtQuick 2.12
import ".."

Item {
    Back {}

    Text {
        text: "CPU"
        width: 90
        height: 20
        anchors.centerIn: parent
        font.pixelSize: 20
        font.weight: "Light"
        color: "white"

        TextInput {
            text: "2"
            width: parent.width - 45
            height: parent.height
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            font.pixelSize: 18
            horizontalAlignment: TextInput.AlignHCenter
            inputMethodHints: Qt.ImhDigitsOnly
            validator: IntValidator { bottom: 1; top: spec['CPU'] }
            color: "white"
            clip: true

            onFocusChanged: {
                if (text < 1)
                    text = 1
                else if (text > spec['CPU'])
                    text = spec['CPU']

                inputData['CPU'] = parseInt(text)
                inputChanged()
            }

            Rectangle {
                width: parent.width
                height: 1
                anchors.bottom: parent.bottom
            }
        }
    }
}
