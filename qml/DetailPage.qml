import VPlayApps 1.0
import QtQuick 2.0
import file 1.0
import managerConnecion 1.0

Page {

    title: qsTr("Detail")

    property alias content: text.text

    AppText {
        id: text
        anchors.centerIn: parent
    }

}
