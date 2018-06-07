import QtQuick 2.9
import QtQuick.Controls 2.2

Item {

    Flickable {

        id: flickArea
        contentWidth: mapArea.width*mapArea.scale
        contentHeight: mapArea.height*mapArea.scale

        width: parent.width
        height: parent.height

        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter

        ScrollBar.vertical: ScrollBar { }
        ScrollBar.horizontal: ScrollBar { }

        clip: true

        Item {

            id: mapArea
            objectName: "mapArea"

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
                        mapProxy.insertEditableItem(drop.getDataAsString("text/editableitemref"));
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
