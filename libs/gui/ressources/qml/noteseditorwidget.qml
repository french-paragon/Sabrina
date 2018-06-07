import QtQuick 2.9
import QtQuick.Controls 2.2

ListView {
    id: notesListView

    clip: true

    model: notesModel

    Component {
        id: notesDelegate

        Rectangle {
            property int rectMargin : 5
            x: 0
            y: 0
            width: parent.width
            height: titleTextEdit.height + noteTextEdit.height + noteCreationDate.height + 6*rectMargin
            color: "#00000000"

            MouseArea {
                id: noteMouseArea
                anchors.fill: parent
                z: 2
                propagateComposedEvents: true

                onClicked: mouse.accepted = false;
                onPressed: {
                    notesListView.currentIndex = index
                    mouse.accepted = false;
                }
                onReleased: mouse.accepted = false;
                onDoubleClicked: mouse.accepted = false;
                onPositionChanged: mouse.accepted = false;
                onPressAndHold: mouse.accepted = false;
            }

            TextEdit {
                id: titleTextEdit
                x: 2*parent.rectMargin
                y: 2*parent.rectMargin
                z: 1
                width: parent.width - parent.rectMargin*2
                height: contentHeight
                text: title
                font.family: "Helvetica"
                font.pointSize: 12
                font.bold: true
                color: "black"
                wrapMode: TextEdit.Wrap
                onEditingFinished: {
                    title = titleTextEdit.text;
                }

                KeyNavigation.down: noteTextEdit
            }

            TextEdit {
                id: noteTextEdit
                x: 2*parent.rectMargin
                y: titleTextEdit.height + parent.rectMargin*3
                z: 1
                width: parent.width - parent.rectMargin*2
                height: contentHeight
                text: display
                font.family: "Helvetica"
                font.pointSize: 8
                color: "black"
                wrapMode: TextEdit.Wrap
                onEditingFinished: {
                    display = noteTextEdit.text;
                }

                KeyNavigation.up: titleTextEdit
            }

            Text {
                id: noteCreationDate
                x: 2*parent.rectMargin
                y: titleTextEdit.height + noteTextEdit.height + parent.rectMargin*4
                z: 1
                width: parent.width - parent.rectMargin*2
                text: date
                font.family: "Helvetica"
                font.pointSize: 8
                color: "#80000000"
                wrapMode: TextEdit.Wrap
            }

            Rectangle {
                id: noteRect
                x: parent.rectMargin
                y: parent.rectMargin
                z: 0
                width: parent.width - 2*rectMargin
                height: titleTextEdit.height + noteTextEdit.height + noteCreationDate.height + 4*parent.rectMargin
                color: decoration

            }
        }
    }

    delegate: notesDelegate

    ScrollBar.vertical: ScrollBar { }

    highlight: Rectangle { color: "lightsteelblue" }
    highlightFollowsCurrentItem: true
}
