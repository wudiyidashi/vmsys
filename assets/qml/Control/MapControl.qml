import QtQuick
import QtQuick.Controls
import MapUnit

Item {
    Rectangle {
        anchors.fill: parent
        anchors.margins: 15
        radius: 10
        color: "white"
        clip: true

        MapUnit {
            anchors.fill: parent
        }

        Rectangle {
            anchors.top: parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.topMargin: 14
            width: statusLabel.implicitWidth + 24
            height: statusLabel.implicitHeight + 12
            visible: !RobotMap.mapReady
            color: "#f7f9fc"
            border.color: "#e2e8f0"
            radius: 7

            Label {
                id: statusLabel
                anchors.centerIn: parent
                text: !RobotStatus.connectionState ? qsTr("等待机器人连接")
                      : RobotMap.mapName ? qsTr("正在同步地图") : qsTr("等待机器人地图")
                color: "#526579"
            }
        }
    }
}
