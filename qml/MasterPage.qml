import VPlayApps 1.0
import QtQuick 2.0


ListPage {
    id: masterPage

    title: qsTr("Master")

    rightBarItem: IconButtonBarItem {
        icon: IconType.plus

        onClicked: {
            // Add a new item and notify listview model
            masterPage.model.push({ text: new Date() })
            masterPage.modelChanged()
        }
    }

    model: []

    delegate: SwipeOptionsContainer {
        id: container

        rightOption: AppButton {
            text: qsTr("Delete")

            onClicked: {
                container.hideOptions()

                // Remove selected item and notify listview model
                masterPage.model.splice(index, 1)
                masterPage.modelChanged()
            }
        }

        SimpleRow {
            onSelected: {
                masterPage.navigationStack.popAllExceptFirstAndPush(Qt.resolvedUrl("DetailPage.qml"), { content: item.text })
            }
        }
    }
}
