import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    id: window
    width: 1440
    height: 900
    visible: true
    title: qsTr("Exclusive E-Commerce")

    header: HeaderBar {
        id: headerBar
        height: 80
    }

    RowLayout {
        anchors.fill: parent
        anchors.topMargin: headerBar.height

        CategoryPanel {
            id: categoryPanel
            Layout.preferredWidth: 260
            Layout.fillHeight: true
        }

        Flickable {
            id: mainScroll
            Layout.fillWidth: true
            Layout.fillHeight: true
            contentWidth: width
            contentHeight: contentColumn.implicitHeight
            clip: true

            Column {
                id: contentColumn
                width: mainScroll.width
                spacing: 32

                HeroBanner {
                    id: heroBanner
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: parent.width
                }

                FlashSaleSection {
                    width: parent.width
                    model: flashSaleModel
                }

                BrowseCategorySection {
                    width: parent.width
                }

                BestSellingSection {
                    width: parent.width
                    model: bestSellingModel
                }

                ExploreProductsSection {
                    width: parent.width
                    model: allProductsModel
                }

                ServicesStrip {
                    width: parent.width
                }

                FooterBar {
                    width: parent.width
                }

                Rectangle { // padding bottom
                    width: parent.width
                    height: 40
                    color: "transparent"
                }
            }
        }
    }
}