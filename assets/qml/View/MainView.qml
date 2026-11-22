import QtQuick
import QtQuick.Layouts
import "../Control/"

Item {
    function switchPage(page) {
        if (page === "home")
            pages.currentIndex = 0
        else if (page === "setting")
            pages.currentIndex = 1
    }

    StackLayout {
        id: pages
        objectName: "mainPages"
        anchors.fill: parent
        MapControl {}
        Setting {}
    }
}
