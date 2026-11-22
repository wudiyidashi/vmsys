import QtQuick

import QtQuick.Controls 6.2
import QtQuick.Shapes
import QtQuick.Layouts

Item {


    width: 100;
    height: 100


    property bool isConnected: true
    property alias titlely:linkstatusr.text
    property alias body:context.text
        Rectangle {
            id: delegateBack;
            width: 200;
            height:70;
            radius: 5;
            color: "#FFFFFF";
            Rectangle {
                id: linkstatusRece;
                width: 190;
                height: 45;
                color: "white"
                anchors.left: parent.left;
                anchors.verticalCenter: delegateBack.verticalCenter;
                Text {
                    id: linkstatusr;
                    anchors.centerIn: parent
                    color: "#0099FF"
                    text: name;
                    font{family: "微软雅黑"; pixelSize: linkstatusRece.height*0.5;bold: true
                    }
                }
            }
            Rectangle {
                id:condit
                width: 100;
                height: 45;
                anchors.left: linkstatusRece.right;
                anchors.verticalCenter: linkstatusRece.verticalCenter;
                radius: 10
                color: "white"

                Text {
                    id:context
                    anchors.centerIn: parent
                    //anchors.centerIn: parent
                    text:"数据"                           
                    color: "black"
                    font {
                        pointSize: condit.height*0.5
                        bold: true
                    }
                }
            }
        }


}
