import VPlayApps 1.0
import QtQuick 2.2
import QtQuick.Dialogs 1.0
import managerConnecion 1.0
import Connection 1.0
import QtGraphicalEffects 1.0
import QtQuick.Controls 1.4
import QtQuick.Controls 2.3

Popup
 {
    id:                                 popupProgress
    visible:                            _showPopupProgress
    width:                              500
    height:                             30
    property real _positionX:           parent.width
    property real _positionY:           parent.height
    property real _presentProgress:     0
//    Loader {
//        id: loader
//        source: "../Main.qml"
//    }

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

    x: _positionX
    y: _positionY

    Text {
        id: txtProgress
        anchors{
            top:        progressbar.bottom

        }

        text:           _presentProgress + "%"
        font.pixelSize: px(15)

    }


}
