import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    width: parent ? parent.width : 1440
    implicitHeight: topBar.height + mainBar.height

    /* ================= TOP BAR ================= */
    Rectangle {
        id: topBar
        width: parent.width
        height: 40
        color: "#000000"

        Item {
            width: Math.min(parent.width - 80, 1200)
            height: parent.height
            anchors.horizontalCenter: parent.horizontalCenter

            RowLayout {
                anchors.fill: parent

                Text {
                    text: qsTr("Summer Sale For All Swim Suits And Free Express Delivery - OFF 50%!")
                    color: "#FAFAFA"
                    font.pixelSize: 14
                    Layout.fillWidth: true
                }

                Text {
                    id: shopNow
                    text: qsTr("ShopNow")
                    color: "#FAFAFA"
                    font.pixelSize: 14
                    font.underline: true

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: console.log("Shop Now clicked")
                    }
                }

                ComboBox {
                    id: languageCombo
                    Layout.leftMargin: 24
                    model: ["English", "Deutsch", "Tiếng Việt"]
                    implicitWidth: 110
                    implicitHeight: 28

                    background: Rectangle { color: "transparent" }
                    contentItem: Text {
                        text: languageCombo.currentText
                        color: "#FAFAFA"
                        verticalAlignment: Text.AlignVCenter
                        font.pixelSize: 14
                    }
                }
            }
        }
    }

    /* ================= MAIN BAR ================= */
    Rectangle {
        id: mainBar
        y: topBar.height
        width: parent.width
        height: 80
        color: "#FFFFFF"
        border.color: "#E0E0E0"

        Item {
            width: Math.min(parent.width - 80, 1200)
            height: parent.height
            anchors.horizontalCenter: parent.horizontalCenter

            RowLayout {
                anchors.fill: parent
                spacing: 32

                /* LOGO */
                Text {
                    text: qsTr("Exclusive")
                    font.pixelSize: 24
                    font.bold: true
                }

                /* NAVIGATION */
                RowLayout {
                    spacing: 24
                    Layout.leftMargin: 40

                    Repeater {
                        model: ["Home", "Contact", "About"]
                        delegate: Text {
                            text: modelData
                            font.pixelSize: 16
                            color: "#000000"

                            MouseArea {
                                anchors.fill: parent
                                cursorShape: Qt.PointingHandCursor
                                onClicked: console.log(modelData, "clicked")
                            }
                        }
                    }

                    Rectangle {
                        radius: 4
                        color: "#DB4444"
                        implicitWidth: 100
                        implicitHeight: 36

                        Text {
                            anchors.centerIn: parent
                            text: qsTr("Sign Up")
                            color: "#FFFFFF"
                            font.pixelSize: 16
                        }

                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            onClicked: console.log("Sign Up clicked")
                        }
                    }
                }

                Item { Layout.fillWidth: true }

                /* SEARCH + ICONS */
                RowLayout {
                    spacing: 16

                    Rectangle {
                        radius: 4
                        color: "#F5F5F5"
                        implicitWidth: 280
                        implicitHeight: 40

                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: 8

                            TextField {
                                Layout.fillWidth: true
                                placeholderText: qsTr("What are you looking for?")
                                background: Rectangle { color: "transparent" }
                            }

                            Image {
                                source: "qrc:/icons/search.svg"
                                width: 18
                                height: 18
                            }
                        }
                    }

                    Repeater {
                        model: [
                            "qrc:/icons/heart.svg",
                            "qrc:/icons/cart.svg",
                            "qrc:/icons/user.svg"
                        ]

                        delegate: ToolButton {
                            icon.source: modelData
                            background: Rectangle { color: "transparent" }
                        }
                    }
                }
            }
        }
    }
}
