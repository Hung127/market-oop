import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: card
    width: 270
    height: 350
    radius: 4
    color: "#FFFFFF"
    border.color: "#E0E0E0"
    border.width: 1

    property alias productName: nameText.text
    property real price: 0
    property real oldPrice: 0
    property int discount: 0
    property real rating: 0
    property int reviewCount: 0
    property url imageUrl
    property bool isNew: false
    property bool isFavorite: false

    Column {
        anchors.fill: parent
        anchors.margins: 0
        spacing: 0

        // Image container with overlay buttons
        Rectangle {
            id: imageFrame
            width: parent.width
            height: 200
            radius: 4
            color: "#F5F5F5"

            Image {
                id: productImage
                anchors.centerIn: parent
                source: card.imageUrl
                fillMode: Image.PreserveAspectFit
                width: parent.width * 0.7
                height: parent.height * 0.7

                // Placeholder if image not loaded
                Rectangle {
                    anchors.fill: parent
                    color: "#E0E0E0"
                    visible: productImage.status === Image.Error || productImage.status === Image.Null
                    
                    Text {
                        anchors.centerIn: parent
                        text: "No Image"
                        color: "#808080"
                        font.pixelSize: 14
                    }
                }
            }

            // Discount badge (top-left)
            Rectangle {
                visible: card.discount > 0
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.margins: 12
                radius: 4
                color: "#DB4444"
                implicitWidth: discountText.implicitWidth + 12
                implicitHeight: 26

                Text {
                    id: discountText
                    anchors.centerIn: parent
                    text: "-" + card.discount + "%"
                    color: "#FFFFFF"
                    font.pixelSize: 12
                    font.bold: true
                }
            }

            // New badge (top-left, if no discount)
            Rectangle {
                visible: card.isNew && card.discount === 0
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.margins: 12
                radius: 4
                color: "#00FF66"
                implicitWidth: newText.implicitWidth + 12
                implicitHeight: 26

                Text {
                    id: newText
                    anchors.centerIn: parent
                    text: "NEW"
                    color: "#FFFFFF"
                    font.pixelSize: 12
                    font.bold: true
                }
            }

            // Action buttons (top-right)
            Column {
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.margins: 12
                spacing: 8

                // Favorite button
                ToolButton {
                    id: favoriteBtn
                    width: 34
                    height: 34

                    background: Rectangle {
                        radius: 17
                        color: "#FFFFFF"
                        border.color: "#E0E0E0"
                        border.width: 1
                    }

                    contentItem: Image {
                        source: card.isFavorite ? "qrc:/icons/heart_filled.svg" : "qrc:/icons/heart.svg"
                        width: 20
                        height: 20
                        fillMode: Image.PreserveAspectFit
                        anchors.centerIn: parent
                    }

                    onClicked: {
                        card.isFavorite = !card.isFavorite
                        console.log("Favorite toggled:", card.isFavorite)
                    }
                }

                // Quick view button
                ToolButton {
                    width: 34
                    height: 34

                    background: Rectangle {
                        radius: 17
                        color: "#FFFFFF"
                        border.color: "#E0E0E0"
                        border.width: 1
                    }

                    contentItem: Image {
                        source: "qrc:/icons/eye.svg"
                        width: 20
                        height: 20
                        fillMode: Image.PreserveAspectFit
                        anchors.centerIn: parent
                    }

                    onClicked: {
                        console.log("Quick view:", card.productName)
                    }
                }
            }

            // Add to Cart button (appears on hover)
            Rectangle {
                id: addToCartBtn
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                height: 41
                color: "#000000"
                opacity: cardMouseArea.containsMouse ? 1.0 : 0.0
                visible: opacity > 0

                Behavior on opacity {
                    NumberAnimation { duration: 200 }
                }

                RowLayout {
                    anchors.centerIn: parent
                    spacing: 8

                    Image {
                        source: "qrc:/icons/cart_white.svg"
                        width: 20
                        height: 20
                        fillMode: Image.PreserveAspectFit
                    }

                    Text {
                        text: qsTr("Add To Cart")
                        color: "#FFFFFF"
                        font.pixelSize: 14
                        font.bold: true
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        console.log("Add to cart:", card.productName)
                    }
                }
            }
        }

        // Product info
        Column {
            width: parent.width
            padding: 12
            spacing: 8

            Text {
                id: nameText
                text: "Product Name"
                font.pixelSize: 16
                font.bold: true
                color: "#000000"
                wrapMode: Text.WordWrap
                width: parent.width - parent.padding * 2
                maximumLineCount: 2
                elide: Text.ElideRight
            }

            Row {
                spacing: 12

                Text {
                    text: "$" + card.price.toFixed(0)
                    color: "#DB4444"
                    font.pixelSize: 16
                    font.bold: true
                }

                Text {
                    visible: card.oldPrice > 0
                    text: "$" + card.oldPrice.toFixed(0)
                    color: "#808080"
                    font.pixelSize: 16
                    font.strikeout: true
                }
            }

            Row {
                spacing: 8

                // Star rating
                Row {
                    spacing: 4
                    Repeater {
                        model: 5
                        delegate: Image {
                            source: index < Math.round(card.rating) 
                                    ? "qrc:/icons/star_filled.svg" 
                                    : "qrc:/icons/star_empty.svg"
                            width: 16
                            height: 16
                            fillMode: Image.PreserveAspectFit
                            
                            // Fallback to colored rectangle if icons not available
                            Rectangle {
                                anchors.fill: parent
                                radius: 8
                                color: index < Math.round(card.rating) ? "#FFAD33" : "#E0E0E0"
                                visible: parent.status === Image.Error || parent.status === Image.Null
                            }
                        }
                    }
                }

                Text {
                    text: "(" + card.reviewCount + ")"
                    font.pixelSize: 14
                    color: "#808080"
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
        }
    }

    // Hover effects
    MouseArea {
        id: cardMouseArea
        anchors.fill: parent
        hoverEnabled: true
        propagateComposedEvents: true
        
        onEntered: {
            card.border.color = "#DB4444"
            card.border.width = 2
        }
        
        onExited: {
            card.border.color = "#E0E0E0"
            card.border.width = 1
        }

        onClicked: {
            console.log("Product card clicked:", card.productName)
        }
    }

    // Drop shadow effect (optional, for better visual)
    layer.enabled: true
    layer.effect: DropShadow {
        transparentBorder: true
        horizontalOffset: 0
        verticalOffset: 2
        radius: 8.0
        samples: 17
        color: "#10000000"
    }
}

// Import for DropShadow effect
import QtGraphicalEffects 1.15