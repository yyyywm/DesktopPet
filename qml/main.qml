import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

PetWindow {
    id: petWindow

    Component.onCompleted: {
        App.ShowPetWindowRequested.connect(function () {
            petWindow.show()
        })
        App.HidePetWindowRequested.connect(function () {
            petWindow.hide()
        })
        App.OpenAddDialogRequested.connect(function () {
            addDialog.openFromModel()
        })
        App.UpdateMessageRequested.connect(function (title, body, url) {
            updateDialog.title = title
            updateDialog.body = body
            updateDialog.url = url
            updateDialog.open()
        })
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
            spacing: AppTheme.paddingSmall

            Label {
                text: updateDialog.title
                font.bold: true
                font.pixelSize: AppTheme.fontTitle
            }
            Label {
                text: updateDialog.body
                visible: updateDialog.body.length > 0
                font.pixelSize: AppTheme.fontBody
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
