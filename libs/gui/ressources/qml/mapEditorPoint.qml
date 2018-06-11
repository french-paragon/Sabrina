import QtQuick 2.9
import SabrinaCartography 0.1

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
         border.color: (mapItem.hasFocus) ? "white" : "black"
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
             preventStealing: true
         }
    }

    TextEdit {

        property int margin: parent.circleSize/2 + 2

        id: nameTextEdit
        color: (mapItem.hasFocus) ? "#ffffff" : "black"
        font.family: "Helvetica"
        font.bold: true
        horizontalAlignment: Text.AlignHCenter
        font.pointSize: 10
        text: mapItem.itemName

        x : (mapItem.legendPosition == SabrinaCartographyItem.TOP_LEFT ||
             mapItem.legendPosition == SabrinaCartographyItem.MIDDLE_LEFT ||
             mapItem.legendPosition == SabrinaCartographyItem.BOTTOM_LEFT) ? -width - margin :
            (mapItem.legendPosition == SabrinaCartographyItem.TOP_MIDDLE ||
             mapItem.legendPosition == SabrinaCartographyItem.BOTTOM_MIDDLE) ? -width/2 : margin

        y : (mapItem.legendPosition == SabrinaCartographyItem.TOP_LEFT ||
              mapItem.legendPosition == SabrinaCartographyItem.TOP_MIDDLE ||
              mapItem.legendPosition == SabrinaCartographyItem.TOP_RIGHT) ? -height - margin :
            (mapItem.legendPosition == SabrinaCartographyItem.MIDDLE_LEFT ||
             mapItem.legendPosition == SabrinaCartographyItem.MIDDLE_RIGHT) ? -height/2 : margin



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

        Component.onCompleted: {
            console.log(mapItem.legendPosition)
        }

    }

}
