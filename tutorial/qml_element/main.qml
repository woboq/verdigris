import QtQuick

import QmlTutorial 1.0

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    MyObject {
        demoProp: 93
    }
}
