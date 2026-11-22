import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts


import "./View/"
import "./Control/"
Window {
    id:root_window
    visible: true
    width:Screen.desktopAvailableWidth
    height:Screen.desktopAvailableHeight


    visibility:Window.FullScreen                // Show FullScreen
    color: "#f0f0f4"

    Component.onCompleted:console.log(root_window.height);

    property int leftpaddint: 15;

    property int topmargin: 15;
    //导航条左侧偏移位置
    property int guidebar_left_margin: -150;

    MainView
    {
     id:mainView

    width: root_window.width*0.6
    height:  root_window.height

    x:(root_window.width-width)/2
      //;
    y:0;

    }
    LeftView
    {
        id:leftView
        //减去右侧空白空间
        width:root_window.width*0.2-15
        height: root_window.height-topmargin*2

        x:15
        y:topmargin

    }

       RightView
       {
           id:rightView
           //减去右侧空白空间
           width:root_window.width*0.2-15
           height: root_window.height-topmargin*2

           x:root_window.width*0.8
           y:topmargin
        }
    function showPopupCenter(raiseItem) {
        popupCenter.raiseItem = raiseItem
        popupCenter.open()
    }

    Popup {
        id:             popupCenter
        modal:          true            //模态， 为 true后弹出窗口会叠加一个独特的背景调光效果
        focus:          true            //焦点,  当弹出窗口实际接收到焦点时，activeFocus将为真
        padding:        0
        closePolicy:    Popup.CloseOnEscape | Popup.CloseOnPressOutside
        property var    raiseItem:          null
        background: Rectangle {
            color:      Qt.rgba(0,0,0,0)   //背景为无色
        }
        Loader {
            id:             loaderCenter
            onLoaded: {
                popupCenter.x = (rootWindow.width - loaderCenter.width) * 0.5
                popupCenter.y = (rootWindow.height - loaderCenter.height) * 0.5
            }
        }
        onOpened: {
            loaderCenter.sourceComponent = popupCenter.raiseItem
        }
        onClosed: {
            loaderCenter.sourceComponent = null
            popupCenter.raiseItem = null
        }
    }

    TopMessage{

           id:message
       }
    function $switchView(index)
    {
            mainView.switchPage(index)
    }

       function $message(data){
           if(data&&data.show){
               message.show=data.show
           }
           if(data&&data.message){
               message.text=data.message
           }

           if(data&&data.type){
               message.type=data.type
           }

           if(data&&data.type&&data.type!=='success'&&data.type!=='error'&&data.type!=='info'){
               return false
           }

           message.openTimer()  //调用Message组件下定时器方法
       }



}



