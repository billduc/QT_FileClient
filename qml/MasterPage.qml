import VPlayApps 1.0
import QtQuick 2.2
import QtQuick.Dialogs 1.0
import managerConnecion 1.0
import QtGraphicalEffects 1.0
import QtQuick.Controls 1.4
import QtQuick.Controls 2.3

ListPage {
    id: masterPage
    title: "File Client"
    property  var filepatch: "NULL"


    FileDialog {
        id: fileDialog
        title: "Please choose a file"
        folder: shortcuts.home
        onAccepted: {
            console.log("You chose: " + fileDialog.fileUrls)
            filepatch = fileDialog.fileUrl;
            fileDialog.close()
            masterPage.model.push({ text: filepatch })
            masterPage.modelChanged()
        }
        onRejected: {
            console.log("Canceled")
            fileDialog.close()
        }
        //Component.onCompleted: visible = true
    }

    AppButton {
        icon: IconType.plus
        anchors.right: parent.right
        anchors.rightMargin: dp(10)
        anchors.bottom: parent.bottom
        anchors.bottomMargin: dp(10)
        onClicked: {
            fileDialog.open()
        }
    }

    rightBarItem: IconButtonBarItem {
        icon: IconType.plus

        onClicked: {
            // Add a new item and notify listview model
            fileDialog.open()
        }
    }

    model: []

    delegate: SwipeOptionsContainer {
        id: container

        rightOption: AppButton {
            text: qsTr("upload")

            onClicked: {
                //uploadfile.upFile(filepatch);
                manageConnecion.sendRequestUpload(filepatch);
                container.hideOptions()

                // Remove selected item and notify listview model
                //masterPage.model.splice(index, 1)
                //masterPage.modelChanged()
            }
        }



        SimpleRow {
            onSelected: {
                masterPage.navigationStack.popAllExceptFirstAndPush(Qt.resolvedUrl("DetailPage.qml"), { content: item.text })
            }
        }
    }

    ProgressBar {
        id: control
        //anchors.centerIn: parent
        anchors.bottom: parent.Bottom
        value: 0
        height: 20
        clip: true
        background: Rectangle {
            implicitWidth: 200
            implicitHeight: 6
            border.color: "#999999"
            radius: 5
        }
        contentItem: Item {
            implicitWidth: 200
            implicitHeight: 4

            Rectangle {
                id: bar
                width: control.visualPosition * parent.width
                height: parent.height
                radius: 5
            }

            LinearGradient {
                anchors.fill: bar
                start: Qt.point(0, 0)
                end: Qt.point(bar.width, 0)
                source: bar
                gradient: Gradient {
                    GradientStop { position: 0.0; color: "#17a81a" }
                    GradientStop { id: grad; position: 0.5; color: Qt.lighter("#17a81a", 2) }
                    GradientStop { position: 1.0; color: "#17a81a" }
                }
                PropertyAnimation {
                    target: grad
                    property: "position"
                    from: 0.1
                    to: 0.9
                    duration: 1000
                    running: true
                    loops: Animation.Infinite
                }
            }
            LinearGradient {
                anchors.fill: bar
                start: Qt.point(0, 0)
                end: Qt.point(0, bar.height)
                source: bar
                gradient: Gradient {
                    GradientStop { position: 0.0; color: Qt.rgba(0,0,0,0) }
                    GradientStop { position: 0.5; color: Qt.rgba(1,1,1,0.3) }
                    GradientStop { position: 1.0; color: Qt.rgba(0,0,0,0.05) }
                }
            }
        }
        PropertyAnimation {
            target: control
            property: "value"
            from: 0
            to: 1
            duration: 5000
            running: true
            loops: Animation.Infinite
        }
    }
}
