import QtQuick 2.9
import QtQuick.Controls 2.2

ListView {
    id: notesListView

    clip: true

    model: notesModel

    Component {
        id: notesDelegate

        Rectangle {
            x: 0
            y: 0
            width: parent.width
            height: childrenRect.height + 2*noteRect.rectMargin
            color: "#00000000"

            Rectangle {
                property int rectMargin : 5
                id: noteRect
                x: rectMargin
                y: rectMargin
                width: parent.width - 2*rectMargin
                height: childrenRect.height + rectMargin
                color: decoration

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
                    x: parent.rectMargin
                    y: parent.rectMargin
                    z: 1
                    width: parent.width - parent.rectMargin*2
                    text: title
                    font.family: "Helvetica"
                    font.pointSize: 12
                    font.bold: true
                    color: "black"
                    wrapMode: TextEdit.Wrap
                    onEditingFinished: {
                        title = titleTextEdit.text;
                    }
                }

                TextEdit {
                    id: noteTextEdit
                    x: parent.rectMargin
                    y: titleTextEdit.height + parent.rectMargin*2
                    z: 1
                    width: parent.width - parent.rectMargin*2
                    text: display
                    font.family: "Helvetica"
                    font.pointSize: 8
                    color: "black"
                    wrapMode: TextEdit.Wrap
                    onEditingFinished: {
                        display = noteTextEdit.text;
                    }
                }

                Text {
                    id: noteCreationDate
                    x: parent.rectMargin
                    y: titleTextEdit.height + noteTextEdit.height + parent.rectMargin*3
                    width: parent.width - parent.rectMargin*2
                    text: date
                    font.family: "Helvetica"
                    font.pointSize: 8
                    color: "#80000000"
                    wrapMode: TextEdit.Wrap
                }
            }
        }
    }

    delegate: notesDelegate

    ScrollBar.vertical: ScrollBar {
        parent: notesListView.parent
        anchors.top: notesListView.top
        anchors.left: notesListView.right
        anchors.bottom: notesListView.bottom
    }

    highlight: Rectangle { color: "lightsteelblue" }
    highlightFollowsCurrentItem: true
}
