import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Page {
    id: buyerHomePage

    property string userName: authController.currentUserName
    
    signal logoutClicked()
    signal productClicked(string productId)
    signal cartClicked()
    signal profileClicked()

    Component.onCompleted: {
        console.log("[BuyerHomePage] Component completed")
        console.log("[BuyerHomePage] Loading products...")
        authController.productModel.loadProducts()
        
        // Log the model count after loading
        Qt.callLater(function() {
            console.log("[BuyerHomePage] Product count:", authController.productModel.rowCount())
        })
    }

    // Toast notification for cart actions
    Rectangle {
        id: toast
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 50
        width: toastLabel.width + 40
        height: 50
        color: "#4CAF50"
        radius: 25
        opacity: 0
        z: 1000
        
        Label {
            id: toastLabel
            anchors.centerIn: parent
            color: "white"
            font.pixelSize: 14
            font.bold: true
        }
        
        PropertyAnimation {
            id: toastAnimation
            target: toast
            property: "opacity"
            from: 0
            to: 1
            duration: 200
        }
        
        PropertyAnimation {
            id: toastFadeOut
            target: toast
            property: "opacity"
            from: 1
            to:  0
            duration: 200
        }
        
        Timer {
            id: toastTimer
            interval:  2000
            onTriggered: toastFadeOut.start()
        }
    }
    
    function showToast(message) {
        toastLabel.text = message
        toastAnimation.start()
        toastTimer.restart()
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

            Label {
                text: "🛒 Market App"
                font.pixelSize: 24
                font.bold: true
                color: "white"
            }

            Item { Layout.fillWidth: true }

            // Search Bar
            TextField {
                id:  searchField
                Layout.preferredWidth: 300
                placeholderText: "Search products..."
                
                background: Rectangle {
                    color:  "white"
                    radius: 20
                }
                
                leftPadding: 15
                rightPadding: 15
                
                onAccepted: {
                    console.log("[BuyerHomePage] Searching for:", text)
                    authController.productModel.searchProducts(text)
                }
            }

            Button {
                text: "🔍"
                font.pixelSize: 18
                flat: true
                
                contentItem: Text {
                    text: parent.text
                    font: parent.font
                    color: "white"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment:  Text.AlignVCenter
                }
                
                onClicked: {
                    console.log("[BuyerHomePage] Searching for:", searchField.text)
                    authController.productModel.searchProducts(searchField.text)
                }
            }

            Item { Layout.fillWidth: true }

            // Cart Button with Item Count
            Button {
                text: "🛒 Cart (" + authController.cartController.itemCount + ")"
                font.pixelSize: 14
                flat: true
                
                contentItem: Text {
                    text: parent.text
                    font: parent.font
                    color: "white"
                }
                
                onClicked:  buyerHomePage.cartClicked()
            }

            Button {
                text: "👤 " + userName
                font.pixelSize: 14
                flat: true
                
                contentItem: Text {
                    text: parent.text
                    font: parent.font
                    color: "white"
                }
                
                onClicked: buyerHomePage.profileClicked()
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
                    color:  parent.pressed ? "#D32F2F" : (parent.hovered ? "#E53935" : "#F44336")
                    radius: 5
                }
                
                onClicked: buyerHomePage.logoutClicked()
            }
        }
    }

    // Main Content
    Rectangle {
        anchors.fill: parent
        color: "#f5f5f5"

        RowLayout {
            anchors.fill: parent
            spacing: 0

            // Sidebar
            Rectangle {
                Layout.preferredWidth: 250
                Layout.fillHeight: true
                color: "white"
                border.color: "#e0e0e0"
                border.width: 1

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 20
                    spacing: 10

                    Label {
                        text: "Categories"
                        font.pixelSize: 18
                        font.bold: true
                        color: "#333"
                    }

                    Repeater {
                        model:  ["All Products", "Laptop", "Mouse", "Keyboard", "Monitor", "Phone", "Headphone", "Watch", "Speaker"]
                        
                        Button {
                            Layout.fillWidth: true
                            text: modelData
                            flat: true
                            
                            contentItem: Text {
                                text: parent.text
                                font.pixelSize: 14
                                color: "#666"
                                horizontalAlignment: Text.AlignLeft
                            }
                            
                            background: Rectangle {
                                color:  parent.hovered ? "#f0f0f0" : "transparent"
                                radius: 5
                            }
                            
                            onClicked: {
                                console.log("[BuyerHomePage] Filter by category:", modelData)
                                if (modelData === "All Products") {
                                    authController.productModel.loadProducts()
                                } else {
                                    authController.productModel.filterByCategory(modelData)
                                }
                            }
                        }
                    }

                    Item { Layout.fillHeight: true }
                }
            }

            // Product Grid
            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: "#f5f5f5"

                ScrollView {
                    anchors.fill: parent
                    anchors.margins: 20
                    clip: true

                    GridView {
                        id: productGrid
                        cellWidth: 250
                        cellHeight: 320
                        
                        // Use the ProductModel from AuthController
                        model: authController.productModel

                        // Show message when no products
                        Label {
                            visible: productGrid.count === 0
                            anchors.centerIn: parent
                            text: "No products available"
                            font.pixelSize: 18
                            color: "#999"
                        }

                        delegate: Rectangle {
                            width: 230
                            height: 300
                            color: "white"
                            radius: 10
                            border.color: "#e0e0e0"
                            border.width: 1

                            MouseArea {
                                anchors.fill: parent
                                hoverEnabled: true
                                cursorShape: Qt.PointingHandCursor
                                
                                onEntered: parent.border.color = "#2196F3"
                                onExited: parent.border.color = "#e0e0e0"
                                onClicked: {
                                    console.log("[BuyerHomePage] Product clicked:", model.productId)
                                    buyerHomePage.productClicked(model.productId)
                                }
                            }

                            ColumnLayout {
                                anchors.fill: parent
                                anchors.margins: 15
                                spacing: 10

                                // Product Image
                                Label {
                                    text: model.image
                                    font.pixelSize: 80
                                    Layout.alignment: Qt.AlignHCenter
                                }

                                // Product Name
                                Label {
                                    text: model.name
                                    font.pixelSize: 16
                                    font.bold: true
                                    color: "#333"
                                    Layout.fillWidth: true
                                    elide: Text.ElideRight
                                    wrapMode: Text.WordWrap
                                    maximumLineCount: 2
                                }

                                // Seller
                                Label {
                                    text: "by " + model.seller
                                    font.pixelSize: 12
                                    color: "#999"
                                }

                                // Stock indicator
                                Label {
                                    text: model.stock > 0 ? "In Stock (" + model.stock + ")" : "Out of Stock"
                                    font.pixelSize: 11
                                    color: model.stock > 0 ? "#4CAF50" : "#F44336"
                                }

                                Item { Layout.fillHeight: true }

                                // Price
                                Label {
                                    text: "$" + model.price.toFixed(2)
                                    font.pixelSize: 20
                                    font.bold: true
                                    color: "#4CAF50"
                                }

                                // Add to Cart Button
                                Button {
                                    Layout.fillWidth: true
                                    text: "Add to Cart"
                                    font.pixelSize: 14
                                    enabled: model.stock > 0
                                    
                                    contentItem: Text {
                                        text: parent.text
                                        font:  parent.font
                                        color: "white"
                                        horizontalAlignment: Text.AlignHCenter
                                        verticalAlignment: Text.AlignVCenter
                                    }
                                    
                                    background: Rectangle {
                                        color: parent.enabled ? 
                                            (parent.pressed ? "#1976D2" : (parent.hovered ? "#1E88E5" : "#2196F3")) 
                                            : "#ccc"
                                        radius:  5
                                    }
                                    
                                    onClicked: {
                                        console.log("[BuyerHomePage] Add to cart:", model.name)
                                        var success = authController.cartController.addToCart(model.productId, 1)
                                        if (success) {
                                            showToast("✓ Added " + model.name + " to cart")
                                        } else {
                                            showToast("✗ Failed to add to cart")
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
