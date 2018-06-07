import QtQuick 2.9

Item {

    id: container

    property int circleSize : 10

    x: 0
    y: 0
    z: 2

    scale: mapItem.scale

    onXChanged: {
        if (mouseArea.drag.active) {
            mapItem.position.x = x
        }
    }
    onYChanged: {
        if (mouseArea.drag.active) {
          mapItem.position.x = y
        }
    }

    Component.onCompleted: {
        x = parseInt(mapItem.position.x)
        y = parseInt(mapItem.position.y)
    }

    Rectangle {

         width: 10
         height: 10
         color: mapItem.color
         border.color: (mapItem.hasFocus) ? "#3195bb" : "black"
         border.width: 2
         radius: width*0.5

         //center the circle.
         anchors.verticalCenter: parent.top
         anchors.horizontalCenter: parent.left

         MouseArea {

             id: mouseArea

             anchors.fill: parent
             onPressed: {
                 mapItem.setAsCurrentItem();
             }

             drag.target: container
             drag.axis: Drag.XandYAxis
         }
    }

    TextEdit {
        id: nameTextEdit
        color: (mapItem.hasFocus) ? "#ffffff" : "black"
        font.family: "Helvetica"
        font.bold: true
        horizontalAlignment: Text.AlignHCenter
        font.pointSize: 10
        text: mapItem.itemName

        anchors.bottom: parent.top
        anchors.bottomMargin: parent.circleSize/2 + 2

        anchors.horizontalCenter: parent.left

        onEditingFinished: {
            mapItem.itemName = nameTextEdit.text;
        }

        readOnly: mapItem.isLinked

        MouseArea {
           anchors.fill: parent

           onClicked: mouse.accepted = false;
           onPressed: {
               mapItem.setAsCurrentItem()
               mouse.accepted = false
           }
           onReleased: mouse.accepted = false;
           onDoubleClicked: mouse.accepted = false;
           onPositionChanged: mouse.accepted = false;
           onPressAndHold: mouse.accepted = false;
        }

    }

}
