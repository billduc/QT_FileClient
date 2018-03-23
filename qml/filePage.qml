import VPlayApps 1.0
import QtQuick 2.2
import QtQuick.Dialogs 1.0
import managerConnecion 1.0
import Connection 1.0
import QtGraphicalEffects 1.0
import QtQuick.Controls 1.4
import QtQuick.Controls 2.3

Page {
    id:                 filePage
    title:              "Files"

    property  string    filepatch:          ""
    property  var       username:           "user1"
    property  real      _presentProgress:   0


    Connections {
        target: manageConnecion
        onSignal_Update_Persent_Progress: {
           _presentProgress = _persent
           console.log(_presentProgress)
        }
    }

    ListModel {
        id:             fileModel

        ListElement {
            fileName:   "file 1"
//            fileIcon: IconType.file
        }

        ListElement {
            fileName:   "file 2"
//            fileIcon: IconType.file
        }
    }

    FileDialog {
        id:         fileDialogf
        title:      "Please choose a file"
        folder:     shortcuts.home
        onAccepted: {
            filepatch = fileDialogf.fileUrl;
            fileDialogf.close()
            fileModel.append({"fileName": filepatch});
        }
        onRejected: {
            console.log("Canceled")
            fileDialog.close()
        }
    }

    AppButton {
        icon:                   IconType.plus
        anchors.right:          parent.right
        anchors.rightMargin:    dp(10)
        anchors.bottom:         parent.bottom
        anchors.bottomMargin:   dp(10)
        onClicked: {
            fileDialogf.open()
        }
    }


    AppListView {
        id:                 listFileUser
        anchors.top:        parent.top
        anchors.left:       parent.left
        anchors.topMargin:  parent.width * 0.01
        width:              parent.width * 0.8
        height:             parent.height - 70
        //backgroundColor:    "lightsteelblue"
        delegate: Item {

            width:          parent.width
            height:         dp(50)

            AppImage {
                id:             imFile
                width:          dp(25)
                height:         dp(25)
                defaultSource:  Qt.resolvedUrl("qrc:qml/images/file.jpg")
                anchors {
                    verticalCenter:     parent.verticalCenter
                    left:               parent.left
                    leftMargin:         dp(10)
                }
            }
            AppText {
                id:         txtFile
                text:       fileName
                font.pixelSize: sp(16)
                anchors {
                    verticalCenter:     parent.verticalCenter
                    left:               imFile.right
                    leftMargin:         dp(10)
                }
            }

            AppButton {
                icon:                   IconType.send
                anchors.right:          parent.right
                //anchors.rightMargin:    dp(10)
                anchors.bottom:         parent.bottom
                //anchors.bottomMargin:   dp(10)
                anchors.verticalCenter: parent.verticalCenter
                onClicked: {
                    //manageConnecion.sendRequestUpload(filepatch);
                    //manageConnecion.share_File(_userName, currentUser,filepatch);
                    manageConnecion.share_File_Save_Server(_userName, currentUser,filepatch);
                }

            }

        }
        model: fileModel
    }

    Rectangle {
        id: infoUser
        anchors {
            top:            parent.top
            left:           listFileUser.right
            right:          parent.right
            rightMargin:    parent.width * 0.01
            leftMargin:     parent.width * 0.01
            topMargin:      parent.width * 0.01
        }

        Icon {
            id:         avataUser
            icon:       IconType.user
            anchors{
                top:        parent.top
                left:       parent.left
            }
            width:      parent.width
            height:     parent.height * 0.7
            size:       px(40)
        }

        Text {
            id: txtUserName
            anchors{
                top:        avataUser.bottom
                left:       parent.left
                centerIn:   parent
            }

            text:           currentUser
            font.pixelSize: parent.width * 0.10;

        }

        width:      parent.width * 0.18
        height:     parent.height * 0.4
        //color:      "lightsteelblue"
    }


    ProgressBar {
        id:                         progressbar
        anchors.bottom:             parent.bottom
        anchors.horizontalCenter:   parent.horizontalCenter
        width:                      parent.width * 0.6
        height:                     px(60)
        value:                      _presentProgress
        from:                       0
        to:                         100
    }

//    Text {
//        id: txtProgress
//        anchors{
//            top:        progressbar.bottom
//            centerIn:   parent
//        }

//        text:           _presentProgress
//        font.pixelSize: px(15)

//    }
}
