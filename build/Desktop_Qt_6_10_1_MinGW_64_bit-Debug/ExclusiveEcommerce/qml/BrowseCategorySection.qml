import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    width: parent ? parent.width : 1440
    implicitHeight: contentItem.implicitHeight + 32

    Item {
        id: contentItem
        width: Math.min(parent.width - 80, 1200)
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 32

        Column {
            anchors.left: parent.left
            anchors.right: parent.right
            spacing: 24

            // Header: label + title + arrows
            RowLayout {
                width: parent.width
                spacing: 24

                Column {
                    spacing: 8

                    Row {
                        spacing: 8
                        Rectangle {
                            width: 20
                            height: 40
                            radius: 4
                            color: "#DB4444"
                        }
                        Text {
                            text: qsTr("Categories")
                            font.pixelSize: 16
                            font.bold: true
                            color: "#DB4444"
                        }
                    }

                    Text {
                        text: qsTr("Browse By Category")
                        font.pixelSize: 36
                        font.bold: true
                        color: "#000000"
                    }
                }

                Item { Layout.fillWidth: true }

                // Điều khiển trái/phải
                Row {
                    spacing: 12
                    ToolButton {
                        width: 32; height: 32
                        background: Rectangle {
                            radius: 16
                            color: "#F5F5F5"
                        }
                        icon.source: "qrc:/icons/arrow_left.svg"
                    }
                    ToolButton {
                        width: 32; height: 32
                        background: Rectangle {
                            radius: 16
                            color: "#F5F5F5"
                        }
                        icon.source: "qrc:/icons/arrow_right.svg"
                    }
                }
            }

            // Grid 6 category - SỬA LỖI: dùng Repeater thay vì tạo dynamic
            GridLayout {
                id: categoryGrid
                width: parent.width
                columns: 6
                columnSpacing: 24
                rowSpacing: 24

                // Component cho từng category card
                Repeater {
                    model: [
                        { label: "Phones", icon: "qrc:/icons/category_phone.svg", highlighted: false },
                        { label: "Computers", icon: "qrc:/icons/category_computer.svg", highlighted: false },
                        { label: "SmartWatch", icon: "qrc:/icons/category_watch.svg", highlighted: false },
                        { label: "Camera", icon: "qrc:/icons/category_camera.svg", highlighted: true },
                        { label: "HeadPhones", icon: "qrc:/icons/category_headphone.svg", highlighted: false },
                        { label: "Gaming", icon: "qrc:/icons/category_gamepad.svg", highlighted: false }
                    ]

                    delegate: Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 140
                        radius: 4
                        color: modelData.highlighted ? "#DB4444" : "#FFFFFF"
                        border.color: modelData.highlighted ? "transparent" : "#E0E0E0"
                        border.width: modelData.highlighted ? 0 : 1

                        Column {
                            anchors.centerIn: parent
                            spacing: 12

                            Image {
                                source: modelData.icon
                                width: 40
                                height: 40
                                fillMode: Image.PreserveAspectFit
                                anchors.horizontalCenter: parent.horizontalCenter
                            }

                            Text {
                                text: modelData.label
                                font.pixelSize: 16
                                color: modelData.highlighted ? "white" : "#000000"
                                horizontalAlignment: Text.AlignHCenter
                                anchors.horizontalCenter: parent.horizontalCenter
                            }
                        }

                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                console.log("Category clicked:", modelData.label)
                            }
                        }
                    }
                }
            }
        }
    }
}