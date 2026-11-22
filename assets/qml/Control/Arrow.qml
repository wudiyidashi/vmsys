import QtQuick

import ArrowHeads
Item {
        property int icon_width: 200
        property real index: 0
        property bool isVisable: true
        //! [0]
        ListModel {
            id: balloonModel
            ListElement {
                balloonColor: "#99CCFF"
                balloonOpacity: 1
            }

            ListElement {
                balloonColor: "#99CCFF"
                balloonOpacity: 1
            }

            ListElement {
                balloonColor: "#99CCFF"
                balloonOpacity: 1
            }

            ListElement {
                balloonColor: "#99CCFF"
                balloonOpacity: 1
            }
        }

        ListView {
//            anchors.bottom: controls.top
//            anchors.bottomMargin: 2
            y:30
            orientation:Qt.Horizontal
            id: balloonView
            delegate: ArrowHeads {
                height: icon_width/6
                 width: icon_width/5
                 color:balloonColor
                 opacity:balloonOpacity
            }

            focus: true
            model: balloonModel
            spacing: 5
            width: parent.width
        }

        Timer {
            id:time1
            interval: 400;  running:true ;repeat: true
            onTriggered:
            {
                if(isVisable)
                {
                     balloonView.model.setProperty(3,"balloonOpacity",1)
                     balloonView.model.setProperty(0,"balloonOpacity",0)
                    isVisable=false;
                }
                else
                {
                    balloonView.model.setProperty(3,"balloonOpacity",0)
                    balloonView.model.setProperty(0,"balloonOpacity",1)
                    isVisable=true;
                }

            }
            }


        }
        //! [1]




