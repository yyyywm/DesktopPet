import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

PetWindow {
    id: petWindow

    Connections {
        target: App
        function onShowPetWindowRequested() {
            petWindow.show()
        }
        function onHidePetWindowRequested() {
            petWindow.hide()
        }
        function onOpenAddDialogRequested() {
            addDialog.openFromModel()
        }
        function onUpdateMessageRequested(title, body, url) {
            updateDialog.title = title
            updateDialog.body = body
            updateDialog.url = url
            updateDialog.open()
        }
    }

    onRightClicked: {
        taskListDialog.showAt(petWindow.x + petWindow.width, petWindow.y)
    }

    onDragStarted: {
        taskListDialog.hideDialog()
    }

    TaskListDialog {
        id: taskListDialog
    }

    AddDialog {
        id: addDialog
    }

    Dialog {
        id: updateDialog
        property string url: ""
        property string body: ""
        title: ""
        modal: true
        standardButtons: url.length > 0 ? Dialog.Ok | Dialog.Cancel : Dialog.Ok
        anchors.centerIn: parent
        implicitWidth: 280
        implicitHeight: contentLayout.implicitHeight + topPadding + bottomPadding

        ColumnLayout {
            id: contentLayout
            width: updateDialog.availableWidth
            spacing: Theme.paddingSmall

            Label {
                text: updateDialog.title
                font.bold: true
                font.pixelSize: Theme.fontTitle
            }
            Label {
                text: updateDialog.body
                visible: updateDialog.body.length > 0
                font.pixelSize: Theme.fontBody
                wrapMode: Text.Wrap
                Layout.fillWidth: true
            }
        }

        onAccepted: {
            if (url.length > 0) {
                Qt.openUrlExternally(url)
            }
        }
    }
}
