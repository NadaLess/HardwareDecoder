import QtQuick 2.7
import QtQuick.Window 2.2
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2

import VideoHW 0.1

Window {
    visible: true
    width: 640
    height: 480

    ColumnLayout {
        anchors.fill: parent

        VideoRenderer {
            id: renderer
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 10
            Layout.fillHeight: true

            source: HWDecoder.source
        }

        RowLayout {
            anchors.left: parent.left
            anchors.right: parent.right

            TextField {
                id:inputStr
                Layout.fillWidth: true
            }
            Button {
                id:inputBtn
                text: 'Open Video'
                onClicked: openDialog.open()
            }
        }

        Button {
            id: decodeBtn
            Layout.alignment: Qt.AlignHCenter
            text: 'Decode'

            onClicked: HWDecoder.decodeVideo(inputStr.text)
        }
    }

    FileDialog {
        id: openDialog
        selectFolder: false
        selectMultiple: false
        onAccepted: inputStr.text = openDialog.fileUrl
    }
}
