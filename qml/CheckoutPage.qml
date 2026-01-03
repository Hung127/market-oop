import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Page {
    id: checkoutPage

    property string userName: "Buyer"
    property var cartItems: []  // Array of cart items
    property real subtotal: 0
    property real tax: 0
    property real shipping: 0
    property real total: 0
    
    signal backClicked()
    signal placeOrderClicked()

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
                
                onClicked:  checkoutPage.backClicked()
            }

            Label {
                text: "💳 Checkout"
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
            anchors.fill: parent
            anchors.margins: 30
            clip: true
            contentWidth: availableWidth

            RowLayout {
                width: parent.width
                spacing: 30
                anchors.horizontalCenter: parent. horizontalCenter

                // Left Column - Order Summary
                ColumnLayout {
                    Layout.fillWidth: true
                    Layout.preferredWidth: 500
                    spacing: 20

                    // Shipping Address Card
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: shippingContent.height + 60
                        color: "white"
                        radius: 10
                        border. color: "#e0e0e0"
                        border.width: 1

                        ColumnLayout {
                            id: shippingContent
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.top: parent.top
                            anchors. margins: 30
                            spacing: 15

                            Label {
                                text: "📍 Shipping Address"
                                font.pixelSize: 20
                                font.bold: true
                                color: "#333"
                            }

                            Rectangle {
                                Layout.fillWidth: true
                                height: 1
                                color:  "#e0e0e0"
                            }

                            TextField {
                                id: addressField
                                Layout.fillWidth: true
                                placeholderText: "Street address"
                                font.pixelSize: 14
                                
                                background: Rectangle {
                                    color: addressField.focus ? "#f0f8ff" : "#f5f5f5"
                                    border.color: addressField. focus ? "#2196F3" : "#ddd"
                                    border.width: 2
                                    radius: 5
                                }
                                padding: 12
                            }

                            RowLayout {
                                Layout. fillWidth: true
                                spacing: 10

                                TextField {
                                    id: cityField
                                    Layout.fillWidth: true
                                    placeholderText: "City"
                                    font.pixelSize: 14
                                    
                                    background: Rectangle {
                                        color: cityField. focus ? "#f0f8ff" : "#f5f5f5"
                                        border. color: cityField.focus ? "#2196F3" : "#ddd"
                                        border. width: 2
                                        radius: 5
                                    }
                                    padding: 12
                                }

                                TextField {
                                    id:  zipField
                                    Layout.preferredWidth: 150
                                    placeholderText: "ZIP Code"
                                    font. pixelSize: 14
                                    
                                    background:  Rectangle {
                                        color:  zipField.focus ? "#f0f8ff" : "#f5f5f5"
                                        border.color: zipField. focus ? "#2196F3" : "#ddd"
                                        border.width: 2
                                        radius: 5
                                    }
                                    padding: 12
                                }
                            }

                            TextField {
                                id: phoneField
                                Layout.fillWidth: true
                                placeholderText: "Phone number"
                                font.pixelSize: 14
                                
                                background: Rectangle {
                                    color: phoneField.focus ? "#f0f8ff" : "#f5f5f5"
                                    border.color: phoneField.focus ? "#2196F3" : "#ddd"
                                    border.width: 2
                                    radius: 5
                                }
                                padding: 12
                            }
                        }
                    }

                    // Payment Method Card
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: paymentContent.height + 60
                        color: "white"
                        radius: 10
                        border.color: "#e0e0e0"
                        border.width: 1

                        ColumnLayout {
                            id: paymentContent
                            anchors.left: parent.left
                            anchors. right: parent.right
                            anchors.top: parent.top
                            anchors.margins: 30
                            spacing: 15

                            Label {
                                text: "💳 Payment Method"
                                font.pixelSize: 20
                                font.bold: true
                                color: "#333"
                            }

                            Rectangle {
                                Layout.fillWidth: true
                                height: 1
                                color: "#e0e0e0"
                            }

                            RadioButton {
                                id: creditCardRadio
                                text: "Credit/Debit Card"
                                checked: true
                                font.pixelSize: 14
                            }

                            RadioButton {
                                id: paypalRadio
                                text:  "PayPal"
                                font.pixelSize: 14
                            }

                            RadioButton {
                                id: codRadio
                                text: "Cash on Delivery"
                                font.pixelSize: 14
                            }
                        }
                    }

                    // Order Items Card
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: Math.min(400, itemsList.contentHeight + 120)
                        color: "white"
                        radius: 10
                        border.color: "#e0e0e0"
                        border.width: 1

                        ColumnLayout {
                            anchors.fill: parent
                            anchors. margins: 30
                            spacing: 15

                            Label {
                                text: "📦 Order Items"
                                font.pixelSize: 20
                                font.bold: true
                                color: "#333"
                            }

                            Rectangle {
                                Layout.fillWidth: true
                                height: 1
                                color: "#e0e0e0"
                            }

                            ListView {
                                id: itemsList
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                clip: true
                                spacing: 10

                                model: ListModel {
                                    ListElement {
                                        name:  "Laptop Pro 15"
                                        price:  1299.99
                                        quantity: 1
                                        image: "💻"
                                    }
                                    ListElement {
                                        name: "Wireless Mouse"
                                        price: 29.99
                                        quantity: 2
                                        image: "🖱️"
                                    }
                                }

                                delegate: Rectangle {
                                    width: ListView.view.width
                                    height: 60
                                    color: "#fafafa"
                                    radius: 5

                                    RowLayout {
                                        anchors.fill: parent
                                        anchors. margins: 10
                                        spacing: 15

                                        Label {
                                            text: model.image
                                            font.pixelSize: 30
                                        }

                                        ColumnLayout {
                                            Layout.fillWidth: true
                                            spacing: 3

                                            Label {
                                                text: model.name
                                                font.pixelSize: 14
                                                font.bold: true
                                                color: "#333"
                                            }

                                            Label {
                                                text: "Qty: " + model.quantity
                                                font.pixelSize: 12
                                                color: "#666"
                                            }
                                        }

                                        Label {
                                            text: "$" + (model.price * model.quantity).toFixed(2)
                                            font.pixelSize: 16
                                            font.bold: true
                                            color: "#4CAF50"
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                // Right Column - Order Total
                Rectangle {
                    Layout.preferredWidth: 350
                    Layout.alignment: Qt.AlignTop
                    Layout.preferredHeight: orderSummaryContent.height + 60
                    color: "white"
                    radius: 10
                    border.color: "#e0e0e0"
                    border.width: 1

                    ColumnLayout {
                        id: orderSummaryContent
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.top: parent.top
                        anchors. margins: 30
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
                                    color: "#666"
                                }

                                Item { Layout.fillWidth: true }

                                Label {
                                    text: "$1359.97"
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
                                    text: "$136.00"
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

                                Item { Layout. fillWidth: true }

                                Label {
                                    text:  "$1505.97"
                                    font.pixelSize: 24
                                    font.bold: true
                                    color: "#4CAF50"
                                }
                            }
                        }

                        Rectangle {
                            Layout.fillWidth: true
                            height: 1
                            color: "#e0e0e0"
                        }

                        Label {
                            id: errorLabel
                            text: ""
                            color: "#F44336"
                            font. pixelSize: 14
                            Layout.fillWidth: true
                            wrapMode: Text.WordWrap
                            visible: text !== ""
                        }

                        Button {
                            Layout.fillWidth: true
                            text: "Place Order"
                            font.pixelSize: 18
                            font.bold: true
                            
                            contentItem: Text {
                                text: parent.text
                                font: parent.font
                                color: "white"
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                            
                            background: Rectangle {
                                color:  parent.pressed ? "#388E3C" : (parent.hovered ? "#43A047" : "#4CAF50")
                                radius: 5
                            }
                            
                            Layout.preferredHeight: 55
                            
                            onClicked: {
                                errorLabel.text = ""
                                
                                // Validation
                                if (addressField. text === "") {
                                    errorLabel. text = "Please enter shipping address"
                                    return
                                }
                                
                                if (cityField.text === "") {
                                    errorLabel.text = "Please enter city"
                                    return
                                }
                                
                                if (zipField.text === "") {
                                    errorLabel. text = "Please enter ZIP code"
                                    return
                                }
                                
                                if (phoneField.text === "") {
                                    errorLabel.text = "Please enter phone number"
                                    return
                                }
                                
                                // Place order
                                checkoutPage.placeOrderClicked()
                            }
                        }

                        Label {
                            text: "🔒 Secure checkout"
                            font.pixelSize: 12
                            color: "#999"
                            Layout.alignment: Qt.AlignHCenter
                        }
                    }
                }
            }
        }
    }
}
