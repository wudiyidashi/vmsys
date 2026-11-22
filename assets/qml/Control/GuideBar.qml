import QtQuick

Item {
    id: root;
    //显示区域的颜色背景

    property bool unfold: false;
    property int selectTopPadding: 15;
    property int selectBottomPadding: 15;
    property int selectSpacing: 10;
    property int selectCompomonetHeight: barRect.height/appModel.count-10;

    signal unfoldChange(bool unf)

    function buttoned(name){
        console.log(name)
        switch(name)
        {
        case "地图":
            $switchView("home")
              break;
        case "设置":
            $switchView("setting")
              break;
        case "充电":
             MissionManager.publishChargeTask();
            $message({
                          "message": '正在前往！',
                          "type": 'success',
                          "show": true
                     })
              break;
        case "推杆上升":
                McuBoard.drawerControl(0);
            $message({
                          "message": '发送成功！',
                          "type": 'success',
                          "show": true
                     })
              break;
        case "推杆下降":
                McuBoard.drawerControl(1);
            $message({
                          "message": '发送成功！',
                          "type": 'success',
                          "show": true
                     })
              break;
        case "退出":
             Qt.callLater(Qt.quit);
              break;
        }
    }
    Rectangle{
        id: barRect;
        anchors.fill: parent

        radius: 10;
        anchors.centerIn: parent;
        color: "white";
        clip: true;

        Behavior on width{
            NumberAnimation{duration: 300;}
        }

            ListModel{
                id: appModel;
                ListElement{
                    name: "地图";
                    icon: "qrc:/images/set.png";

                }
                ListElement{
                    name: "设置";
                    icon: "qrc:/images/setting.png";
                }
                ListElement{
                    name: "充电";
                    icon: "qrc:/images/power.png";
                }
                ListElement{
                name: "推杆上升";
                icon: "qrc:/images/Up.png";
            }
            ListElement{
                name: "推杆下降";
                icon: "qrc:/images/Down.png";
            }
            ListElement{
                name: "退出";
                icon: "qrc:/images/exit.png";
            }
        }

        Component{
            id: appDelegate;
            Rectangle {
                id: delegateBackground;
                width: barRect.width;
                height: root.selectCompomonetHeight;
                radius: 5;
                color: "#FFFFFF";
                //显示图标
                Image {
                    id: imageIcon;
                    width: selectCompomonetHeight/1.84;//70;
                    height: selectCompomonetHeight/1.84;
                    anchors.verticalCenter: parent.verticalCenter;
                    anchors.left: parent.left;
                    anchors.leftMargin: barRect.width/8.34;//35
                    mipmap: true;
                    source: icon;
                }
                //显示APP文字
                Text {
                    anchors.left: imageIcon.right;
                    anchors.leftMargin: barRect.width/7.3;//40
                    anchors.verticalCenter: imageIcon.verticalCenter;
                    color: "#0099FF"
                    text: name;
                    font{family: "微软雅黑"; pixelSize: imageIcon.height*0.4;bold: true
                    }
                }
                //鼠标处理
                MouseArea{
                    anchors.fill: parent;
                    hoverEnabled: true;
                    onEntered: delegateBackground.color = "#10000000";
                    onExited: delegateBackground.color = "#00000000";
                    onClicked: root.buttoned(name);
                }
            }
        }




        Column{
            anchors.fill: parent;
            spacing: root.selectSpacing;
            topPadding: root.selectTopPadding;
            bottomPadding: root.selectBottomPadding;
            Repeater{
                model: appModel;
                delegate: appDelegate;
            }

        }
}



}
