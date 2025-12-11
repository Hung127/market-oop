import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    implicitWidth: timerRow.implicitWidth
    implicitHeight: timerRow.implicitHeight

    // Properties for countdown
    property int days: 3
    property int hours: 23
    property int minutes: 19
    property int seconds: 56

    // Timer to update countdown
    Timer {
        id: countdownTimer
        interval: 1000
        repeat: true
        running: true
        
        onTriggered: {
            root.seconds--
            
            if (root.seconds < 0) {
                root.seconds = 59
                root.minutes--
            }
            
            if (root.minutes < 0) {
                root.minutes = 59
                root.hours--
            }
            
            if (root.hours < 0) {
                root.hours = 23
                root.days--
            }
            
            if (root.days < 0) {
                // Reset or stop timer when countdown ends
                root.days = 0
                root.hours = 0
                root.minutes = 0
                root.seconds = 0
                countdownTimer.stop()
            }
        }
    }

    RowLayout {
        id: timerRow
        spacing: 16

        // Days
        Column {
            spacing: 4
            
            Text {
                text: qsTr("Days")
                font.pixelSize: 12
                color: "#000000"
                font.bold: true
            }
            
            Rectangle {
                width: 62
                height: 62
                radius: 31
                color: "#FFFFFF"
                border.color: "#E0E0E0"
                border.width: 1
                
                Text {
                    anchors.centerIn: parent
                    text: root.days.toString().padStart(2, '0')
                    font.pixelSize: 32
                    font.bold: true
                    color: "#000000"
                }
            }
        }

        // Separator
        Text {
            text: ":"
            font.pixelSize: 32
            font.bold: true
            color: "#DB4444"
            Layout.alignment: Qt.AlignBottom
            Layout.bottomMargin: 12
        }

        // Hours
        Column {
            spacing: 4
            
            Text {
                text: qsTr("Hours")
                font.pixelSize: 12
                color: "#000000"
                font.bold: true
            }
            
            Rectangle {
                width: 62
                height: 62
                radius: 31
                color: "#FFFFFF"
                border.color: "#E0E0E0"
                border.width: 1
                
                Text {
                    anchors.centerIn: parent
                    text: root.hours.toString().padStart(2, '0')
                    font.pixelSize: 32
                    font.bold: true
                    color: "#000000"
                }
            }
        }

        // Separator
        Text {
            text: ":"
            font.pixelSize: 32
            font.bold: true
            color: "#DB4444"
            Layout.alignment: Qt.AlignBottom
            Layout.bottomMargin: 12
        }

        // Minutes
        Column {
            spacing: 4
            
            Text {
                text: qsTr("Minutes")
                font.pixelSize: 12
                color: "#000000"
                font.bold: true
            }
            
            Rectangle {
                width: 62
                height: 62
                radius: 31
                color: "#FFFFFF"
                border.color: "#E0E0E0"
                border.width: 1
                
                Text {
                    anchors.centerIn: parent
                    text: root.minutes.toString().padStart(2, '0')
                    font.pixelSize: 32
                    font.bold: true
                    color: "#000000"
                }
            }
        }

        // Separator
        Text {
            text: ":"
            font.pixelSize: 32
            font.bold: true
            color: "#DB4444"
            Layout.alignment: Qt.AlignBottom
            Layout.bottomMargin: 12
        }

        // Seconds
        Column {
            spacing: 4
            
            Text {
                text: qsTr("Seconds")
                font.pixelSize: 12
                color: "#000000"
                font.bold: true
            }
            
            Rectangle {
                width: 62
                height: 62
                radius: 31
                color: "#FFFFFF"
                border.color: "#E0E0E0"
                border.width: 1
                
                Text {
                    anchors.centerIn: parent
                    text: root.seconds.toString().padStart(2, '0')
                    font.pixelSize: 32
                    font.bold: true
                    color: "#000000"
                }
            }
        }
    }
}