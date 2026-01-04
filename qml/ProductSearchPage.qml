import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Page {
    id: productSearchPage
    
    property string userName: authController.currentUserName
    
    signal backClicked()
    signal productClicked(string productId)
    signal addToCartClicked(string productId)
    
    Component.onCompleted: {
        console.log("[ProductSearchPage] Loading all products...")
        refreshProducts()
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
                
                onClicked: productSearchPage.backClicked()
            }
            
            Label {
                text: "🔍 Search Products"
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
        
        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 20
            spacing:  20
            
            // Search & Filter Section
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 120
                color: "white"
                radius: 10
                border.color: "#e0e0e0"
                border.width: 1
                
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 20
                    spacing: 15
                    
                    // Search bar
                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 10
                        
                        TextField {
                            id: searchField
                            Layout.fillWidth: true
                            placeholderText: "Search products by name..."
                            font.pixelSize: 14
                            
                            background: Rectangle {
                                color:  searchField.focus ? "#f0f8ff" : "#f5f5f5"
                                border.color: searchField.focus ? "#2196F3" : "#ddd"
                                border.width: 2
                                radius: 5
                            }
                            
                            padding: 12
                            
                            Keys.onReturnPressed: searchButton.clicked()
                        }
                        
                        Button {
                            id: searchButton
                            text: "🔍 Search"
                            font.pixelSize: 14
                            font.bold: true
                            
                            contentItem: Text {
                                text: parent.text
                                font: parent.font
                                color: "white"
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment:  Text.AlignVCenter
                            }
                            
                            background: Rectangle {
                                color:  parent.pressed ? "#1976D2" : (parent.hovered ? "#1E88E5" : "#2196F3")
                                radius: 5
                            }
                            
                            onClicked: {
                                console.log("[ProductSearchPage] Searching:", searchField.text)
                                searchProducts()
                            }
                        }
                        
                        Button {
                            text: "Clear"
                            font.pixelSize: 14
                            
                            contentItem: Text {
                                text: parent.text
                                font: parent.font
                                color: "#666"
                                horizontalAlignment:  Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                            
                            background: Rectangle {
                                color: parent.hovered ? "#f0f0f0" : "white"
                                border.color: "#ddd"
                                border.width: 1
                                radius:  5
                            }
                            
                            onClicked:  {
                                searchField.text = ""
                                refreshProducts()
                            }
                        }
                    }
                    
                    // Price filter
                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 10
                        
                        Label {
                            text:  "Price Range:"
                            font.pixelSize: 14
                            color: "#666"
                        }
                        
                        TextField {
                            id: minPriceField
                            Layout.preferredWidth: 100
                            placeholderText:  "Min"
                            validator: DoubleValidator { bottom: 0 }
                            font.pixelSize: 14
                            
                            background: Rectangle {
                                color: "#f5f5f5"
                                border.color: "#ddd"
                                border.width: 1
                                radius: 5
                            }
                        }
                        
                        Label {
                            text: "to"
                            font.pixelSize: 14
                        }
                        
                        TextField {
                            id: maxPriceField
                            Layout.preferredWidth: 100
                            placeholderText: "Max"
                            validator: DoubleValidator { bottom: 0 }
                            font.pixelSize: 14
                            
                            background: Rectangle {
                                color: "#f5f5f5"
                                border.color: "#ddd"
                                border.width: 1
                                radius: 5
                            }
                        }
                        
                        Button {
                            text: "Apply"
                            font.pixelSize: 14
                            
                            contentItem: Text {
                                text: parent.text
                                font: parent.font
                                color: "#2196F3"
                            }
                            
                            background: Rectangle {
                                color:  parent.hovered ? "#E3F2FD" : "transparent"
                                border.color: "#2196F3"
                                border.width: 1
                                radius: 5
                            }
                            
                            onClicked: filterByPrice()
                        }
                        
                        Item { Layout.fillWidth: true }
                        
                        Label {
                            text: resultCountLabel.text
                            font.pixelSize: 14
                            font.bold: true
                            color: "#4CAF50"
                        }
                    }
                }
            }
            
            // Results Grid
            ScrollView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true
                
                GridView {
                    id: productGrid
                    cellWidth: 250
                    cellHeight: 320
                    
                    model: ListModel {
                        id: productListModel
                    }
                    
                    Label {
                        id: resultCountLabel
                        visible: false
                        text: ""
                    }
                    
                    // Empty state
                    Label {
                        visible: productGrid.count === 0
                        anchors.centerIn: parent
                        text: "No products found"
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
                            onExited:  parent.border.color = "#e0e0e0"
                            onClicked: productSearchPage.productClicked(model.productId)
                        }
                        
                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 15
                            spacing: 10
                            
                            // Product Image
                            Label {
                                text: model.image
                                font.pixelSize: 60
                                Layout.alignment: Qt.AlignHCenter
                            }
                            
                            // Product Name
                            Label {
                                text: model.name
                                font.pixelSize: 14
                                font.bold: true
                                color: "#333"
                                Layout.fillWidth: true
                                elide: Text.ElideRight
                                wrapMode: Text.WordWrap
                                maximumLineCount: 2
                            }
                            
                            // Seller
                            Label {
                                text: "by " + model.sellerName
                                font.pixelSize: 11
                                color: "#999"
                            }
                            
                            // Stock
                            Label {
                                text: model.stock > 0 ? "In Stock (" + model.stock + ")" : "Out of Stock"
                                font.pixelSize: 10
                                color: model.stock > 0 ? "#4CAF50" : "#F44336"
                            }
                            
                            Item { Layout.fillHeight: true }
                            
                            // Price
                            Label {
                                text: "$" + model.price.toFixed(2)
                                font.pixelSize: 18
                                font.bold: true
                                color: "#4CAF50"
                            }
                            
                            // Add to Cart Button
                            Button {
                                Layout.fillWidth: true
                                text: "Add to Cart"
                                font.pixelSize: 12
                                enabled: model.stock > 0
                                
                                contentItem: Text {
                                    text: parent.text
                                    font: parent.font
                                    color: "white"
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                }
                                
                                background: Rectangle {
                                    color: parent.enabled ? 
                                        (parent.pressed ? "#1976D2" :  (parent.hovered ? "#1E88E5" : "#2196F3"))
                                        : "#ccc"
                                    radius:  5
                                }
                                
                                onClicked:  {
                                    console.log("[ProductSearchPage] Add to cart:", model.name)
                                    productSearchPage.addToCartClicked(model.productId)
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    // Functions
    function refreshProducts() {
        console.log("[ProductSearchPage] Refreshing all products...")
        var products = authController.productController.getAllProducts()
        updateProductList(products)
    }
    
    function searchProducts() {
        var keyword = searchField.text.trim()
        console.log("[ProductSearchPage] Searching for:", keyword)
        var products = authController.productController.searchProducts(keyword)
        updateProductList(products)
    }
    
    function filterByPrice() {
        var min = parseFloat(minPriceField.text) || 0
        var max = parseFloat(maxPriceField.text) || 999999
        
        console.log("[ProductSearchPage] Filtering by price:", min, "-", max)
        var products = authController.productController.getProductsByPriceRange(min, max)
        updateProductList(products)
    }
    
    function updateProductList(products) {
        console.log("[ProductSearchPage] Updating product list with", products.length, "items")
        
        productListModel.clear()
        
        for (var i = 0; i < products.length; i++) {
            productListModel.append(products[i])
        }
        
        resultCountLabel.text = products.length + " product(s) found"
    }
}
