
import VPlayApps 1.0
import QtQuick 2.0
import QtQuick.Controls 2.0 as Controls
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0
import managerConnecion 1.0

Page {
    id: loginPage
    signal loginSucceeded
    property bool waiting: false
    backgroundColor: "white"
    //     login form background

    Rectangle {
        id:                 loginForm
        anchors.centerIn:   parent
        color:              "transparent"
        width:              content.width +title.width+ dp(48)
        height:             content.height+title.height + dp(16)
        radius:             dp(4)

        // login form content
        GridLayout {
            id:                         content
            anchors.horizontalCenter:   parent.horizontalCenter
            anchors.top:                title.bottom
            anchors.topMargin:          dp(20)
            rowSpacing:                 rowSpacingSize
            columns:                    1

            property real itemSize:         Qt.platform.os === "ios"?dp(250):dp(200)
            property real imageSize:        Qt.platform.os === "ios"?dp(30):dp(15)
            property real textSize:         Qt.platform.os === "ios"?sp(20):sp(15)
            property real checkBoxSize:     Qt.platform.os === "ios"?dp(20):dp(13)
            property real rowSpacingSize:   Qt.platform.os === "ios"?dp(15):dp(10)

            Item {
                id:                     itemUsername
                Layout.preferredWidth:  content.itemSize
                height:                 txtUsername.implicitHeight

                AppTextField {
                    id:                 txtUsername
                    text:               qsTr("user1")
                    showClearButton:    true
                    font.pixelSize:     content.textSize
                    placeholderText:    qsTr("Username")
                    placeholderColor:   "grey"
                    backgroundColor:    "transparent"
                    textColor:          "black"
                    enabled:            waiting ? false:true
                    anchors.left:       parent.left
                    anchors.leftMargin: dp(5)
                    anchors.right:      parent.right
                }
                Rectangle {
                    width:              parent.width
                    height:             dp(1)
                    color:              "blue"
                    anchors.bottom:     parent.bottom
                }
            }
            Item {
                id:                     itemPassword
                Layout.preferredWidth:  content.itemSize
                height:                 txtUsername.implicitHeight

                AppTextField {
                    id:                 txtPassword
                    text:               qsTr("user1")
                    showClearButton:    true
                    font.pixelSize:     content.textSize
                    placeholderText:    qsTr("Password")
                    placeholderColor:   "grey"
                    backgroundColor:    "transparent"
                    textColor:          "black"
                    echoMode:           TextInput.Password
                    enabled:            waiting ? false:true
                    anchors.left:       parent.left
                    anchors.leftMargin: dp(5)
                    anchors.right:      parent.right
                }
                Rectangle {
                    width:              parent.width
                    height:             dp(1)
                    color:              "blue"
                    anchors.bottom:     parent.bottom
                }
            }


            // buttons
            Rectangle {
                id:                     itemLogin
                Layout.preferredWidth:  content.itemSize
                height:                 txtUsername.height + dp(30)
                color:                  "transparent"
                anchors.topMargin:      dp(10)
                AppButton {
                    text:               qsTr("LOGIN")
                    flat:               false
                    minimumWidth:       dp(200)
                    anchors.fill:       itemLogin
                    minimumHeight:      txtUsername.height
                    anchors.bottom:     itemLogin.bottom
                    backgroundColor:    "#137dd4"
                    textColor:          "white"
                    onClicked: {
                        if(txtUsername.text != "" && txtPassword.text != "")
                        {
                            loginManual()
                        }
                    }
                }
            }

            Item {
                id:                     itemStatus
                anchors.topMargin:      dp(10)
                anchors.top:            itemLogin.bottom
                Text {
                    id:                 statusLogin
                    text:               qsTr("");
                    color:              "red"
                }
            }
        }
        Keys.onReturnPressed: {
            if(txtUsername.text != "" && txtPassword.text != "")
            {
                loginManual()
            }
        }
        Keys.onEnterPressed: {
            if(txtUsername.text != "" && txtPassword.text != "")
            {
                loginManual()
            }
        }
    }

    function loginManual(){
        //Boolean check = manageConnecion.auth_Connection(txtUsername.text,txtPassword.text);

        if (manageConnecion.main_connectToServer("localhost","443.") )
            console.log("connection success");
        else{
            console.log("conneciton fail");
            statusLogin.text = "cannot connect to server";
            return
        }

        if ( manageConnecion.auth_Connection(txtUsername.text,txtPassword.text) == true){
            //container.state = qsTr("Master");
            //container.state = qsTr("User");
            container.state = qsTr("listUsers");
            _userName = txtUsername.text
            _password = txtPassword.text
        }
        else{
            console.log("login fail");
            statusLogin.text = "Invalid username or password.";
        }
    }

}
