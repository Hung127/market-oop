import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Column {
    id: root
    property var model
    width: parent ? parent.width - 40 : 1200
    leftPadding: 20
    rightPadding: 20
    spacing: 16

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
                    text: qsTr("This Month")
                    font.pixelSize: 16
                    font.bold: true
                    color: "#DB4444"
                }
            }
            Text {
                text: qsTr("Best Selling Products")
                font.pixelSize: 36
                font.bold: true
                color: "#000000"
            }
        }

        Item { Layout.fillWidth: true }  // Giờ đây hoạt động đúng trong RowLayout

        Button {
            text: qsTr("View All")
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
        width: parent.width - parent.leftPadding - parent.rightPadding
        spacing: 16
        Repeater {
            model: root.model
            delegate: ProductCard {
                Layout.preferredWidth: 270
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