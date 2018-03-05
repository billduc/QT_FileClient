import VPlayApps 1.0
import QtQuick 2.0

Page {

    AppListView {
        delegate: SimpleRow {
            onSelected: container.state = "Master"
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
