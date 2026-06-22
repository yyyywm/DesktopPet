import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window

Window {
    id: root
    flags: Qt.WindowStaysOnTopHint | Qt.Dialog | Qt.FramelessWindowHint
    color: Theme.background
    width: 320
    height: 360
    minimumWidth: 240
    minimumHeight: 200
    visible: false
    modality: Qt.ApplicationModal

    function openFromModel() {
        listModel.clear()
        for (let i = 0; i < TaskModel.rowCount(); ++i) {
            const idx = TaskModel.index(i, 0)
            listModel.append({
                text: TaskModel.data(idx, TaskModel.TextRole)
            })
        }
        root.show()
    }

    function saveToModel() {
        TaskModel.clear()
        for (let i = 0; i < listModel.count; ++i) {
            TaskModel.addEvent(listModel.get(i).text)
        }
        TaskModel.save()
        root.hide()
    }

    MouseArea {
        id: dragArea
        anchors.fill: parent
        property point dragStartMouse
        property point dragStartWindow

        onPressed: function (mouse) {
            dragStartMouse = Qt.point(mouse.x, mouse.y)
            dragStartWindow = Qt.point(root.x, root.y)
        }
        onPositionChanged: function (mouse) {
            if (mouse.buttons & Qt.LeftButton) {
                root.x = dragStartWindow.x + (mouse.x - dragStartMouse.x)
                root.y = dragStartWindow.y + (mouse.y - dragStartMouse.y)
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.paddingMedium
        spacing: Theme.paddingSmall

        ListView {
            id: listView
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: ListModel { id: listModel }
            clip: true
            spacing: Theme.paddingSmall

            delegate: RowLayout {
                width: ListView.view.width
                TextField {
                    Layout.fillWidth: true
                    text: model.text
                    onEditingFinished: {
                        model.text = text
                    }
                }
            }
        }

        RowLayout {
            Layout.alignment: Qt.AlignHCenter
            spacing: Theme.paddingSmall

            Button {
                text: qsTr("添加")
                onClicked: listModel.append({ text: "" })
            }
            Button {
                text: qsTr("删除")
                enabled: listView.currentIndex >= 0
                onClicked: {
                    if (listView.currentIndex >= 0) {
                        listModel.remove(listView.currentIndex)
                    }
                }
            }
            Button {
                text: qsTr("确定")
                onClicked: root.saveToModel()
            }
            Button {
                text: qsTr("取消")
                onClicked: root.hide()
            }
        }
    }
}
