import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Page {
    id: cartPage

    property string userName: "Buyer"
    
    signal backClicked()
    signal checkoutClicked()
    signal continueShoppingClicked()

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
                text: "← Back"
                font.pixelSize: 14
                flat: true
                
                contentItem: Text {
                    text:  parent.text
                    font: parent.font
                    color: "white"
                }
                
                onClicked: cartPage.backClicked()
            }

            Label {
                text: "🛒 Shopping Cart"
                font.pixelSize: 24
                font. bold: true
                color: "white"
            }

            Item { Layout.fillWidth: true }

            Label {
                text: "👤 " + userName
                font.pixelSize: 14
                color: "white"
            }
        }
    }

    Rectangle {
        anchors. fill: parent
        color: "#f5f5f5"

        RowLayout {
            anchors.fill: parent
            anchors.margins: 30
            spacing: 30

            // Cart Items List
            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: "white"
                radius: 10
                border.color: "#e0e0e0"
                border.width: 1

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 20
                    spacing: 15

                    Label {
                        text: "Cart Items"
                        font.pixelSize: 22
                        font.bold: true
                        color: "#333"
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        height: 1
                        color: "#e0e0e0"
                    }

                    ListView {
                        id: cartListView
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        clip:  true
                        spacing: 15

                        model: ListModel {
                            id: cartModel
                            ListElement {
                                productId: 1
                                name: "Laptop Pro 15"
                                price: 1299.99
                                quantity: 1
                                image: "💻"
                            }
                            ListElement {
                                productId: 2
                                name: "Wireless Mouse"
                                price: 29.99
                                quantity: 2
                                image: "🖱️"
                            }
                            ListElement {
                                productId: 3
                                name: "Mechanical Keyboard"
                                price: 89.99
                                quantity: 1
                                image: "⌨️"
                            }
                        }

                        delegate: Rectangle {
                            width: ListView.view.width
                            height: 120
                            color: "#fafafa"
                            radius:  8
                            border.color: "#e0e0e0"
                            border.width: 1

                            RowLayout {
                                anchors.fill: parent
                                anchors.margins: 15
                                spacing: 20

                                // Product Image
                                Label {
                                    text: model.image
                                    font.pixelSize: 50
                                    Layout.preferredWidth: 80
                                    Layout.alignment: Qt.AlignVCenter
                                }

                                // Product Info
                                ColumnLayout {
                                    Layout.fillWidth: true
                                    spacing: 5

                                    Label {
                                        text: model.name
                                        font.pixelSize: 18
                                        font.bold: true
                                        color: "#333"
                                    }

                                    Label {
                                        text: "$" + model. price.toFixed(2) + " each"
                                        font.pixelSize: 14
                                        color: "#666"
                                    }

                                    Label {
                                        text: "Subtotal: $" + (model. price * model.quantity).toFixed(2)
                                        font.pixelSize: 16
                                        font.bold: true
                                        color: "#4CAF50"
                                    }
                                }

                                // Quantity Controls
                                RowLayout {
                                    spacing: 10

                                    Label {
                                        text: "Qty:"
                                        font.pixelSize: 14
                                        color: "#666"
                                    }

                                    Button {
                                        text: "-"
                                        font.pixelSize: 18
                                        font.bold: true
                                        implicitWidth: 40
                                        implicitHeight: 40
                                        
                                        contentItem: Text {
                                            text: parent.text
                                            font: parent.font
                                            color: "#666"
                                            horizontalAlignment: Text.AlignHCenter
                                            verticalAlignment: Text.AlignVCenter
                                        }
                                        
                                        background: Rectangle {
                                            color: parent.hovered ? "#f0f0f0" : "white"
                                            border.color: "#ddd"
                                            border. width: 1
                                            radius: 5
                                        }
                                        
                                        onClicked: {
                                            if (model.quantity > 1) {
                                                cartModel.setProperty(index, "quantity", model. quantity - 1)
                                                updateTotal()
                                            }
                                        }
                                    }

                                    Label {
                                        text: model.quantity
                                        font.pixelSize: 16
                                        font.bold: true
                                        Layout.preferredWidth: 30
                                        horizontalAlignment: Text.AlignHCenter
                                    }

                                    Button {
                                        text: "+"
                                        font.pixelSize: 18
                                        font.bold: true
                                        implicitWidth: 40
                                        implicitHeight: 40
                                        
                                        contentItem: Text {
                                            text: parent.text
                                            font: parent.font
                                            color: "#666"
                                            horizontalAlignment:  Text.AlignHCenter
                                            verticalAlignment: Text. AlignVCenter
                                        }
                                        
                                        background: Rectangle {
                                            color: parent.hovered ?  "#f0f0f0" : "white"
                                            border.color: "#ddd"
                                            border.width: 1
                                            radius:  5
                                        }
                                        
                                        onClicked: {
                                            cartModel.setProperty(index, "quantity", model.quantity + 1)
                                            updateTotal()
                                        }
                                    }
                                }

                                // Remove Button
                                Button {
                                    text: "🗑️"
                                    font.pixelSize: 18
                                    implicitWidth: 45
                                    implicitHeight:  45
                                    
                                    background: Rectangle {
                                        color: parent.pressed ? "#FFCDD2" : (parent.hovered ? "#FFEBEE" : "transparent")
                                        border.color: "#F44336"
                                        border.width: 1
                                        radius: 5
                                    }
                                    
                                    onClicked: {
                                        cartModel.remove(index)
                                        updateTotal()
                                    }
                                }
                            }
                        }
                    }

                    // Empty Cart Message
                    Label {
                        visible: cartModel.count === 0
                        text: "Your cart is empty"
                        font. pixelSize: 18
                        color: "#999"
                        Layout.alignment: Qt.AlignHCenter
                        Layout.topMargin: 50
                    }

                    Button {
                        visible: cartModel.count === 0
                        text: "Continue Shopping"
                        font.pixelSize: 16
                        Layout.alignment: Qt.AlignHCenter
                        
                        contentItem: Text {
                            text: parent.text
                            font: parent.font
                            color: "white"
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment:  Text.AlignVCenter
                        }
                        
                        background: Rectangle {
                            color: parent.pressed ? "#1976D2" : (parent.hovered ? "#1E88E5" : "#2196F3")
                            radius: 5
                        }
                        
                        onClicked: cartPage.continueShoppingClicked()
                    }
                }
            }

            // Order Summary
            Rectangle {
                Layout.preferredWidth: 350
                Layout.fillHeight: true
                color: "white"
                radius: 10
                border.color: "#e0e0e0"
                border.width: 1

                ColumnLayout {
                    anchors.fill: parent
                    anchors. margins: 20
                    spacing: 20

                    Label {
                        text: "Order Summary"
                        font.pixelSize: 22
                        font.bold: true
                        color: "#333"
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        height: 1
                        color: "#e0e0e0"
                    }

                    ColumnLayout {
                        Layout. fillWidth: true
                        spacing: 15

                        RowLayout {
                            Layout.fillWidth: true

                            Label {
                                text: "Subtotal:"
                                font.pixelSize: 16
                                color:  "#666"
                            }

                            Item { Layout.fillWidth: true }

                            Label {
                                id: subtotalLabel
                                text: "$0.00"
                                font. pixelSize: 16
                                color: "#333"
                            }
                        }

                        RowLayout {
                            Layout.fillWidth: true

                            Label {
                                text: "Tax (10%):"
                                font. pixelSize: 16
                                color: "#666"
                            }

                            Item { Layout.fillWidth: true }

                            Label {
                                id:  taxLabel
                                text: "$0.00"
                                font.pixelSize: 16
                                color: "#333"
                            }
                        }

                        RowLayout {
                            Layout.fillWidth: true

                            Label {
                                text: "Shipping:"
                                font.pixelSize: 16
                                color: "#666"
                            }

                            Item { Layout.fillWidth: true }

                            Label {
                                id: shippingLabel
                                text: "$10.00"
                                font. pixelSize: 16
                                color: "#333"
                            }
                        }

                        Rectangle {
                            Layout.fillWidth: true
                            height: 1
                            color: "#e0e0e0"
                        }

                        RowLayout {
                            Layout.fillWidth: true

                            Label {
                                text: "Total:"
                                font.pixelSize: 20
                                font.bold: true
                                color: "#333"
                            }

                            Item { Layout.fillWidth: true }

                            Label {
                                id: totalLabel
                                text:  "$0.00"
                                font.pixelSize: 24
                                font.bold: true
                                color: "#4CAF50"
                            }
                        }
                    }

                    Item { Layout.fillHeight: true }

                    Button {
                        Layout.fillWidth: true
                        text: "Proceed to Checkout"
                        font.pixelSize: 16
                        font.bold: true
                        enabled: cartModel.count > 0
                        
                        contentItem: Text {
                            text:  parent.text
                            font: parent.font
                            color: "white"
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        
                        background: Rectangle {
                            color:  parent.enabled ? (parent.pressed ? "#388E3C" : (parent.hovered ? "#43A047" : "#4CAF50")) : "#ccc"
                            radius: 5
                        }
                        
                        Layout.preferredHeight: 50
                        
                        onClicked: cartPage.checkoutClicked()
                    }

                    Button {
                        Layout.fillWidth: true
                        text: "Continue Shopping"
                        font.pixelSize: 14
                        
                        contentItem: Text {
                            text: parent.text
                            font: parent.font
                            color: "#2196F3"
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment:  Text.AlignVCenter
                        }
                        
                        background: Rectangle {
                            color: parent.hovered ? "#E3F2FD" : "transparent"
                            border.color: "#2196F3"
                            border.width: 1
                            radius: 5
                        }
                        
                        Layout.preferredHeight: 45
                        
                        onClicked: cartPage.continueShoppingClicked()
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        updateTotal()
    }

    function updateTotal() {
        var subtotal = 0
        for (var i = 0; i < cartModel.count; i++) {
            var item = cartModel.get(i)
            subtotal += item. price * item.quantity
        }
        
        var tax = subtotal * 0.1
        var shipping = cartModel.count > 0 ? 10.00 : 0.00
        var total = subtotal + tax + shipping
        
        subtotalLabel.text = "$" + subtotal.toFixed(2)
        taxLabel.text = "$" + tax.toFixed(2)
        shippingLabel.text = "$" + shipping.toFixed(2)
        totalLabel. text = "$" + total.toFixed(2)
    }
}
