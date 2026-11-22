import QtQuick 2.15
import QtQuick.Layouts
import QtQuick.Controls
import "../Control/"


Item {
    id:root

    ColumnLayout
    {
        id:lauout2
        spacing: 6

        Rectangle {
            id:timelab
            Layout.rowSpan: 2
            Layout.fillWidth: true
            Layout.preferredWidth: root.width
            Layout.preferredHeight: root.height*0.2
            radius: 10

            Label {
                id: timeLabel
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top:parent.top
                anchors.topMargin: timelab.height*0.17//16
                color: "#0099FF"
                font {
                    pointSize: timelab.height*0.25
                    bold: true

                }


            }
            Label {
                id: timeLabel2
                color: "#0099FF"
                anchors.top: timeLabel.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.topMargin: 5
                font {
                    pointSize: timelab.height*0.15
                    bold: true
                }
            }

            Timer {

                    interval: 500
                    running: true
                    repeat: true
                    onTriggered:
                    {
                        timeLabel.text = Qt.formatTime(new Date(), "hh:mm:ss")
                        timeLabel2.text = Qt.formatDateTime(new Date(),"ddd MM-dd")
                    }
                }
        }


        Rectangle {
            id:batery
            Layout.rowSpan: 2
            Layout.fillWidth: true
            Layout.preferredWidth: root.width
            Layout.preferredHeight: root.height*0.2
            radius: 10
            Battery{
                batwidth: batery.width*0.8//250
                batheight: batwidth*0.4
                x:(parent.width-batwidth)/2
                y:(parent.height-batheight)/2
            }
        }

        Rectangle {
            id:condition
            Layout.preferredWidth: root.width
            Layout.preferredHeight:root.height*0.6-lauout2.spacing*2
            radius: 10

            TaskView {
                anchors.fill: parent
            }
        }





    }


}
