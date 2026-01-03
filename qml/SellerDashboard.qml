import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Page {
    id: sellerDashboard

    property string userName: "Seller"
    
    signal logoutClicked()
    signal addProductClicked()
    signal editProductClicked(int productId)
    signal profileClicked()

    header: ToolBar {
        background: Rectangle {
            color: "#4CAF50"
        }

        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 20
            anchors. rightMargin: 20
            spacing: 20

            Label {
                text: "🏪 Seller Dashboard"
                font.pixelSize: 24
                font. bold: true
                color: "white"
            }

            Item { Layout.fillWidth: true }

            Button {
                text:  "👤 " + userName
                font.pixelSize: 14
                flat: true
                
                contentItem: Text {
                    text: parent.text
                    font: parent.font
                    color: "white"
                }
                
                onClicked: sellerDashboard. profileClicked()
            }

            Button {
                text: "Logout"
                font.pixelSize: 14
                
                contentItem: Text {
                    text: parent.text
                    font: parent.font
                    color: "white"
                }
                
                background: Rectangle {
                    color: parent. pressed ? "#D32F2F" : (parent.hovered ?  "#E53935" : "#F44336")
                    radius:  5
                }
                
                onClicked: sellerDashboard.logoutClicked()
            }
        }
    }

    Rectangle {
        anchors.fill: parent
        color: "#f5f5f5"

        ScrollView {
            anchors.fill: parent
            anchors.margins: 30
            clip: true

            ColumnLayout {
                width: parent. parent.width - 60
                spacing: 30

                // Statistics Cards
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 20

                    // Total Revenue Card
                    Rectangle {
                        Layout. fillWidth: true
                        Layout.preferredHeight: 150
                        color: "#4CAF50"
                        radius: 10

                        ColumnLayout {
                            anchors.centerIn: parent
                            spacing:  10

                            Label {
                                text: "💰"
                                font.pixelSize: 40
                                Layout. alignment: Qt.AlignHCenter
                            }

                            Label {
                                text: "$12,459.80"
                                font.pixelSize: 28
                                font.bold: true
                                color: "white"
                                Layout.alignment: Qt.AlignHCenter
                            }

                            Label {
                                text: "Total Revenue"
                                font.pixelSize: 14
                                color: "white"
                                Layout.alignment: Qt.AlignHCenter
                            }
                        }
                    }

                    // Total Products Card
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 150
                        color:  "#2196F3"
                        radius: 10

                        ColumnLayout {
                            anchors.centerIn: parent
                            spacing: 10

                            Label {
                                text: "📦"
                                font. pixelSize: 40
                                Layout.alignment: Qt.AlignHCenter
                            }

                            Label {
                                text: "24"
                                font.pixelSize: 28
                                font.bold: true
                                color: "white"
                                Layout.alignment: Qt. AlignHCenter
                            }

                            Label {
                                text: "Total Products"
                                font.pixelSize: 14
                                color: "white"
                                Layout.alignment: Qt.AlignHCenter
                            }
                        }
                    }

                    // Total Orders Card
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 150
                        color: "#FF9800"
                        radius: 10

                        ColumnLayout {
                            anchors. centerIn: parent
                            spacing: 10

                            Label {
                                text: "🛍️"
                                font. pixelSize: 40
                                Layout.alignment: Qt.AlignHCenter
                            }

                            Label {
                                text:  "156"
                                font. pixelSize: 28
                                font.bold: true
                                color: "white"
                                Layout.alignment: Qt.AlignHCenter
                            }

                            Label {
                                text: "Total Orders"
                                font.pixelSize: 14
                                color: "white"
                                Layout.alignment: Qt.AlignHCenter
                            }
                        }
                    }
                }

                // Product Management Section
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 500
                    color: "white"
                    radius: 10
                    border.color: "#e0e0e0"
                    border.width: 1

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 20
                        spacing:  15

                        // Header
                        RowLayout {
                            Layout.fillWidth: true

                            Label {
                                text: "My Products"
                                font.pixelSize: 22
                                font.bold: true
                                color: "#333"
                            }

                            Item { Layout.fillWidth: true }

                            Button {
                                text: "+ Add Product"
                                font.pixelSize: 14
                                font.bold: true
                                
                                contentItem: Text {
                                    text: parent.text
                                    font: parent.font
                                    color: "white"
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                }
                                
                                background: Rectangle {
                                    color: parent.pressed ?  "#388E3C" : (parent.hovered ? "#43A047" : "#4CAF50")
                                    radius: 5
                                }
                                
                                onClicked: sellerDashboard.addProductClicked()
                            }
                        }

                        Rectangle {
                            Layout.fillWidth: true
                            height: 1
                            color: "#e0e0e0"
                        }

                        // Product List
                        ListView {
                            Layout. fillWidth: true
                            Layout.fillHeight: true
                            clip: true
                            spacing:  10

                            model: ListModel {
                                ListElement {
                                    productId: 1
                                    name: "Laptop Pro 15"
                                    price: 1299.99
                                    stock: 5
                                    sales: 23
                                }
                                ListElement {
                                    productId: 2
                                    name: "Wireless Mouse"
                                    price: 29.99
                                    stock: 50
                                    sales: 87
                                }
                                ListElement {
                                    productId: 3
                                    name: "Mechanical Keyboard"
                                    price: 89.99
                                    stock: 15
                                    sales:  34
                                }
                                ListElement {
                                    productId: 4
                                    name: "USB-C Hub"
                                    price: 49.99
                                    stock: 0
                                    sales: 12
                                }
                            }

                            delegate:  Rectangle {
                                width: ListView.view.width
                                height: 80
                                color:  index % 2 === 0 ? "#fafafa" :  "white"
                                radius:  5

                                RowLayout {
                                    anchors. fill: parent
                                    anchors.margins: 15
                                    spacing: 20

                                    Label {
                                        text: model.name
                                        font.pixelSize: 16
                                        font.bold: true
                                        color:  "#333"
                                        Layout.preferredWidth: 250
                                    }

                                    Label {
                                        text:  "$" + model.price.toFixed(2)
                                        font.pixelSize: 16
                                        color: "#4CAF50"
                                        font.bold: true
                                        Layout.preferredWidth: 100
                                    }

                                    Label {
                                        text: "Stock: " + model.stock
                                        font. pixelSize: 14
                                        color: model.stock > 0 ? "#666" :  "#F44336"
                                        Layout.preferredWidth: 100
                                    }

                                    Label {
                                        text: "Sales: " + model.sales
                                        font. pixelSize: 14
                                        color: "#666"
                                        Layout.preferredWidth: 100
                                    }

                                    Item { Layout. fillWidth: true }

                                    Button {
                                        text: "Edit"
                                        font.pixelSize: 13
                                        
                                        contentItem: Text {
                                            text: parent.text
                                            font: parent.font
                                            color: "#2196F3"
                                        }
                                        
                                        background: Rectangle {
                                            color: parent.hovered ? "#E3F2FD" : "transparent"
                                            border.color: "#2196F3"
                                            border.width: 1
                                            radius: 5
                                        }
                                        
                                        onClicked:  sellerDashboard.editProductClicked(model.productId)
                                    }

                                    Button {
                                        text: "Delete"
                                        font.pixelSize: 13
                                        
                                        contentItem: Text {
                                            text: parent.text
                                            font: parent. font
                                            color: "#F44336"
                                        }
                                        
                                        background: Rectangle {
                                            color: parent.hovered ?  "#FFEBEE" : "transparent"
                                            border.color: "#F44336"
                                            border.width: 1
                                            radius: 5
                                        }
                                        
                                        onClicked:  {
                                            console.log("Delete product:  " + model.productId)
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
