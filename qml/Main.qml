import VPlayApps 1.0
import QtQuick 2.0
import QtQuick 2.3
import QtQuick.Window 2.2
import managerConnecion 1.0
import QtQuick.Controls 2.0
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import "src/lists"
import "src/bars"
import "src/buttons"
import "src/variables/fontawesome.js" as FontAwesome

App {
    // You get free licenseKeys from https://v-play.net/licenseKey
    // With a licenseKey you can:
    //  * Publish your games & apps for the app stores
    //  * Remove the V-Play Splash Screen or set a custom one (available with the Pro Licenses)
    //  * Add plugins to monetize, analyze & improve your apps (available with the Pro Licenses)
    //licenseKey: "<generate one from https://v-play.net/licenseKey>"

    property string currentUser:            ""
    property string _userName:              ""
    property string _password:              ""
    property string _sender:                ""
    property string _receiver:              ""
    property string _fileName:              ""
    property string _fileSize:              ""
    property bool   _isFileStransfer:       flase

    //FontLoader{ source: "qrc:/src/fonts/fontawesome-webfont.ttf"}

    ManageConnection{
        id: manageConnecion
    }

    Connections {
        target: manageConnecion
        onSignal_Notify_Download: {
           _sender      = sender
           _receiver    = receiver
           _fileName    = fileName
           _fileSize    = fileSize
           popupDownload.open();
        }
    }

    Component.onCompleted: {
    }


    Rectangle{
            id:     container
            width:  parent.width
            height: parent.height

            Loader{
                id:             mainLoader
                anchors.fill:   parent
            }
            states: [
                State{
                    name: "Login"
                    PropertyChanges {
                        target:     mainLoader
                        source:     Qt.resolvedUrl("login.qml")
                    }
                },
                State{
                    name: "Master"
                    PropertyChanges {
                        target:     mainLoader
                        source:     Qt.resolvedUrl("MasterPage.qml")
                    }
                },
                State{
                    name: "User"
                    PropertyChanges {
                        target:     mainLoader
                        source:     Qt.resolvedUrl("userPage.qml");
                    }
                },
                State{
                    name: "File"
                    PropertyChanges {
                        target:     mainLoader
                        source:     Qt.resolvedUrl("filePage.qml");
                    }
                },
                State{
                    name: "listUsers"
                    PropertyChanges {
                        target:     mainLoader
                        source:     Qt.resolvedUrl("listUserPage.qml");
                    }
                }


            ]
            Component.onCompleted:
            {
                container.state = "Login"
            }
        }






    Popup {
         id: popupDownload
         x: 350
         y: 50
         width: 200
         height: 250
         modal: true
         focus: true
         closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
         ColumnLayout {
            anchors.fill: parent
            Text {
                id:     _txtFileName
                text:   _fileName
            }
            Text {
                id:     _txtFileSize
                text:   _fileSize
            }
            Text {
                id:     _txtSender
                text:   _sender
            }
            Text {
                id:     _txtReceiver
                text:   _receiver
            }
            Button {
                text:       "Download"
                onClicked: {
                    popupDownload.close();
                    dowloadFile()
                }
            }
            Button{
                text:       "Cancel"
                onClicked:  popupDownload.close()
            }
         }
    }


    function dowloadFile(){
        console.log("click download")
        //manageConnecion.receive_File(_fileName, _fileSize);
        manageConnecion.receive_File_Save_Server(_fileName, _fileSize);
    }
}
