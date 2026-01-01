import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    width: parent ? parent.width : 1440
    color: "#000000"
    implicitHeight: topContent.implicitHeight + bottomBar.implicitHeight + 32

    Column {
        anchors.fill: parent
        anchors.margins: 0
        spacing: 24

        // Top multi-column content
        Item {
            id: topContent
            width: Math.min(parent.width - 80, 1200)
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: 40
            anchors.bottomMargin: 24
            implicitHeight: colContent.implicitHeight

            RowLayout {
                id: colContent
                anchors.fill: parent
                spacing: 40

                // Column 1: Logo + subscribe
                Column {
                    spacing: 16
                    Layout.fillWidth: true

                    Text {
                        text: qsTr("Exclusive")
                        color: "#FFFFFF"
                        font.pixelSize: 24
                        font.bold: true
                    }

                    Text {
                        text: qsTr("Subscribe")
                        color: "#FFFFFF"
                        font.pixelSize: 18
                        font.bold: true
                    }

                    Text {
                        text: qsTr("Get 10% off your first order")
                        color: "#B3B3B3"
                        font.pixelSize: 14
                    }

                    Rectangle {
                        radius: 4
                        color: "#000000"
                        border.color: "#555555"
                        implicitWidth: 260
                        implicitHeight: 40

                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: 8
                            spacing: 8

                            TextField {
                                id: emailField
                                Layout.fillWidth: true
                                background: Rectangle { color: "transparent" }
                                placeholderText: qsTr("Enter your email")
                                color: "#FFFFFF"
                                font.pixelSize: 14
                            }

                            ToolButton {
                                icon.source: "qrc:/icons/send.svg"
                                background: Rectangle { color: "transparent" }
                            }
                        }
                    }
                }

                // Column 2: Account
                Column {
                    spacing: 12
                    Layout.fillWidth: true

                    Text {
                        text: qsTr("Account")
                        color: "#FFFFFF"
                        font.pixelSize: 18
                        font.bold: true
                    }

                    Repeater {
                        model: [
                            qsTr("My Account"),
                            qsTr("Login / Register"),
                            qsTr("Cart"),
                            qsTr("Wishlist"),
                            qsTr("Shop")
                        ]
                        delegate: Text {
                            text: modelData
                            color: "#B3B3B3"
                            font.pixelSize: 14
                        }
                    }
                }

                // Column 3: Quick Link
                Column {
                    spacing: 12
                    Layout.fillWidth: true

                    Text {
                        text: qsTr("Quick Link")
                        color: "#FFFFFF"
                        font.pixelSize: 18
                        font.bold: true
                    }

                    Repeater {
                        model: [
                            qsTr("Privacy Policy"),
                            qsTr("Terms Of Use"),
                            qsTr("FAQ"),
                            qsTr("Contact")
                        ]
                        delegate: Text {
                            text: modelData
                            color: "#B3B3B3"
                            font.pixelSize: 14
                        }
                    }
                }

                // Column 4: Download App + payments
                Column {
                    spacing: 12
                    Layout.fillWidth: true

                    Text {
                        text: qsTr("Download App")
                        color: "#FFFFFF"
                        font.pixelSize: 18
                        font.bold: true
                    }

                    Text {
                        text: qsTr("Save $3 with App New User Only")
                        color: "#B3B3B3"
                        font.pixelSize: 12
                    }

                    RowLayout {
                        spacing: 12

                        // QR code
                        Rectangle {
                            width: 80
                            height: 80
                            color: "#000000"
                            border.color: "#555555"
                            radius: 4

                            Image {
                                anchors.centerIn: parent
                                source: "qrc:/images/qr_code.png" // Figma: Qr Code
                                width: parent.width * 0.9
                                height: parent.height * 0.9
                                fillMode: Image.PreserveAspectFit
                            }
                        }

                        // App badges
                        Column {
                            spacing: 8

                            Image {
                                source: "qrc:/images/appstore.png" // AppStore
                                width: 120
                                height: 36
                                fillMode: Image.PreserveAspectFit
                            }

                            Image {
                                source: "qrc:/images/googleplay.png" // GooglePlay
                                width: 120
                                height: 36
                                fillMode: Image.PreserveAspectFit
                            }
                        }
                    }

                    // Payment methods row
                    Row {
                        spacing: 8
                        Image {
                            source: "qrc:/images/mastercard.png"
                            width: 40; height: 24
                            fillMode: Image.PreserveAspectFit
                        }
                        Image {
                            source: "qrc:/images/visa.png"
                            width: 40; height: 24
                            fillMode: Image.PreserveAspectFit
                        }
                        Image {
                            source: "qrc:/images/bkash.png"
                            width: 40; height: 24
                            fillMode: Image.PreserveAspectFit
                        }
                        Image {
                            source: "qrc:/images/nagad.png"
                            width: 40; height: 24
                            fillMode: Image.PreserveAspectFit
                        }
                    }
                }
            }
        }

        // Divider + bottom bar
        Column {
            id: bottomBar
            width: parent.width
            spacing: 16

            Rectangle {
                width: Math.min(parent.width - 80, 1200)
                height: 1
                color: "#333333"
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Item {
                width: Math.min(parent.width - 80, 1200)
                height: 32
                anchors.horizontalCenter: parent.horizontalCenter

                RowLayout {
                    anchors.fill: parent

                    RowLayout {
                        spacing: 8
                        Layout.fillWidth: true

                        Image {
                            source: "qrc:/icons/copyright.svg"
                            width: 16
                            height: 16
                            fillMode: Image.PreserveAspectFit
                        }

                        Text {
                            text: qsTr("Copyright © 2023 Exclusive. All rights reserved")
                            color: "#B3B3B3"
                            font.pixelSize: 14
                        }
                    }

                    RowLayout {
                        spacing: 16
                        // Social icons
                        ToolButton {
                            icon.source: "qrc:/icons/facebook.svg"
                            background: Rectangle { color: "transparent" }
                        }
                        ToolButton {
                            icon.source: "qrc:/icons/twitter.svg"
                            background: Rectangle { color: "transparent" }
                        }
                        ToolButton {
                            icon.source: "qrc:/icons/instagram.svg"
                            background: Rectangle { color: "transparent" }
                        }
                        ToolButton {
                            icon.source: "qrc:/icons/linkedin.svg"
                            background: Rectangle { color: "transparent" }
                        }
                    }
                }
            }
        }
    }
}