import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window

Window {
    id: root
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint | Qt.Tool
    color: Theme.background
    width: 360
    height: 400
    visible: false

    function showAt(xPos, yPos) {
        root.x = xPos
        root.y = yPos
        root.show()
    }

    function hideDialog() {
        TaskModel.removeDoneEvents()
        TaskModel.save()
        root.hide()
    }

    onClosing: {
        hideDialog()
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.paddingMedium
        spacing: Theme.paddingSmall

        ListView {
            id: listView
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: TaskModel
            clip: true
            spacing: 6

            delegate: TaskListItem {
                index: model.index
                eventId: model.id
                text: model.text
                done: model.done

                onTextEdited: function (newText) {
                    TaskModel.setData(TaskModel.index(index, 0), newText, TaskModel.TextRole)
                }
                onDoneToggled: function (newDone) {
                    TaskModel.setData(TaskModel.index(index, 0), newDone, TaskModel.DoneRole)
                }
            }
        }

        Button {
            text: qsTr("添加")
            Layout.alignment: Qt.AlignHCenter
            onClicked: {
                TaskModel.addEvent("")
                listView.positionViewAtEnd()
            }
        }
    }
}
