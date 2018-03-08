import VPlayApps 1.0
import QtQuick 2.0
import managerConnecion 1.0

App {
    // You get free licenseKeys from https://v-play.net/licenseKey
    // With a licenseKey you can:
    //  * Publish your games & apps for the app stores
    //  * Remove the V-Play Splash Screen or set a custom one (available with the Pro Licenses)
    //  * Add plugins to monetize, analyze & improve your apps (available with the Pro Licenses)
    //licenseKey: "<generate one from https://v-play.net/licenseKey>"

    property string currentUser:    ""
    property string _userName:      ""
    property string _password:      ""

    ManageConnection{
        id: manageConnecion;
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
                }


            ]
            Component.onCompleted:
            {
                container.state = "Login"

            }
        }
}
