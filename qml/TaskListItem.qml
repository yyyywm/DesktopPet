import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

RowLayout {
    id: root
    width: ListView.view ? ListView.view.width : implicitWidth
    spacing: Theme.paddingSmall

    required property int index
    required property int eventId
    required property string text
    required property bool done

    signal textEdited(string newText)
    signal doneToggled(bool newDone)

    CheckBox {
        id: checkBox
        checked: root.done
        onToggled: root.doneToggled(checked)

        Connections {
            target: root
            function onDoneChanged() {
                checkBox.checked = root.done
            }
        }
    }

    TextField {
        id: textField
        Layout.fillWidth: true
        text: root.text
        background: Rectangle {
            color: "transparent"
            border.color: parent.activeFocus ? Theme.primary : "transparent"
            radius: Theme.radiusSmall
        }
        onEditingFinished: {
            if (text !== root.text) {
                root.textEdited(text)
            }
        }

        Connections {
            target: root
            function onTextChanged() {
                textField.text = root.text
            }
        }
    }
}
