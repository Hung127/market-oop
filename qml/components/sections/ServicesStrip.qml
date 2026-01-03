import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../products"
import "../products"
Item {
    id: root
    width: parent ? parent.width : 1440
    implicitHeight: contentItem.implicitHeight + 48

    Item {
        id: contentItem
        width: Math.min(parent.width - 80, 1200)
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 48

        // SỬA LỖI: Dùng Repeater thay vì tạo dynamic objects
        RowLayout {
            id: row
            anchors.fill: parent
            spacing: 32

            Repeater {
                model: [
                    {
                        icon: "qrc:/icons/icon_delivery.svg",
                        title: "FREE AND FAST DELIVERY",
                        desc: "Free delivery for all orders over $140"
                    },
                    {
                        icon: "qrc:/icons/icon_customer_service.svg",
                        title: "24/7 CUSTOMER SERVICE",
                        desc: "Friendly 24/7 customer support"
                    },
                    {
                        icon: "qrc:/icons/icon_secure.svg",
                        title: "MONEY BACK GUARANTEE",
                        desc: "We return money within 30 days"
                    }
                ]

                delegate: Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 160
                    radius: 4
                    color: "#F5F5F5"

                    Column {
                        anchors.centerIn: parent
                        spacing: 12
                        width: parent.width * 0.9

                        Rectangle {
                            width: 48
                            height: 48
                            radius: 24
                            color: "#DF4244"
                            anchors.horizontalCenter: parent.horizontalCenter

                            Image {
                                anchors.centerIn: parent
                                source: modelData.icon
                                width: 24
                                height: 24
                                fillMode: Image.PreserveAspectFit
                            }
                        }

                        Text {
                            text: modelData.title
                            font.pixelSize: 18
                            font.bold: true
                            color: "#000000"
                            horizontalAlignment: Text.AlignHCenter
                            width: parent.width
                            wrapMode: Text.WordWrap
                        }

                        Text {
                            text: modelData.desc
                            font.pixelSize: 14
                            color: "#808080"
                            horizontalAlignment: Text.AlignHCenter
                            width: parent.width
                            wrapMode: Text.WordWrap
                        }
                    }
                }
            }
        }
    }
}
