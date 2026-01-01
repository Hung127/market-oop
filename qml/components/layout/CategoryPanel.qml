import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    width: 260

    Column {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 8

        // 2 mục đầu có dropdown, giống Frame 558/559
        Row {
            spacing: 4
            Text {
                text: qsTr("Woman's Fashion")
                font.pixelSize: 16
                color: "#000000"
            }
            Image {
                source: "qrc:/icons/dropdown.svg"
                width: 16; height: 16
            }
        }

        Row {
            spacing: 4
            Text {
                text: qsTr("Men's Fashion")
                font.pixelSize: 16
                color: "#000000"
            }
            Image {
                source: "qrc:/icons/dropdown.svg"
                width: 16; height: 16
            }
        }

        Rectangle { height: 1; width: parent.width; color: "#E0E0E0"; opacity: 0.3 }

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

            delegate: Text {
                text: modelData
                font.pixelSize: 16
                color: "#000000"
                padding: 4
            }
        }
    }
}