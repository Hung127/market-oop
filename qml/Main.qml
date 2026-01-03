import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import "components/layout"
import "components/sections"
import "components/products"
import "components/common"
import "components/dialogs"
import "pages"
import "styles"

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
        spacing: 0

        // ===== LEFT SIDEBAR =====
        CategoryPanel {
            Layout.preferredWidth: 260
            Layout.fillHeight: true
        }

        // ===== MAIN CONTENT =====
        Flickable {
            id: mainScroll
            Layout.fillWidth: true
            Layout.fillHeight: true

            clip: true
            boundsBehavior: Flickable.StopAtBounds
            pressDelay: 120

            contentWidth: width
            contentHeight: contentColumn.implicitHeight

            Column {
                id: contentColumn
                width: mainScroll.width
                spacing: 48

                HeroBanner { width: parent.width }

                FlashSaleSection {
                    width: parent.width
                    model: flashSaleModel
                }

                BrowseCategorySection { width: parent.width }

                BestSellingSection {
                    width: parent.width
                    model: bestSellingModel
                }

                ExploreProductsSection {
                    width: parent.width
                    model: allProductsModel
                }

                ServicesStrip { width: parent.width }

                FooterBar { width: parent.width }

                Rectangle {
                    width: parent.width
                    height: 40
                    color: "transparent"
                }
            }
        }
    }
}
