import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Page {
    id: cartPage

    property string userName: authController.currentUserName
    
    signal backClicked()
    signal checkoutClicked()
    signal continueShoppingClicked()

    // Listen for cart changes
    Connections {
        target: authController.cartController
        
        function onCartChanged() {
            console.log("[CartPage] Cart changed, updating...")
            updateTotal()
        }
        
        function onCheckoutSuccess(message) {
            console.log("[CartPage] Checkout success:", message)
            successDialog.open()
        }
        
        function onCheckoutFailed(error) {
            console.log("[CartPage] Checkout failed:", error)
            errorDialog.errorMessage = error
            errorDialog.open()
        }
    }

    // Success Dialog
    Dialog {
        id: successDialog
        title: "Order Placed!"
        standardButtons: Dialog.Ok
        
        Label {
            text: "Your order has been placed successfully!\nThank you for your purchase."
        }
        
        onAccepted: {
            cartPage.backClicked()
        }
    }

    // Error Dialog
    Dialog {
        id: errorDialog
        property string errorMessage: ""
        title: "Checkout Failed"
        standardButtons:  Dialog.Ok
        
        Label {
            text: errorDialog.errorMessage
            wrapMode: Text.WordWrap
        }
    }

    Component.onCompleted: {
        console.log("[CartPage] Loading cart items...")
        cartListView.model = authController.cartController.getCartItems()
        updateTotal()
    }

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
                    text: parent.text
                    font: parent.font
                    color: "white"
                }
                
                onClicked: cartPage.backClicked()
            }

            Label {
                text: "🛒 Shopping Cart"
                font.pixelSize: 24
                font.bold: true
                color: "white"
            }

            Item { Layout.fillWidth: true }

            Label {
                text: "👤 " + userName
                font.pixelSize: 14
                color:  "white"
            }
        }
    }

    Rectangle {
        anchors.fill: parent
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
                        clip: true
                        spacing: 15

                        delegate: Rectangle {
                            width:  ListView.view.width
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
                                    text: modelData.image
                                    font.pixelSize: 50
                                    Layout.preferredWidth: 80
                                    Layout.alignment: Qt.AlignVCenter
                                }

                                // Product Info
                                ColumnLayout {
                                    Layout.fillWidth: true
                                    spacing: 5

                                    Label {
                                        text: modelData.name
                                        font.pixelSize: 18
                                        font.bold: true
                                        color: "#333"
                                    }

                                    Label {
                                        text: "$" + modelData.price.toFixed(2) + " each"
                                        font.pixelSize: 14
                                        color: "#666"
                                    }

                                    Label {
                                        text: "Subtotal: $" + modelData.subtotal.toFixed(2)
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
                                        implicitHeight:  40
                                        enabled: modelData.quantity > 1
                                        
                                        contentItem: Text {
                                            text: parent.text
                                            font: parent.font
                                            color: parent.enabled ? "#666" : "#ccc"
                                            horizontalAlignment: Text.AlignHCenter
                                            verticalAlignment: Text.AlignVCenter
                                        }
                                        
                                        background: Rectangle {
                                            color: parent.hovered ? "#f0f0f0" : "white"
                                            border.color: "#ddd"
                                            border.width: 1
                                            radius: 5
                                        }
                                        
                                        onClicked: {
                                            var newQty = modelData.quantity - 1
                                            authController.cartController.updateQuantity(modelData.productId, newQty)
                                            refreshCart()
                                        }
                                    }

                                    Label {
                                        text:  modelData.quantity
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
                                        implicitHeight:  40
                                        enabled:  modelData.quantity < modelData.stock
                                        
                                        contentItem: Text {
                                            text: parent.text
                                            font: parent.font
                                            color: parent.enabled ? "#666" :  "#ccc"
                                            horizontalAlignment: Text.AlignHCenter
                                            verticalAlignment: Text.AlignVCenter
                                        }
                                        
                                        background: Rectangle {
                                            color: parent.hovered ? "#f0f0f0" : "white"
                                            border.color: "#ddd"
                                            border.width: 1
                                            radius: 5
                                        }
                                        
                                        onClicked: {
                                            var newQty = modelData.quantity + 1
                                            authController.cartController.updateQuantity(modelData.productId, newQty)
                                            refreshCart()
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
                                        authController.cartController.removeFromCart(modelData.productId)
                                        refreshCart()
                                    }
                                }
                            }
                        }
                    }

                    // Empty Cart Message
                    Label {
                        visible: cartListView.count === 0
                        text: "Your cart is empty"
                        font.pixelSize: 18
                        color: "#999"
                        Layout.alignment: Qt.AlignHCenter
                        Layout.topMargin: 50
                    }

                    Button {
                        visible: cartListView.count === 0
                        text: "Continue Shopping"
                        font.pixelSize: 16
                        Layout.alignment: Qt.AlignHCenter
                        
                        contentItem: Text {
                            text: parent.text
                            font: parent.font
                            color: "white"
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
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
                    anchors.margins: 20
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
                        Layout.fillWidth: true
                        spacing:  15

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
                                font.pixelSize: 16
                                color: "#333"
                            }
                        }

                        RowLayout {
                            Layout.fillWidth: true

                            Label {
                                text: "Tax (10%):"
                                font.pixelSize: 16
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
                                font.pixelSize: 16
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
                                text: "$0.00"
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
                        enabled: cartListView.count > 0
                        
                        contentItem: Text {
                            text: parent.text
                            font: parent.font
                            color: "white"
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment:  Text.AlignVCenter
                        }
                        
                        background: Rectangle {
                            color: parent.enabled ? (parent.pressed ? "#388E3C" : (parent.hovered ? "#43A047" : "#4CAF50")) : "#ccc"
                            radius: 5
                        }
                        
                        Layout.preferredHeight: 50
                        
                        onClicked: {
                            console.log("[CartPage] Checkout clicked")
                            authController.cartController.checkout()
                        }
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

    function refreshCart() {
        cartListView.model = authController.cartController.getCartItems()
        updateTotal()
    }

    function updateTotal() {
        var subtotal = authController.cartController.totalPrice
        var tax = subtotal * 0.1
        var shipping = authController.cartController.itemCount > 0 ? 10.00 : 0.00
        var total = subtotal + tax + shipping
        
        subtotalLabel.text = "$" + subtotal.toFixed(2)
        taxLabel.text = "$" + tax.toFixed(2)
        shippingLabel.text = "$" + shipping.toFixed(2)
        totalLabel.text = "$" + total.toFixed(2)
    }
}
