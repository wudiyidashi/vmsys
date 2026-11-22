import QtQuick

import QtQuick.Controls 6.2
import QtQuick.Shapes
import QtQuick.Layouts

Item {
    width: 100;
    height: 100

    property bool isConnected: true
    property alias title: linkstatus.text
        Rectangle {
            id: delegateBackground;
            width: 200;
            height:70;
            radius: 5;
            color: "#FFFFFF";
            Rectangle {
                id: linkstatusRec;
                width: 190;
                height: 45;
                color: "white"
                anchors.left: parent.left;
                anchors.verticalCenter: delegateBackground.verticalCenter;
                Text {
                    id: linkstatus;
                    anchors.centerIn: parent
                    color: "#0099FF"
                    text: name;
                    font{family: "微软雅黑"; pixelSize: linkstatusRec.height*0.5;bold: true
                    }
                }
            }
            Rectangle {
                id:condition
                width: 100;
                height: 45;
                anchors.left: linkstatusRec.right;
                anchors.verticalCenter: linkstatusRec.verticalCenter;
                radius: 10
                color: isConnected?"green":"red";
                    //connected===false?"green":"red";

                Text {
                    anchors.centerIn: parent
                    //anchors.centerIn: parent
                    text:isConnected?"已连接":"未连接";
                    color: "black"
                    font {
                        pointSize: condition.height*0.5
                        bold: true
                    }
                }
            }
        }
}



