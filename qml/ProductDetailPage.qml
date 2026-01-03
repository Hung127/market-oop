import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Page {
    id: productDetailPage

    // Product properties (will be set when navigating to this page)
    property int productId: 1
    property string productName: "Laptop Pro 15"
    property real productPrice: 1299.99
    property string productImage: "💻"
    property string productDescription: "High-performance laptop with 15-inch display, perfect for professionals and developers."
    property string sellerName: "Tech Store"
    property int stockQuantity: 5
    
    signal backClicked()
    signal addToCartClicked(int quantity)
    signal buyNowClicked(int quantity)

    header: ToolBar {
        background: Rectangle {
            color: "#2196F3"
        }

        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 20
            anchors.rightMargin: 20
            spacing: 20

            Button {
                text:  "← Back"
                font. pixelSize: 14
                flat: true
                
                contentItem: Text {
                    text: parent.text
                    font: parent.font
                    color: "white"
                }
                
                onClicked: productDetailPage.backClicked()
            }

            Label {
                text: "Product Details"
                font.pixelSize: 24
                font. bold: true
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

            RowLayout {
                width: parent.parent.width - 60
                spacing: 30

                // Product Image Section
                Rectangle {
                    Layout.preferredWidth: 400
                    Layout.preferredHeight: 400
                    color: "white"
                    radius: 10
                    border.color: "#e0e0e0"
                    border.width: 1

                    Label {
                        anchors.centerIn: parent
                        text:  productImage
                        font.pixelSize: 200
                    }
                }

                // Product Info Section
                ColumnLayout {
                    Layout. fillWidth: true
                    spacing: 20

                    // Product Name
                    Label {
                        text: productName
                        font.pixelSize: 32
                        font.bold: true
                        color: "#333"
                        Layout.fillWidth: true
                        wrapMode: Text.WordWrap
                    }

                    // Seller Info
                    RowLayout {
                        spacing: 10

                        Label {
                            text: "Sold by:"
                            font.pixelSize: 14
                            color: "#666"
                        }

                        Label {
                            text: sellerName
                            font.pixelSize: 14
                            font.bold: true
                            color: "#2196F3"
                        }
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        height: 1
                        color: "#e0e0e0"
                    }

                    // Price
                    Label {
                        text: "$" + productPrice.toFixed(2)
                        font.pixelSize: 36
                        font.bold: true
                        color: "#4CAF50"
                    }

                    // Stock Status
                    Label {
                        text: stockQuantity > 0 ? "In Stock (" + stockQuantity + " available)" : "Out of Stock"
                        font.pixelSize: 16
                        color: stockQuantity > 0 ? "#4CAF50" : "#F44336"
                        font.bold: true
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        height: 1
                        color: "#e0e0e0"
                    }

                    // Description
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing:  10

                        Label {
                            text: "Description"
                            font.pixelSize: 18
                            font. bold: true
                            color:  "#333"
                        }

                        Label {
                            text: productDescription
                            font. pixelSize: 14
                            color: "#666"
                            Layout.fillWidth: true
                            wrapMode: Text.WordWrap
                        }
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        height: 1
                        color: "#e0e0e0"
                    }

                    // Quantity Selector
                    RowLayout {
                        spacing: 15

                        Label {
                            text: "Quantity:"
                            font.pixelSize: 16
                            color: "#333"
                        }

                        Button {
                            text: "-"
                            font.pixelSize: 18
                            font. bold: true
                            implicitWidth: 40
                            implicitHeight: 40
                            enabled: quantitySpinBox.value > 1
                            
                            contentItem:  Text {
                                text: parent.text
                                font: parent.font
                                color: parent.enabled ? "#666" : "#ccc"
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                            
                            background: Rectangle {
                                color:  parent.hovered ? "#f0f0f0" : "white"
                                border.color: "#ddd"
                                border. width: 1
                                radius: 5
                            }
                            
                            onClicked: quantitySpinBox.value--
                        }

                        SpinBox {
                            id:  quantitySpinBox
                            from: 1
                            to: stockQuantity > 0 ? stockQuantity :  1
                            value: 1
                            editable: true
                            implicitWidth: 120
                        }

                        Button {
                            text: "+"
                            font.pixelSize: 18
                            font. bold: true
                            implicitWidth: 40
                            implicitHeight: 40
                            enabled: quantitySpinBox. value < stockQuantity
                            
                            contentItem: Text {
                                text: parent.text
                                font: parent.font
                                color: parent.enabled ? "#666" : "#ccc"
                                horizontalAlignment:  Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                            
                            background: Rectangle {
                                color: parent. hovered ? "#f0f0f0" : "white"
                                border.color: "#ddd"
                                border.width: 1
                                radius:  5
                            }
                            
                            onClicked:  quantitySpinBox.value++
                        }
                    }

                    // Action Buttons
                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 15

                        Button {
                            Layout.fillWidth: true
                            text: "Add to Cart"
                            font.pixelSize: 16
                            font.bold: true
                            enabled: stockQuantity > 0
                            
                            contentItem: Text {
                                text: parent.text
                                font: parent.font
                                color: "white"
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                            
                            background: Rectangle {
                                color: parent.enabled ? (parent.pressed ? "#1976D2" : (parent. hovered ? "#1E88E5" : "#2196F3")) : "#ccc"
                                radius: 5
                            }
                            
                            Layout.preferredHeight: 50
                            
                            onClicked: {
                                productDetailPage.addToCartClicked(quantitySpinBox.value)
                                console.log("Added " + quantitySpinBox.value + " to cart")
                            }
                        }

                        Button {
                            Layout.fillWidth: true
                            text:  "Buy Now"
                            font.pixelSize: 16
                            font.bold: true
                            enabled: stockQuantity > 0
                            
                            contentItem: Text {
                                text: parent.text
                                font: parent.font
                                color: "white"
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                            
                            background: Rectangle {
                                color: parent.enabled ? (parent.pressed ? "#388E3C" : (parent.hovered ?  "#43A047" : "#4CAF50")) : "#ccc"
                                radius: 5
                            }
                            
                            Layout.preferredHeight: 50
                            
                            onClicked: {
                                productDetailPage.buyNowClicked(quantitySpinBox. value)
                                console.log("Buy now:  " + quantitySpinBox. value)
                            }
                        }
                    }
                }
            }
        }
    }
}
