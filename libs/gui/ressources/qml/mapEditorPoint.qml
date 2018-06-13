/*
This file is part of the project Sabrina
Copyright (C) 2018  Paragon <french.paragon@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

import QtQuick 2.9
import SabrinaCartography 0.1

Item {

    id: container

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

         width: mapItem.radius*2
         height: mapItem.radius*2
         color: mapItem.color
         border.color: (mapItem.hasFocus) ? "white" : mapItem.borderColor
         border.width: mapItem.border
         radius: mapItem.radius

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

        property int margin: mapItem.radius + 2

        id: nameTextEdit
        color: (mapItem.hasFocus) ? "#ffffff" : mapItem.legendColor
        font.family: mapItem.legendFont
        font.bold: mapItem.legendBold
        font.italic: mapItem.legendItalic
        font.underline: mapItem.legendUnderlined
        horizontalAlignment: Text.AlignHCenter
        font.pointSize: mapItem.legendSize
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
