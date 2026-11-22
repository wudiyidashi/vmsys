import QtQuick
import QtQuick.Controls 6.2

Item {
    id: item1
    Grid {
        verticalItemAlignment: Grid.AlignVCenter
        horizontalItemAlignment: Grid.AlignHCenter
        flow: Grid.TopToBottom
        layoutDirection: Qt.LeftToRight
        spacing: 5
        columns: 2
        rows: 1
        FlatSwitch {
                       checkedColor: "#727CF5"
                   }

        FlatSwitch {

            id: switch1

            text: qsTr("推杆控制")

            onToggled:
            {
                console.log(checked)
                McuBoard.drawerControl(checked);
            }
        }

    }



//    Timer {
//        interval: 1000
//        running: true
//        repeat: true
//        //        onTriggered: McuBoard.drawerControl(switch1.checked);
//    }



}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
