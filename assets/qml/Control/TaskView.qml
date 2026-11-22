import QtQuick
import QtQuick.Controls 6.2
import QtQuick.Shapes
import QtQuick.Layouts

Item {
    id:root

    property int pad: parent.height*0.33;

    //用于控件3的高度
     property int layout3_height: pad*0.8;
    //用于控件2的高度
     property int layout2_height: pad;
    //用于控件1的高度
     property int layout1_height:  pad;
    ColumnLayout
    {
        id:layout
        spacing: 5
        ColumnLayout{
            id:layout1
            Layout.preferredHeight: layout1_height
            Layout.preferredWidth: parent.width


            Rectangle{
                Layout.preferredHeight: layout1_height.height*0.1
                Layout.alignment: Qt.AlignCenter
            }

            Text {
                Layout.preferredHeight: layout1_height.height*0.33
                Layout.alignment: Qt.AlignCenter
                text: MissionManager.group_text1_title
                color: "#0099FF"
                font {
                    pointSize: (root.height*0.6-12)*0.08
                    bold: true
                }
                horizontalAlignment: Text.AlignHCenter
            }
            Text {
                Layout.alignment: Qt.AlignCenter
                Layout.preferredHeight: layout1_height.height*0.33//60
                //anchors.centerI(root.height*0.6)*0.1n: parent
                text: MissionManager.group_text1_content
                color: "#0099FF"
                font {
                    pointSize:(root.height*0.6-12)*0.1
                    bold: true
                }
            }
            Rectangle{
                color: "#E0E0E0"
                Layout.preferredHeight: 3
                Layout.preferredWidth: parent.width
                Layout.alignment: Qt.AlignCenter
            }

        }

        ColumnLayout{
            id:layout2
            Layout.preferredHeight: layout2_height
            Layout.preferredWidth: root.width

            Rectangle{
                Layout.preferredWidth: root.width
                Layout.preferredHeight:  layout2_height*0.4
                Image {
                    anchors.centerIn: parent
                    id: name
                    visible: false
                    width: 100
                    height: 100
                    source: "../../images/pause.png"
                }
                Arrow {
                    anchors.centerIn: parent
                    width: root.width
                    height: 100
                    icon_width:root.width-1
                }
            }

            Text {
                id:textlink
                width: 100
                height: 100
                Layout.alignment: Qt.AlignCenter
                text: MissionManager.mid_content
                color: "#0099FF"
                font {
                    pointSize: (root.height*0.6-12)*0.08
                    bold: true
                }
                horizontalAlignment: Text.AlignHCenter
            }
            Rectangle{
                Layout.preferredHeight: 10
            }
            Rectangle{
                color: "#E0E0E0"
                Layout.preferredHeight: 3
                Layout.preferredWidth: parent.width
                Layout.alignment: Qt.AlignCenter
            }
        }
        ColumnLayout{
            id:layout3
            Layout.preferredHeight: layout3_height
            Layout.preferredWidth: root.width

            Rectangle{

                Layout.preferredHeight: layout3_height*0.1
            }

            Text {
                Layout.preferredHeight:layout3_height*0.33
                Layout.alignment: Qt.AlignCenter
                text: MissionManager.group_text2_title
                color: "#0099FF"
                font {
                    pointSize: (root.height*0.6-12)*0.08
                    bold: true
                }
                horizontalAlignment: Text.AlignHCenter
            }
            Text {
                Layout.alignment: Qt.AlignCenter
                Layout.preferredHeight: layout3_height*0.33
                //anchors.centerIn: parent
                text: MissionManager.group_text2_content
                color: "#0099FF"
                font {
                    pointSize: (root.height*0.6-12)*0.1
                    bold: true
                }
            }
        }


    }

}






//        RadialBarShape {
//            value: feeder.value
//        }
//    QtObject {
//            id: feeder

//            property real value: 0

//            NumberAnimation on value{
//                    from: 0
//                    to: 100
//                    loops: Animation.Infinite
//                    duration: 2000
//                }
//        }




/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
