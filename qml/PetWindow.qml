import QtQuick
import QtQuick
import QtQuick.Window

Window {
    id: root
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint | Qt.Tool
    color: "transparent"
    width: 168
    height: 163

    signal rightClicked()
    signal dragStarted()

    AnimatedImage {
        id: petImage
        source: "../image/bear.gif"
        anchors.fill: parent
        playing: true
    }

    MouseArea {
        id: dragArea
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton

        property point dragStartMouse
        property point dragStartWindow

        onPressed: function (mouse) {
            if (mouse.button === Qt.LeftButton) {
                dragStartMouse = Qt.point(mouse.x, mouse.y)
                dragStartWindow = Qt.point(root.x, root.y)
                root.dragStarted()
            } else if (mouse.button === Qt.RightButton) {
                root.rightClicked()
            }
        }

        onPositionChanged: function (mouse) {
            if (mouse.buttons & Qt.LeftButton) {
                root.x = dragStartWindow.x + (mouse.x - dragStartMouse.x)
                root.y = dragStartWindow.y + (mouse.y - dragStartMouse.y)
            }
        }
    }
}
