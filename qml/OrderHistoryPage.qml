import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Page {
    id: orderHistoryPage

    property string userName: "Buyer"
    
    signal backClicked()
    signal viewOrderDetails(string orderId)

    header: ToolBar {
        background: Rectangle {
            color: "#2196F3"
        }

        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 20
            anchors.rightMargin: 20
            spacing:  20

            Button {
                text: "← Back"
                font.pixelSize: 14
                flat: true
                
                contentItem: Text {
                    text: parent.text
                    font: parent.font
                    color: "white"
                }
                
                onClicked: orderHistoryPage.backClicked()
            }

            Label {
                text: "📜 Order History"
                font. pixelSize: 24
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
            anchors.fill: parent
            anchors.margins: 30
            clip: true
            contentWidth:  availableWidth

            ColumnLayout {
                width: Math.min(1000, parent.width)
                anchors.horizontalCenter: parent. horizontalCenter
                spacing: 15

                // Filter Tabs
                RowLayout {
                    Layout. fillWidth: true
                    spacing: 10

                    Repeater {
                        model:  ["All Orders", "Pending", "Confirmed", "Shipped", "Delivered", "Cancelled"]
                        
                        Button {
                            text: modelData
                            font.pixelSize: 14
                            checkable: true
                            checked:  index === 0
                            
                            contentItem: Text {
                                text: parent.text
                                font: parent.font
                                color: parent.checked ? "white" : "#2196F3"
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment:  Text.AlignVCenter
                            }
                            
                            background: Rectangle {
                                color: parent.checked ? "#2196F3" : (parent.hovered ? "#E3F2FD" : "white")
                                border.color: "#2196F3"
                                border.width: 2
                                radius: 20
                            }
                        }
                    }
                }

                // Orders List
                ListView {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 600
                    clip: true
                    spacing: 15

                    model: ListModel {
                        ListElement {
                            orderId:  "ORD-001"
                            date: "2026-01-02"
                            total: 1505.97
                            itemCount: 3
                            status:  "Delivered"
                            statusColor: "#4CAF50"
                        }
                        ListElement {
                            orderId: "ORD-002"
                            date: "2026-01-01"
                            total: 299.99
                            itemCount: 1
                            status: "Shipped"
                            statusColor: "#FF9800"
                        }
                        ListElement {
                            orderId: "ORD-003"
                            date: "2025-12-30"
                            total: 89.99
                            itemCount: 1
                            status: "Pending"
                            statusColor: "#FFC107"
                        }
                    }

                    delegate: Rectangle {
                        width:  ListView.view.width
                        height: 120
                        color: "white"
                        radius: 10
                        border.color: "#e0e0e0"
                        border.width: 1

                        MouseArea {
                            anchors. fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            
                            onEntered: parent.border.color = "#2196F3"
                            onExited:  parent.border.color = "#e0e0e0"
                            onClicked: orderHistoryPage.viewOrderDetails(model.orderId)
                        }

                        RowLayout {
                            anchors. fill: parent
                            anchors.margins: 20
                            spacing: 20

                            // Order Info
                            ColumnLayout {
                                Layout.fillWidth: true
                                spacing: 8

                                Label {
                                    text: "Order #" + model.orderId
                                    font.pixelSize: 18
                                    font.bold: true
                                    color:  "#333"
                                }

                                Label {
                                    text: "📅 " + model.date
                                    font.pixelSize: 14
                                    color: "#666"
                                }

                                Label {
                                    text:  "📦 " + model.itemCount + " item(s)"
                                    font.pixelSize: 14
                                    color: "#666"
                                }
                            }

                            // Status Badge
                            Rectangle {
                                width: statusLabel.width + 30
                                height: 35
                                color: model.statusColor
                                radius: 18

                                Label {
                                    id: statusLabel
                                    anchors.centerIn: parent
                                    text: model.status
                                    font.pixelSize: 14
                                    font.bold: true
                                    color: "white"
                                }
                            }

                            // Total Price
                            Label {
                                text: "$" + model.total.toFixed(2)
                                font.pixelSize: 22
                                font.bold: true
                                color: "#4CAF50"
                                Layout.preferredWidth: 120
                                horizontalAlignment: Text.AlignRight
                            }

                            // Arrow
                            Label {
                                text: "→"
                                font.pixelSize: 24
                                color: "#2196F3"
                            }
                        }
                    }
                }

                // Empty State
                Label {
                    visible: false  // Show when no orders
                    text:  "No orders yet"
                    font.pixelSize: 18
                    color: "#999"
                    Layout.alignment: Qt.AlignHCenter
                    Layout.topMargin: 50
                }
            }
        }
    }
}
