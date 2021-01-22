import QtQuick 2.2
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.0
import QtQuick.Window 2.0
import CppTutorial 1.0

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Cpp Tutorial")

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20

        TextField {
            Layout.fillWidth: true
            readOnly: true
            placeholderText: "Missing name"
            text: person && person.name
        }
        RowLayout {
            Layout.fillWidth: true
            TextField {
                Layout.fillWidth: true
                readOnly: true
                text: person && person.birthYear
            }
            Button {
                text: "+1"
                onClicked: person.birthYear += +1
            }
            Button {
                text: "-1"
                onClicked: person.birthYear += -1
            }
        }
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
        RowLayout {
            Layout.fillWidth: true
            TextField {
                id: renameName
                Layout.fillWidth: true
                placeholderText: "Enter new name here"
            }
            Button {
                text: "Rename"
                onClicked: person.name = renameName.text
            }
        }
    }
}
