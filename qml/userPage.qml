import VPlayApps 1.0
import QtQuick 2.0

Page {
    id: userPage
    title: "Users"
    //Component { id:
    AppListView {
        id: listUsers
        delegate: SimpleRow {
            //onSelected: container.state = "Master"
            onSelected: {
                currentUser = text
                container.state = "File"
            }
        }
        model: [
          { text: "user2",
            icon: IconType.user },
          { text: "user3",
            icon: IconType.user },
          { text: "user4",
            icon: IconType.user }
        ]
    }
}
