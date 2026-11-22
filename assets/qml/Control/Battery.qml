import QtQuick
import QtQuick.Controls

Item {
    id:rootbat
    property int batwidth: 200;
    property int batheight: 80;
    property int volume: 100;
    Rectangle {
        id:righticon
        width:20
        height: 30
        x:background.width-width/2
        y:(background.height-height)/2
        color: "#d3d3d3"
        radius: 10

    }

    Rectangle {
        //边框
        id:background
        width: batwidth
        height: batheight
         color: "#d3d3d3";
        //border.color: "#0f0f0f"
        //border.width: 3
        radius: 10
    }

    Rectangle {
        //右侧的缝隙
        x:righticon.x+righticon.width/2
        y:righticon.y
        width: 2
        height: righticon.height
         opacity: 0.9
        //border.color: "#0f0f0f"
        //border.width: 3
    }

    Rectangle {
        //电池主体
        anchors.left: background.left
        anchors.leftMargin: 1
        anchors.top: background.top
        anchors.topMargin: 1
        width:(background.width-2)*0.01*Bat.power
        height: background.height-2
        color: "#33CC33"
        radius: 10
    }

    Text {
        id: name
        anchors.centerIn: background
        color:"#666666"
        font {
            pointSize: (background.height-2)*0.5
            bold: true
        }
        text: Bat.power+"%"
    }

}
