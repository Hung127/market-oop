import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../common"
import "../products"
Column {
    id: root
    property var model

    anchors.horizontalCenter: parent ? parent.horizontalCenter : undefined
    width: parent ? parent.width - 40 : 1200
    spacing: 16
    leftPadding: 20
    rightPadding: 20

    RowLayout {  // ĐỔI từ Row sang RowLayout
        width: parent.width - parent.leftPadding - parent.rightPadding
        spacing: 16
        
        Column {
            spacing: 8
            Row {
                spacing: 8
                Rectangle {
                    width: 20; height: 40
                    radius: 4
                    color: "#DB4444"
                }
                Text {
                    text: qsTr("Today's")
                    font.pixelSize: 16
                    font.bold: true
                    color: "#DB4444"
                }
            }
            Text {
                text: qsTr("Flash Sales")
                font.pixelSize: 36
                font.bold: true
                color: "#000000"
            }
        }

        Loader {
            Layout.alignment: Qt.AlignVCenter
            source: "qrc:/qml/qml/components/common/FlashSaleTimer.qml"
        }


        Item { Layout.fillWidth: true }  // Giờ đây hoạt động đúng trong RowLayout

        // Nút điều hướng / View All
        Button {
            text: qsTr("View All Products")
            background: Rectangle {
                radius: 4
                color: "#DB4444"
            }
            contentItem: Text {
                text: parent.text
                color: "white"
                font.pixelSize: 16
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }
    }

    RowLayout {
        id: productRow
        width: parent.width - parent.leftPadding - parent.rightPadding
        spacing: 16

        Repeater {
            model: root.model
            delegate: ProductCard {
                Layout.preferredWidth: 270
                Layout.fillHeight: true

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
}
