import QtQuick

Item {
    id:button_root
    property alias button_text:rect_text.text
    property alias tittle_text:tittle.text
    property alias text_input_rec:text_input_rec
    property alias text_input:text_input
//    text_input
    signal btnclicked
    Row {
        spacing: 10
        anchors.left: parent.left;
        anchors.leftMargin: 10
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: 10
        Text {
            id:tittle
            text: "车号" ; font.pointSize: 50
            color: "#0099FF"
            verticalAlignment: Text.AlignVCenter }

        Rectangle {
            id:text_input_rec
            width: 100
            height: 80

            color: "#CCCCCC"
            border.color: "#f0f0f0"
            radius: 5;

            TextInput {
                id:text_input
                clip: true
                anchors.fill: parent
                anchors.margins: 2
                font.pointSize: 45
                focus: true
            }
        }
        Rectangle
        {
            id:butten
            width:150
            height:80
            radius:6

            color: rect_mouse.pressed ? "#01968c" : (rect_mouse.containsMouse ? "#20c9b3" : "#00beac")


            Text
            {
                id:rect_text
                font.pointSize: 50
                font.bold: true
                color:"#ffffff"
                anchors.horizontalCenter:parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                anchors.verticalCenterOffset: 0
                text:"确认"
            }


            MouseArea
            {
                id:rect_mouse
                anchors.fill: parent
                hoverEnabled: true //是否处理悬浮事件，默认false，只有按下鼠标键时才处理鼠标事件，为true时即使没有按下鼠标键也会作相应的处理
                preventStealing:true//默认为false，为true时可以防止当前鼠标事件被其它对象拦截。
                propagateComposedEvents:true//默认为 false，当设置为 true 时，就可以将事件传递给重叠的其他鼠标区域了
                enabled: true
                cursorShape: Qt.PointingHandCursor

                onPressed:
                {
                    rect_text.anchors.verticalCenterOffset = 5
                }

                onReleased:
                {
                    if(rect_mouse.containsMouse)//鼠标在按钮范围才有效
                        button_root.btnclicked()

                    rect_text.anchors.verticalCenterOffset = 0
                }
            }
        }


    }
}
