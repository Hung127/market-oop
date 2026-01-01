import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    width: parent ? parent.width : 1440
    implicitHeight: contentItem.implicitHeight + 32

    // model: ProductModel từ C++ (đã set trong Main.qml)
    property var model

    Item {
        id: contentItem
        width: Math.min(parent.width - 80, 1200)
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 32

        Column {
            anchors.left: parent.left
            anchors.right: parent.right
            spacing: 24

            // Header
            RowLayout {
                spacing: 24
                Layout.fillWidth: true

                Column {
                    spacing: 8

                    Row {
                        spacing: 8
                        Rectangle {
                            width: 20
                            height: 40
                            radius: 4
                            color: "#DB4444"
                        }
                        Text {
                            text: qsTr("Our Products")
                            font.pixelSize: 16
                            font.bold: true
                            color: "#DB4444"
                        }
                    }

                    Text {
                        text: qsTr("Explore Our Products")
                        font.pixelSize: 36
                        font.bold: true
                        color: "#000000"
                    }
                }

                Item { Layout.fillWidth: true }

                // Arrows giống section flash sale / categories (Frame 616)
                Row {
                    spacing: 12
                    ToolButton {
                        width: 32; height: 32
                        background: Rectangle { radius: 16; color: "#F5F5F5" }
                        icon.source: "qrc:/icons/arrow_left.svg"
                    }
                    ToolButton {
                        width: 32; height: 32
                        background: Rectangle { radius: 16; color: "#F5F5F5" }
                        icon.source: "qrc:/icons/arrow_right.svg"
                    }
                }
            }

            // Grid 4 cột, giống bố cục các Cart trong Figma
            GridLayout {
                id: productGrid
                columns: 4
                columnSpacing: 24
                rowSpacing: 24
                Layout.fillWidth: true

                Repeater {
                    model: root.model
                    delegate: ProductCard {
                        Layout.fillWidth: true
                        Layout.preferredWidth: (productGrid.width - (productGrid.columns - 1) * productGrid.columnSpacing) / productGrid.columns

                        productName: model.name
                        price: model.price
                        oldPrice: model.oldPrice
                        discount: model.discount
                        rating: model.rating
                        reviewCount: model.reviewCount
                        imageUrl: model.imageUrl
                    }
                }
            }

            // Optional: nút "View All Products" phía dưới (một số variant của thiết kế có)
            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("View All Products")
                background: Rectangle {
                    radius: 4
                    color: "#DB4444"
                }
                contentItem: Text {
                    text: parent.text
                    color: "#FFFFFF"
                    font.pixelSize: 16
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }
    }
}