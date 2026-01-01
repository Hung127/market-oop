import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    width: parent ? parent.width : 1440
    implicitHeight: 400

    Rectangle {
        id: bannerContainer
        width: Math.min(parent.width - 80, 1200)
        height: 350
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 24
        color: "#000000"
        radius: 8

        RowLayout {
            anchors.fill: parent
            anchors.margins: 40
            spacing: 40

            // Left side - Content
            Column {
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignVCenter
                spacing: 24

                // Brand badge
                Row {
                    spacing: 12

                    Image {
                        source: "qrc:/icons/apple_logo.svg"
                        width: 40
                        height: 48
                        fillMode: Image.PreserveAspectFit
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    Text {
                        text: qsTr("iPhone 14 Series")
                        color: "#FFFFFF"
                        font.pixelSize: 16
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }

                // Main heading
                Column {
                    spacing: 12

                    Text {
                        text: qsTr("Up to 10%")
                        color: "#FFFFFF"
                        font.pixelSize: 48
                        font.bold: true
                        lineHeight: 1.2
                    }

                    Text {
                        text: qsTr("off Voucher")
                        color: "#FFFFFF"
                        font.pixelSize: 48
                        font.bold: true
                        lineHeight: 1.2
                    }
                }

                // Shop Now link
                Row {
                    spacing: 8

                    Text {
                        text: qsTr("Shop Now")
                        color: "#FFFFFF"
                        font.pixelSize: 16
                        font.underline: true
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    Image {
                        source: "qrc:/icons/arrow_right_white.svg"
                        width: 20
                        height: 20
                        fillMode: Image.PreserveAspectFit
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }
            }

            // Right side - Product Image
            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true

                Image {
                    id: heroImage
                    anchors.centerIn: parent
                    source: "qrc:/images/hero_iphone.png"
                    width: parent.width * 0.9
                    height: parent.height * 0.9
                    fillMode: Image.PreserveAspectFit
                }

                // Optional: Add glow effect
                Rectangle {
                    anchors.centerIn: heroImage
                    width: heroImage.width * 0.8
                    height: heroImage.height * 0.8
                    radius: width / 2
                    color: "#FFFFFF"
                    opacity: 0.1
                    z: -1
                }
            }
        }

        // Pagination dots
        Row {
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 16
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 12

            Repeater {
                model: 5

                delegate: Rectangle {
                    width: index === 2 ? 12 : 8  // Middle dot is active
                    height: index === 2 ? 12 : 8
                    radius: width / 2
                    color: index === 2 ? "#DB4444" : "#FFFFFF"
                    opacity: index === 2 ? 1.0 : 0.5

                    Behavior on width {
                        NumberAnimation { duration: 200 }
                    }
                    Behavior on height {
                        NumberAnimation { duration: 200 }
                    }

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            console.log("Banner", index, "clicked")
                        }
                    }
                }
            }
        }

        // Optional: Add navigation arrows
        ToolButton {
            anchors.left: parent.left
            anchors.leftMargin: 16
            anchors.verticalCenter: parent.verticalCenter
            width: 40
            height: 40

            background: Rectangle {
                radius: 20
                color: "#FFFFFF"
                opacity: parent.hovered ? 0.9 : 0.7
            }

            icon.source: "qrc:/icons/arrow_left_black.svg"
            icon.width: 20
            icon.height: 20

            onClicked: {
                console.log("Previous banner")
            }
        }

        ToolButton {
            anchors.right: parent.right
            anchors.rightMargin: 16
            anchors.verticalCenter: parent.verticalCenter
            width: 40
            height: 40

            background: Rectangle {
                radius: 20
                color: "#FFFFFF"
                opacity: parent.hovered ? 0.9 : 0.7
            }

            icon.source: "qrc:/icons/arrow_right_black.svg"
            icon.width: 20
            icon.height: 20

            onClicked: {
                console.log("Next banner")
            }
        }
    }

    // Animation timer for auto-slide (optional)
    Timer {
        id: autoSlideTimer
        interval: 5000
        repeat: true
        running: true
        onTriggered: {
            // Auto-slide logic here
            console.log("Auto-slide to next banner")
        }
    }
}