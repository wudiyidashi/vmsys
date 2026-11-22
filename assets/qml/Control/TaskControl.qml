import QtQuick

Item {
    // Text explaining whats happening
    Row
    {
        spacing: 8

        // Rotating icon
        Text
        {
            id: loadingSpinner

//            font.family: fontAwesome.name
            text: "\uf021"
            font.pixelSize: 18
            color: "#0091f8";

            NumberAnimation on rotation {
                from: 0; to: 360; running: loadingSpinner.visible == true;
                loops: Animation.Infinite; duration: 700;
            }
        }

        // Text explaining whats happening
        Text {
            text: "Updating..."
            font.family: "Lato"
            font.pixelSize: 16
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
