import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick. Layouts 1.12

Page {
    id: addEditProductPage

    property bool isEditMode: false
    property int productId: 0
    
    signal backClicked()
    signal saveClicked(string name, string description, real price, int stock, string category)

    header: ToolBar {
        background: Rectangle {
            color: "#4CAF50"
        }

        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 20
            anchors. rightMargin: 20
            spacing: 20

            Button {
                text:  "← Back"
                font.pixelSize: 14
                flat: true
                
                contentItem: Text {
                    text:  parent.text
                    font: parent.font
                    color: "white"
                }
                
                onClicked: addEditProductPage.backClicked()
            }

            Label {
                text: isEditMode ? "Edit Product" :  "Add New Product"
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

            Rectangle {
                width: Math.min(800, parent.parent.width - 60)
                height: contentLayout.height + 40
                anchors.horizontalCenter: parent.horizontalCenter
                color: "white"
                radius: 10
                border.color: "#e0e0e0"
                border.width: 1

                ColumnLayout {
                    id: contentLayout
                    anchors.fill: parent
                    anchors.margins: 40
                    spacing: 25

                    Label {
                        text: isEditMode ? "Edit Product Information" : "Product Information"
                        font.pixelSize: 22
                        font.bold: true
                        color: "#333"
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        height: 1
                        color:  "#e0e0e0"
                    }

                    // Product Name
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing:  8

                        Label {
                            text: "Product Name *"
                            font.pixelSize: 14
                            font.bold: true
                            color: "#333"
                        }

                        TextField {
                            id: nameField
                            Layout.fillWidth: true
                            placeholderText: "Enter product name"
                            font.pixelSize: 14
                            selectByMouse: true
                            
                            background: Rectangle {
                                color:  nameField.focus ? "#f0f8ff" : "#f5f5f5"
                                border.color: nameField.focus ? "#4CAF50" :  "#ddd"
                                border.width: 2
                                radius: 5
                            }
                            
                            padding: 12
                        }
                    }

                    // Description
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 8

                        Label {
                            text: "Description *"
                            font.pixelSize: 14
                            font.bold: true
                            color: "#333"
                        }

                        ScrollView {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 120
                            clip: true

                            TextArea {
                                id: descriptionField
                                placeholderText: "Enter product description"
                                font.pixelSize: 14
                                selectByMouse: true
                                wrapMode: TextArea. Wrap
                                
                                background: Rectangle {
                                    color: descriptionField.focus ? "#f0f8ff" : "#f5f5f5"
                                    border.color: descriptionField.focus ? "#4CAF50" : "#ddd"
                                    border.width: 2
                                    radius:  5
                                }
                            }
                        }
                    }

                    // Price
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 8

                        Label {
                            text: "Price (USD) *"
                            font.pixelSize: 14
                            font.bold: true
                            color: "#333"
                        }

                        TextField {
                            id: priceField
                            Layout.preferredWidth: 200
                            placeholderText:  "0.00"
                            font. pixelSize: 14
                            selectByMouse: true
                            validator: DoubleValidator {
                                bottom: 0
                                decimals: 2
                            }
                            
                            background: Rectangle {
                                color:  priceField.focus ? "#f0f8ff" : "#f5f5f5"
                                border.color: priceField.focus ? "#4CAF50" : "#ddd"
                                border.width: 2
                                radius: 5
                            }
                            
                            padding: 12
                            leftPadding: 30
                            
                            Label {
                                anchors.left:  parent.left
                                anchors.leftMargin: 12
                                anchors.verticalCenter: parent.verticalCenter
                                text: "$"
                                font.pixelSize: 14
                                color: "#666"
                            }
                        }
                    }

                    // Stock Quantity
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 8

                        Label {
                            text: "Stock Quantity *"
                            font.pixelSize: 14
                            font. bold: true
                            color:  "#333"
                        }

                        SpinBox {
                            id:  stockField
                            Layout.preferredWidth: 200
                            from: 0
                            to:  9999
                            value: 0
                            editable: true
                        }
                    }

                    // Category
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 8

                        Label {
                            text: "Category *"
                            font.pixelSize: 14
                            font.bold: true
                            color: "#333"
                        }

                        ComboBox {
                            id:  categoryCombo
                            Layout.preferredWidth: 300
                            model: ["Electronics", "Clothing", "Books", "Home & Garden", "Sports", "Toys", "Other"]
                            
                            background: Rectangle {
                                color: categoryCombo.focus ? "#f0f8ff" : "#f5f5f5"
                                border.color: categoryCombo.focus ? "#4CAF50" : "#ddd"
                                border.width: 2
                                radius: 5
                            }
                        }
                    }

                    // Error/Success Message
                    Label {
                        id: messageLabel
                        text: ""
                        font.pixelSize: 14
                        Layout.fillWidth: true
                        wrapMode: Text.WordWrap
                        visible: text !== ""
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        height: 1
                        color: "#e0e0e0"
                    }

                    // Action Buttons
                    RowLayout {
                        Layout.fillWidth: true
                        spacing:  15

                        Button {
                            text: "Cancel"
                            font.pixelSize: 16
                            Layout.preferredWidth: 150
                            
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
                                border.width: 1
                                radius:  5
                            }
                            
                            Layout.preferredHeight: 50
                            
                            onClicked: addEditProductPage.backClicked()
                        }

                        Item { Layout.fillWidth: true }

                        Button {
                            text: isEditMode ? "Update Product" : "Add Product"
                            font.pixelSize: 16
                            font.bold: true
                            Layout.preferredWidth: 200
                            
                            contentItem: Text {
                                text:  parent.text
                                font: parent.font
                                color: "white"
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text. AlignVCenter
                            }
                            
                            background:  Rectangle {
                                color: parent.pressed ? "#388E3C" :  (parent.hovered ? "#43A047" : "#4CAF50")
                                radius: 5
                            }
                            
                            Layout.preferredHeight: 50
                            
                            onClicked: {
                                messageLabel.text = ""
                                messageLabel.color = "#F44336"
                                
                                // Validation
                                if (nameField.text === "") {
                                    messageLabel.text = "Please enter product name"
                                    return
                                }
                                
                                if (descriptionField. text === "") {
                                    messageLabel.text = "Please enter product description"
                                    return
                                }
                                
                                if (priceField. text === "" || parseFloat(priceField.text) <= 0) {
                                    messageLabel.text = "Please enter a valid price"
                                    return
                                }
                                
                                // Success
                                messageLabel.color = "#4CAF50"
                                messageLabel.text = isEditMode ? "Product updated successfully!" : "Product added successfully!"
                                
                                addEditProductPage.saveClicked(
                                    nameField.text,
                                    descriptionField. text,
                                    parseFloat(priceField.text),
                                    stockField.value,
                                    categoryCombo. currentText
                                )
                                
                                // Clear form if not in edit mode
                                if (!isEditMode) {
                                    clearForm()
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    function clearForm() {
        nameField.text = ""
        descriptionField.text = ""
        priceField.text = ""
        stockField.value = 0
        categoryCombo.currentIndex = 0
        messageLabel.text = ""
    }

    function loadProduct(id, name, description, price, stock, category) {
        productId = id
        nameField.text = name
        descriptionField.text = description
        priceField. text = price.toString()
        stockField.value = stock
        
        // Find category index
        for (var i = 0; i < categoryCombo. model.length; i++) {
            if (categoryCombo.model[i] === category) {
                categoryCombo.currentIndex = i
                break
            }
        }
    }
}
