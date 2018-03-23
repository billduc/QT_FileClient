import VPlayApps 1.0
import QtQuick 2.0
import "../src/variables/fontawesome.js" as FontAwesome
import "../src/lists"

Page {

    title: "List Users"
    width: parent.width
    height: parent.height
    AvatarListView{
        anchors.fill: parent

        model: ListModel {
            ListElement{
                avatar: "qrc:qml/images/venkman.jpg"
                title:"user2"
                detail: "Back off, man. I'm a scientist."
            }
            ListElement{
                avatar: "qrc:qml/images/spengler.jpg"
                title:"user3"
                detail: "We're gonna go full stream."
            }
            ListElement{
                avatar: "qrc:qml/images/barrett.jpg"
                title:"user4"
                detail: "Ugly little spud, isn't he?"
            }
            ListElement{
                avatar: "qrc:qml/images/winston.jpg"
                title:"user5"
                detail: "That's a big Twinkie."
            }
            ListElement{
                avatar: "qrc:qml/images/tully.jpg"
                title:"user6"
                detail: "Okay, who brought the dog?"
            }
            ListElement{
                avatar: "qrc:qml/images/barrett.jpg"
                title:"user7"
                detail: "I am The Gatekeeper!"
            }
            ListElement{
                avatar: "qrc:qml/images/slimer.jpg"
                title:"user8"
                detail: "Boo!"
            }
        }
    }

}
