import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick. Layouts 1.12

Page {
    id: sellerOrderPage

    property string userName: "Seller"
    
    signal backClicked()
    signal confirmOrder(string orderId, string productId)
    signal shipOrder(string orderId, string productId)
    signal deliverOrder(string orderId, string productId)
    signal cancelOrder(string orderId, string productId)

    header: ToolBar {
        background: Rectangle {
            color: "#4CAF50"
        }

        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 20
            anchors.rightMargin: 20
            spacing: 20

            Button {
                text: "← Back"
                font.pixelSize: 14
                flat: true
                
                contentItem: Text {
                    text: parent.text
                    font: parent.font
                    color: "white"
                }
                
                onClicked:  sellerOrderPage.backClicked()
            }

            Label {
                text: "📦 Order Management"
                font.pixelSize: 24
                font.bold: true
                color: "white"
            }

            Item { Layout.fillWidth: true }
        }
    }

    Rectangle {
        anchors.fill: parent
        color: "#f5f5f5"

        ScrollView {
            anchors. fill: parent
            anchors. margins: 30
            clip:  true
            contentWidth: availableWidth

            ColumnLayout {
                width: Math.min(1200, parent.width)
                anchors.horizontalCenter: parent. horizontalCenter
                spacing: 20

                // Statistics Cards
                RowLayout {
                    Layout.fillWidth: true
                    spacing:  15

                    Repeater {
                        model:  ListModel {
                            ListElement { title: "Pending"; count: 5; color: "#FFC107" }
                            ListElement { title:  "Confirmed"; count: 12; color: "#2196F3" }
                            ListElement { title: "Shipped"; count: 8; color:  "#FF9800" }
                            ListElement { title: "Delivered"; count: 45; color: "#4CAF50" }
                        }

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 100
                            color: "white"
                            radius: 10
                            border. color: model.color
                            border.width: 2

                            ColumnLayout {
                                anchors.centerIn: parent
                                spacing:  8

                                Label {
                                    text: model.count
                                    font.pixelSize: 32
                                    font.bold: true
                                    color: model.color
                                    Layout.alignment: Qt.AlignHCenter
                                }

                                Label {
                                    text: model. title
                                    font.pixelSize: 14
                                    color: "#666"
                                    Layout.alignment: Qt.AlignHCenter
                                }
                            }
                        }
                    }
                }

                // Filter Tabs
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 10

                    Repeater {
                        model: ["All", "Pending", "Confirmed", "Shipped", "Delivered"]
                        
                        Button {
                            text: modelData
                            font.pixelSize: 14
                            checkable: true
                            checked: index === 0
                            
                            contentItem: Text {
                                text: parent.text
                                font: parent.font
                                color: parent.checked ? "white" : "#4CAF50"
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment:  Text.AlignVCenter
                            }
                            
                            background: Rectangle {
                                color: parent.checked ? "#4CAF50" : (parent. hovered ? "#E8F5E9" : "white")
                                border.color: "#4CAF50"
                                border.width: 2
                                radius: 20
                            }
                        }
                    }
                }

                // Orders List
                ListView {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 500
                    clip: true
                    spacing: 15

                    model: ListModel {
                        ListElement {
                            orderId: "ORD-001"
                            productId: "PROD-1"
                            productName: "Laptop Pro 15"
                            buyer: "John Doe"
                            date: "2026-01-03"
                            quantity: 1
                            price: 1299.99
                            status: "Pending"
                            statusColor: "#FFC107"
                        }
                        ListElement {
                            orderId:  "ORD-002"
                            productId: "PROD-2"
                            productName: "Wireless Mouse"
                            buyer: "Jane Smith"
                            date: "2026-01-02"
                            quantity: 2
                            price: 59.98
                            status: "Confirmed"
                            statusColor: "#2196F3"
                        }
                        ListElement {
                            orderId: "ORD-003"
                            productId: "PROD-3"
                            productName: "Mechanical Keyboard"
                            buyer:  "Bob Johnson"
                            date: "2026-01-01"
                            quantity: 1
                            price: 89.99
                            status: "Shipped"
                            statusColor: "#FF9800"
                        }
                    }

                    delegate: Rectangle {
                        width: ListView.view.width
                        height: 150
                        color: "white"
                        radius: 10
                        border.color: "#e0e0e0"
                        border.width: 1

                        ColumnLayout {
                            anchors. fill: parent
                            anchors.margins: 20
                            spacing: 15

                            // Header Row
                            RowLayout {
                                Layout.fillWidth: true

                                Label {
                                    text: "Order #" + model.orderId
                                    font.pixelSize: 16
                                    font.bold: true
                                    color: "#333"
                                }

                                Label {
                                    text: "•"
                                    font.pixelSize: 16
                                    color: "#999"
                                }

                                Label {
                                    text:  model.date
                                    font.pixelSize: 14
                                    color: "#666"
                                }

                                Item { Layout.fillWidth: true }

                                Rectangle {
                                    width: statusLabel2.width + 20
                                    height: 30
                                    color: model.statusColor
                                    radius: 15

                                    Label {
                                        id: statusLabel2
                                        anchors.centerIn: parent
                                        text:  model.status
                                        font.pixelSize: 12
                                        font.bold: true
                                        color: "white"
                                    }
                                }
                            }

                            // Product Info Row
                            RowLayout {
                                Layout.fillWidth: true
                                spacing: 15

                                ColumnLayout {
                                    Layout.fillWidth: true
                                    spacing: 5

                                    Label {
                                        text: model.productName
                                        font.pixelSize: 16
                                        font.bold: true
                                        color: "#333"
                                    }

                                    Label {
                                        text: "Buyer: " + model.buyer
                                        font.pixelSize: 13
                                        color: "#666"
                                    }

                                    Label {
                                        text: "Qty: " + model.quantity + " × $" + (model.price / model.quantity).toFixed(2)
                                        font.pixelSize: 13
                                        color: "#666"
                                    }
                                }

                                Label {
                                    text: "$" + model.price.toFixed(2)
                                    font.pixelSize: 20
                                    font.bold: true
                                    color: "#4CAF50"
                                }
                            }

                            // Action Buttons Row
                            RowLayout {
                                Layout.fillWidth: true
                                spacing: 10

                                Button {
                                    visible: model.status === "Pending"
                                    text: "✓ Confirm"
                                    font.pixelSize: 12
                                    
                                    contentItem: Text {
                                        text: parent. text
                                        font: parent.font
                                        color: "white"
                                        horizontalAlignment: Text.AlignHCenter
                                        verticalAlignment: Text.AlignVCenter
                                    }
                                    
                                    background: Rectangle {
                                        color: parent.pressed ? "#1976D2" : (parent.hovered ? "#1E88E5" : "#2196F3")
                                        radius: 5
                                    }
                                    
                                    onClicked: sellerOrderPage.confirmOrder(model.orderId, model.productId)
                                }

                                Button {
                                    visible:  model.status === "Confirmed"
                                    text: "🚚 Ship"
                                    font.pixelSize: 12
                                    
                                    contentItem: Text {
                                        text: parent.text
                                        font: parent.font
                                        color: "white"
                                        horizontalAlignment:  Text.AlignHCenter
                                        verticalAlignment: Text. AlignVCenter
                                    }
                                    
                                    background: Rectangle {
                                        color: parent.pressed ? "#F57C00" : (parent.hovered ? "#FB8C00" : "#FF9800")
                                        radius: 5
                                    }
                                    
                                    onClicked: sellerOrderPage.shipOrder(model.orderId, model.productId)
                                }

                                Button {
                                    visible: model.status === "Shipped"
                                    text: "✓ Mark Delivered"
                                    font. pixelSize: 12
                                    
                                    contentItem: Text {
                                        text: parent.text
                                        font: parent.font
                                        color: "white"
                                        horizontalAlignment: Text. AlignHCenter
                                        verticalAlignment: Text.AlignVCenter
                                    }
                                    
                                    background: Rectangle {
                                        color: parent.pressed ? "#388E3C" : (parent. hovered ? "#43A047" : "#4CAF50")
                                        radius: 5
                                    }
                                    
                                    onClicked: sellerOrderPage.deliverOrder(model. orderId, model.productId)
                                }

                                Item { Layout.fillWidth: true }

                                Button {
                                    visible: model.status !== "Delivered"
                                    text: "✗ Cancel"
                                    font.pixelSize: 12
                                    
                                    contentItem: Text {
                                        text: parent.text
                                        font: parent.font
                                        color: "#F44336"
                                        horizontalAlignment: Text.AlignHCenter
                                        verticalAlignment: Text.AlignVCenter
                                    }
                                    
                                    background: Rectangle {
                                        color: parent.hovered ? "#FFEBEE" : "transparent"
                                        border.color: "#F44336"
                                        border.width: 1
                                        radius: 5
                                    }
                                    
                                    onClicked: sellerOrderPage.cancelOrder(model.orderId, model.productId)
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
