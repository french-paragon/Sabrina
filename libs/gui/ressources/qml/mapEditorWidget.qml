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
import QtQuick.Controls 2.2

Item {

    width: flickArea.width
    height: flickArea.height

    Flickable {

        id: flickArea
        contentWidth: mapArea.width*mapArea.scale
        contentHeight: mapArea.height*mapArea.scale

        width: mapArea.width*mapArea.scale
        height: mapArea.height*mapArea.scale

        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter

        ScrollBar.vertical: ScrollBar { }
        ScrollBar.horizontal: ScrollBar { }

        clip: true
        interactive: false

        Item {

            id: mapArea
            objectName: "mapArea"

            transformOrigin: Item.TopLeft
            x: 0
            y: 0

            width: parseInt(mapProxy.size.width)
            height: parseInt(mapProxy.size.height)

            scale: mapProxy.scale

            Image {

                anchors.fill: parent

                z: 0

                id: mapImage
                source: "image://provider/" + mapProxy.imageBackground // "image://provider/name"

                fillMode: Image.Stretch
            }

            DropArea {
                anchors.fill: parent

                z:1

                onDropped: {
                    if (drop.formats.indexOf("text/editableitemref") >= 0) {
                        mapProxy.insertEditableItem(drop.getDataAsArrayBuffer("text/editableitemref"));
                    }
                }
            }

            MouseArea {
                anchors.fill: parent

                onClicked: {
                    parent.forceActiveFocus()
                    mapProxy.clearItemSelection();
                }
            }

        }

    }

}
