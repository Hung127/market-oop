import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.12

ApplicationWindow {
    id: root
    visible: true
    width: 1280
    height: 800
    title: "Market App"

    Connections {
        target:  authController
        
        function onLoginSuccess() {
            console.log("[Main] ✓ Login success detected")
            var role = authController.currentUserRole
            console.log("[Main] User role:", role)
            
            // Navigate based on role
            if (role === "BUYER") {
                console.log("[Main] → Navigating to Buyer Home")
                stackView.push(buyerHomePage)
            } else if (role === "SELLER") {
                console.log("[Main] → Navigating to Seller Dashboard")
                stackView.push(sellerDashboard)
            } else {
                console.log("[Main] ✗ Unknown role:", role)
            }
        }
        
        function onRegisterSuccess() {
            console.log("[Main] ✓ Registration success - going back to login")
            // Auto-navigate back to login after 2 seconds
            registerSuccessTimer.start()
        }
    }

    // Timer for register success redirect
    Timer {
        id:  registerSuccessTimer
        interval:  2000
        repeat: false
        onTriggered: {
            stackView.pop()  // Go back to login
        }
    }

    // Stack view for page navigation
    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: loginPage
    }

    // ============================================
    // LOGIN PAGE
    // ============================================
    Component {
        id: loginPage
        
        LoginPage {
            // No need for signals - Main.qml listens to authController directly
        }
    }

    // ============================================
    // REGISTER PAGE
    // ============================================
    Component {
        id:  registerPage
        
        RegisterPage {
            onBackToLoginClicked: {
                stackView.pop()
            }
        }
    }

    // ============================================
    // BUYER HOME PAGE
    // ============================================
    Component {
        id: buyerHomePage
        
        BuyerHomePage {
            onLogoutClicked: {
                authController.logout()
                while (stackView. depth > 1) {
                            stackView.pop()
                }
            }
            
            onProductClicked: {
                console.log("[Main] Product clicked:", productId)
                stackView.push(productDetailPage, {
                    "productId": productId
                })
            }
            
            onCartClicked: {
                console.log("[Main] Cart clicked")
                stackView.push(cartPage)
            }
            
            onProfileClicked: {
                console.log("[Main] Profile clicked")
                stackView.push(profilePage)
            }
        }
    }

    // ============================================
    // SELLER DASHBOARD
    // ============================================
    Component {
        id: sellerDashboard
        
        SellerDashboard {
            onLogoutClicked: {
                authController.logout()
                stackView.clear()
                stackView.push(loginPage)
            }
            
            onAddProductClicked: {
                console.log("[Main] Add product clicked")
                stackView.push(addEditProductPage, {
                    "isEditMode": false
                })
            }
            
            onEditProductClicked: {
                console.log("[Main] Edit product:", productId)
                stackView.push(addEditProductPage, {
                    "isEditMode": true,
                    "productId": productId
                })
            }
            
            onProfileClicked: {
                console.log("[Main] Profile clicked")
                stackView.push(profilePage)
            }
        }
    }

    // ============================================
    // CART PAGE
    // ============================================
    Component {
        id: cartPage
        
        CartPage {
            onBackClicked: {
                stackView.pop()
            }
            
            onCheckoutClicked: {
                console.log("[Main] Proceed to checkout")
                stackView.push(checkoutPage)
            }
            
            onContinueShoppingClicked: {
                stackView.pop()
            }
        }
    }

    // ============================================
    // PRODUCT DETAIL PAGE
    // ============================================
    Component {
        id: productDetailPage
        
        ProductDetailPage {
            onBackClicked: {
                stackView.pop()
            }
            
            onAddToCartClicked: {
                console.log("[Main] Added to cart:", quantity, "items")
                stackView.pop()
            }
            
            onBuyNowClicked: {
                console.log("[Main] Buy now:", quantity, "items")
                stackView.pop()
            }
        }
    }

    // ============================================
    // ADD/EDIT PRODUCT PAGE
    // ============================================
    Component {
        id: addEditProductPage
        
        AddEditProductPage {
            onBackClicked:  {
                stackView.pop()
            }
            
            onSaveClicked: {
                console.log("[Main] Product saved")
                saveProductTimer.start()
            }
            
            Timer {
                id: saveProductTimer
                interval: 1500
                onTriggered: stackView.pop()
            }
        }
    }

    // ============================================
    // PROFILE PAGE
    // ============================================
    Component {
        id: profilePage
        
        ProfilePage {
            userName: authController.currentUserName
            userEmail: authController.currentUserEmail
            userRole: authController.currentUserRole
            userBalance: authController.currentBalance
            
            onBackClicked: {
                stackView.pop()
            }
            
            onSaveProfileClicked: {
                console.log("[Main] Profile saved")
            }
            
            onChangePasswordClicked: {
                console.log("[Main] Password changed")
            }
            
            onLogoutClicked: {
                authController.logout()
                while (stackView. depth > 1) {
                            stackView.pop()
                }
            }
        }
    }

    // ============================================
    // CHECKOUT PAGE
    // ============================================
    Component {
        id: checkoutPage
        
        CheckoutPage {
            onBackClicked: {
                stackView.pop()
            }
            
            onPlaceOrderClicked: {
                console.log("[Main] Order placed!")
                stackView.pop()  // Go back to cart
                stackView.pop()  // Go back to home
            }
        }
    }

    // ============================================
    // ORDER HISTORY PAGE
    // ============================================
    Component {
        id: orderHistoryPage
        
        OrderHistoryPage {
            onBackClicked: {
                stackView.pop()
            }
            
            onViewOrderDetails: {
                console.log("[Main] View order:", orderId)
            }
        }
    }

    // ============================================
    // SELLER ORDER MANAGEMENT PAGE
    // ============================================
    Component {
        id: sellerOrderManagementPage
        
        SellerOrderManagementPage {
            onBackClicked: {
                stackView.pop()
            }
            
            onConfirmOrder: {
                console.log("[Main] Confirm order:", orderId, productId)
            }
            
            onShipOrder: {
                console.log("[Main] Ship order:", orderId, productId)
            }
            
            onDeliverOrder: {
                console.log("[Main] Deliver order:", orderId, productId)
            }
            
            onCancelOrder: {
                console.log("[Main] Cancel order:", orderId, productId)
            }
        }
    }
}
