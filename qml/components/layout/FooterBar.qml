import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    width: parent ? parent.width : 1440
    color: "#000000"

    Column {
        width: parent.width
        spacing: 32

        // ===== TOP CONTENT =====
        Item {
            width: Math.min(parent.width - 80, 1200)
            implicitHeight: contentRow.implicitHeight
            anchors.horizontalCenter: parent.horizontalCenter

            RowLayout {
                id: contentRow
                width: parent.width
                spacing: 40

                // Column 1
                Column {
                    spacing: 16
                    Layout.fillWidth: true

                    Text {
                        text: "Exclusive"
                        color: "white"
                        font.pixelSize: 24
                        font.bold: true
                    }

                    Text {
                        text: "Subscribe"
                        color: "white"
                        font.pixelSize: 18
                        font.bold: true
                    }

                    Text {
                        text: "Get 10% off your first order"
                        color: "#B3B3B3"
                        font.pixelSize: 14
                    }

                    TextField {
                        placeholderText: "Enter your email"
                        implicitWidth: 260
                        implicitHeight: 40
                    }
                }

                // Column 2
                Column {
                    spacing: 12
                    Layout.fillWidth: true

                    Text {
                        text: "Account"
                        color: "white"
                        font.pixelSize: 18
                        font.bold: true
                    }

                    Repeater {
                        model: ["My Account", "Login / Register", "Cart", "Wishlist", "Shop"]
                        delegate: Text {
                            text: modelData
                            color: "#B3B3B3"
                            font.pixelSize: 14
                        }
                    }
                }

                // Column 3
                Column {
                    spacing: 12
                    Layout.fillWidth: true

                    Text {
                        text: "Quick Link"
                        color: "white"
                        font.pixelSize: 18
                        font.bold: true
                    }

                    Repeater {
                        model: ["Privacy Policy", "Terms Of Use", "FAQ", "Contact"]
                        delegate: Text {
                            text: modelData
                            color: "#B3B3B3"
                            font.pixelSize: 14
                        }
                    }
                }

                // Column 4
                Column {
                    spacing: 12
                    Layout.fillWidth: true

                    Text {
                        text: "Download App"
                        color: "white"
                        font.pixelSize: 18
                        font.bold: true
                    }

                    Text {
                        text: "Save $3 with App New User Only"
                        color: "#B3B3B3"
                        font.pixelSize: 12
                    }

                    Rectangle {
                        width: 80
                        height: 80
                        radius: 4
                        color: "#111111"
                    }
                }
            }
        }

        // ===== DIVIDER =====
        Rectangle {
            width: Math.min(parent.width - 80, 1200)
            height: 1
            color: "#333333"
            anchors.horizontalCenter: parent.horizontalCenter
        }

        // ===== BOTTOM BAR =====
        Item {
            width: Math.min(parent.width - 80, 1200)
            height: 32
            anchors.horizontalCenter: parent.horizontalCenter

            RowLayout {
                anchors.fill: parent

                Text {
                    text: "Copyright © 2023 Exclusive. All rights reserved"
                    color: "#B3B3B3"
                    font.pixelSize: 14
                    Layout.fillWidth: true
                }

                RowLayout {
                    spacing: 12
                    ToolButton { text: "F" }
                    ToolButton { text: "T" }
                    ToolButton { text: "I" }
                }
            }
        }
    }
}
