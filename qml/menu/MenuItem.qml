import QtQuick 2.12
import QtQuick.Layouts 1.12

Rectangle {
    property int iD: !modelData.Id ? 0 : modelData.Id
    property string name: modelData.name
    property string icon: modelData.icon
    property bool actived: modelData.active
    property string type: ""
    property color hoverColor: "#8EF4FC"

    id: menuItem
    Layout.fillHeight: true
    Layout.fillWidth: true
    Layout.margins: 4
    border.color: hoverColor
    color: "transparent"
    opacity: (name.match(RegExp(/Settings/)) || type !== "settings" || opacityToggle === -1) ? 1 : 0
    radius: 4

    // Name
    Text {
        text: name
        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
            bottomMargin: 20
        }
        font.pixelSize: 16
        color: borderBottomMenu.color
    }

    // Icon
    Text {
        text: icon
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: parent.top
            topMargin: 27
        }
        font.pixelSize: 27
        color: borderBottomMenu.color
    }

    // Border Bottom
    Rectangle {
        id: borderBottomMenu
        width: parent.width
        anchors.bottom: parent.bottom
        radius: 4
    }

    // Animations
    state: "mouseLeaved"

    states: [
        State {
            name: "deactive"
            when: !actived
            PropertyChanges { target: menuItem; border.width: 0 }
            PropertyChanges { target: borderBottomMenu; height: 1.5 }
            PropertyChanges { target: borderBottomMenu; color: "grey" }
        },
        State {
            name: "mouseEntered"
            when: menuListMouse.containsMouse
            PropertyChanges { target: menuItem; border.width: 1.5 }
            PropertyChanges { target: borderBottomMenu; height: 0 }
            PropertyChanges { target: borderBottomMenu; color: hoverColor }
        },
        State {
            name: "mouseLeaved"
            when: !menuListMouse.containsMouse
            PropertyChanges { target: menuItem; border.width: 0 }
            PropertyChanges { target: borderBottomMenu; height: 1.5 }
            PropertyChanges { target: borderBottomMenu; color: "white" }
        }
    ]

    transitions: [
        // On Mouse Entered
        Transition {
            to: "mouseEntered"
            PropertyAnimation {
                target: menuItem; property: "border.width"; duration: 400 }
            PropertyAnimation {
                target: borderBottomMenu; properties: "color,height"; duration: 400 }
        },
        // On Mouse Leaved
        Transition {
            to: "mouseLeaved"
            PropertyAnimation {
                target: menuItem; property: "border.width"; duration: 300 }
            PropertyAnimation {
                target: borderBottomMenu; properties: "color,height"; duration: 300 }
        }
    ]

    // Settings Animation
    property int opacityToggle: -1
    property int settingsXLeft: 535
    property int settingsXRight: 5
    Behavior on x {
        NumberAnimation { duration: 900 }
    }
    Behavior on opacity {
        NumberAnimation {
            // I need to remove type settings when I load it fomr loader
            duration: (type === "settings") && 400 * iD
        }
    }
    onXChanged: {
        if (x === settingsXRight) direction = "left"
        else if (x === settingsXLeft) direction = "right"
    }
    onOpacityToggleChanged: {
        if (!name.match(RegExp(/Settings/)))
            menuItem.opacity = 0
    }
    Component.onCompleted: {
        opacity = 1
        animating = false
    }

    MouseArea {
        id: menuListMouse
        anchors.fill: parent
        hoverEnabled: actived
        visible: !animating

        onClicked: {
            if (name.match(RegExp(/Settings/))) {
                animating = true
                opacityAnimationToggle = !opacityAnimationToggle
                if (direction === "left")
                    menuItem.x = settingsXLeft
                else if (direction === "right")
                    menuItem.x = settingsXRight
            }
            else if(actived)
                 action(name)
        }
    }
}
