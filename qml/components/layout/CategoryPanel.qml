import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    width: 260

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12

        // ===== WOMEN =====
        Rectangle {
            Layout.fillWidth: true
            height: 36
            radius: 4
            color: womanMouse.containsMouse ? "#F5F5F5" : "transparent"

            RowLayout {
                anchors.fill: parent
                anchors.margins: 4

                Text {
                    text: qsTr("Woman's Fashion")
                    font.pixelSize: 16
                    Layout.fillWidth: true
                }

                Image {
                    source: "qrc:/icons/dropdown.svg"
                    width: 16
                    height: 16
                    Layout.alignment: Qt.AlignVCenter
                }
            }

            MouseArea {
                id: womanMouse
                anchors.fill: parent   // OK vì Rectangle không phải Layout item
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor
                onClicked: console.log("Woman clicked")
            }
        }

        // ===== MEN =====
        Rectangle {
            Layout.fillWidth: true
            height: 36
            radius: 4
            color: menMouse.containsMouse ? "#F5F5F5" : "transparent"

            RowLayout {
                anchors.fill: parent
                anchors.margins: 4

                Text {
                    text: qsTr("Men's Fashion")
                    font.pixelSize: 16
                    Layout.fillWidth: true
                }

                Image {
                    source: "qrc:/icons/dropdown.svg"
                    width: 16
                    height: 16
                    Layout.alignment: Qt.AlignVCenter
                }
            }

            MouseArea {
                id: menMouse
                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor
                onClicked: console.log("Men clicked")
            }
        }

        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: "#E0E0E0"
            opacity: 0.3
        }

        // ===== OTHER CATEGORIES =====
        Repeater {
            model: [
                qsTr("Electronics"),
                qsTr("Home & Lifestyle"),
                qsTr("Medicine"),
                qsTr("Sports & Outdoor"),
                qsTr("Baby’s & Toys"),
                qsTr("Groceries & Pets"),
                qsTr("Health & Beauty")
            ]

            delegate: Rectangle {
                Layout.fillWidth: true
                height: 36
                radius: 4
                color: mouse.containsMouse ? "#F5F5F5" : "transparent"

                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    anchors.leftMargin: 4
                    text: modelData
                    font.pixelSize: 16
                }

                MouseArea {
                    id: mouse
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: console.log("Category clicked:", modelData)
                }
            }
        }
    }
}
