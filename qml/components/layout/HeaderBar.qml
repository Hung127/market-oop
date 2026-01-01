import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    height: topBar.height + mainBar.height
    width: parent ? parent.width : 1440

    Rectangle {
        id: topBar
        width: parent.width
        height: 40
        color: "#000000"

        Item {
            id: topContent
            width: Math.min(parent.width - 80, 1200)
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter

            RowLayout {
                anchors.fill: parent

                Text {
                    text: qsTr("Summer Sale For All Swim Suits And Free Express Delivery - OFF 50%!")
                    color: "#FAFAFA"
                    font.pixelSize: 14
                    Layout.fillWidth: true
                }

                Text {
                    text: qsTr("ShopNow")
                    color: "#FAFAFA"
                    font.pixelSize: 14
                    font.underline: true
                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                }

                // Language dropdown - SỬA LỖI: thêm id cho ComboBox
                ComboBox {
                    id: languageCombo
                    Layout.leftMargin: 24
                    model: [ "English", "Deutsch", "Tiếng Việt" ]
                    currentIndex: 0
                    implicitWidth: 110
                    background: Rectangle {
                        color: "transparent"
                        border.width: 0
                    }
                    contentItem: Text {
                        text: languageCombo.currentText  // SỬA: dùng languageCombo thay vì root
                        color: "#FAFAFA"
                        font.pixelSize: 14
                        verticalAlignment: Text.AlignVCenter
                    }
                }
            }
        }
    }

    Rectangle {
        id: mainBar
        y: topBar.height
        width: parent.width
        height: 80
        color: "#FFFFFF"
        border.color: "#E0E0E0"
        border.width: 0

        Item {
            id: mainContent
            width: Math.min(parent.width - 80, 1200)
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter

            RowLayout {
                id: row
                anchors.fill: parent
                spacing: 32

                // Logo "Exclusive"
                Text {
                    id: logoText
                    text: qsTr("Exclusive")
                    font.pixelSize: 24
                    font.bold: true
                    Layout.alignment: Qt.AlignVCenter
                }

                // Navigation
                RowLayout {
                    id: navRow
                    spacing: 24
                    Layout.alignment: Qt.AlignVCenter
                    Layout.leftMargin: 40

                    // Home (active)
                    Column {
                        spacing: 4
                        Layout.alignment: Qt.AlignVCenter
                        Text {
                            text: qsTr("Home")
                            font.pixelSize: 16
                            color: "#000000"
                        }
                        Rectangle {
                            height: 2
                            width: implicitWidth
                            implicitWidth: 30
                            color: "#DB4444"
                        }
                    }

                    Text {
                        text: qsTr("Contact")
                        font.pixelSize: 16
                        color: "#000000"
                    }

                    Text {
                        text: qsTr("About")
                        font.pixelSize: 16
                        color: "#000000"
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
                    }
                }

                Item { Layout.fillWidth: true }

                // Search + icons
                RowLayout {
                    spacing: 16
                    Layout.alignment: Qt.AlignVCenter

                    Rectangle {
                        radius: 4
                        color: "#F5F5F5"
                        implicitWidth: 280
                        implicitHeight: 40

                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: 8
                            spacing: 8

                            TextField {
                                id: searchField
                                Layout.fillWidth: true
                                background: Rectangle { color: "transparent" }
                                placeholderText: qsTr("What are you looking for?")
                                font.pixelSize: 14
                            }

                            Image {
                                source: "qrc:/icons/search.svg"
                                width: 18
                                height: 18
                                fillMode: Image.PreserveAspectFit
                                Layout.alignment: Qt.AlignVCenter
                            }
                        }
                    }

                    // Wishlist icon
                    ToolButton {
                        icon.source: "qrc:/icons/heart.svg"
                        background: Rectangle { color: "transparent" }
                    }

                    // Cart icon
                    ToolButton {
                        icon.source: "qrc:/icons/cart.svg"
                        background: Rectangle { color: "transparent" }
                    }

                    // User icon
                    ToolButton {
                        icon.source: "qrc:/icons/user.svg"
                        background: Rectangle { color: "transparent" }
                    }
                }
            }
        }
    }
}