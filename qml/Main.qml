import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick. Window 2.12

ApplicationWindow {
    id: root
    visible: true
    width: 1280
    height: 800
    title: "Market App"

    // Mock user data
    property string currentUser: ""
    property string currentEmail: ""
    property string currentRole: ""
    property bool isLoggedIn: false
    property real currentBalance: 1000.00

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
            onLoginSucceeded: {
                console. log("Main: Login succeeded, role =", role)
                
                if (role === "BUYER") {
                    stackView.push(buyerHomePage)
                } else if (role === "SELLER") {
                    stackView.push(sellerDashboard)
                }
            }
            
            onRegisterClicked: {
                stackView.push(registerPage)
            }
        }
    }

    // ============================================
    // REGISTER PAGE
    // ============================================
    Component {
        id: registerPage
        
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
        id:  buyerHomePage
        
        BuyerHomePage {
            userName: root.currentUser
            
            onLogoutClicked: {
                root.isLoggedIn = false
                root.currentUser = ""
                root.currentEmail = ""
                root.currentRole = ""
                stackView.push(loginPage)
            }
            
            onProductClicked: {
                console.log("Product clicked:", productId)
                stackView.push(productDetailPage, {
                    "productId": productId,
                    "productName": "Laptop Pro 15",
                    "productPrice": 1299.99,
                    "productImage": "💻",
                    "productDescription": "High-performance laptop with 15-inch display, Intel i7 processor, 16GB RAM, 512GB SSD.  Perfect for professionals, developers, and content creators.",
                    "sellerName": "Tech Store",
                    "stockQuantity": 5
                })
            }
            
            onCartClicked: {
                console.log("Cart clicked")
                stackView.push(cartPage)
            }
            
            onProfileClicked: {
                console. log("Profile clicked")
                stackView.push(profilePage, {
                    "userName": root.currentUser,
                    "userEmail": root.currentEmail,
                    "userRole": root.currentRole,
                    "userBalance": root.currentBalance
                })
            }
        }
    }

    // ============================================
    // SELLER DASHBOARD
    // ============================================
    Component {
        id: sellerDashboard
        
        SellerDashboard {
            userName: root.currentUser
            
            onLogoutClicked: {
                root.isLoggedIn = false
                root. currentUser = ""
                root. currentEmail = ""
                root. currentRole = ""
                stackView.push(loginPage)
            }
            
            onAddProductClicked: {
                console.log("Add product clicked")
                stackView.push(addEditProductPage, {
                    "isEditMode": false
                })
            }
            
            onEditProductClicked: {
                console.log("Edit product:", productId)
                stackView. push(addEditProductPage, {
                    "isEditMode":  true,
                    "productId": productId
                })
                
                // Load product data (mock)
                var page = stackView.currentItem
                page.loadProduct(
                    productId,
                    "Laptop Pro 15",
                    "High-performance laptop with 15-inch display",
                    1299.99,
                    5,
                    "Electronics"
                )
            }
            
            onProfileClicked: {
                console. log("Profile clicked")
                stackView.push(profilePage, {
                    "userName": root. currentUser,
                    "userEmail": root.currentEmail,
                    "userRole": root. currentRole,
                    "userBalance": root.currentBalance
                })
            }
        }
    }

    // ============================================
    // CART PAGE
    // ============================================
    Component {
        id: cartPage
        
        CartPage {
            userName: root.currentUser
            
            onBackClicked: {
                stackView.pop()
            }
            
            onCheckoutClicked: {
                console.log("Proceed to checkout")
                // TODO: Create checkout page
                // stackView.push(checkoutPage)
                
                // For now, show success message
                stackView.pop()
                console.log("Order placed successfully!  (mock)")
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
                console.log("Added to cart:", quantity, "items")
                // TODO: Add to cart logic
                stackView.pop()
            }
            
            onBuyNowClicked: {
                console.log("Buy now:", quantity, "items")
                // TODO: Direct checkout
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
            onBackClicked: {
                stackView.pop()
            }
            
            onSaveClicked:  {
                console.log("Product saved:", name, description, price, stock, category)
                
                // Go back after 1.5 seconds
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
            onBackClicked: {
                stackView.pop()
            }
            
            onSaveProfileClicked: {
                console. log("Profile saved:", name, email)
                root.currentUser = name
                root.currentEmail = email
            }
            
            onChangePasswordClicked: {
                console.log("Password changed")
                // TODO: Connect to backend
            }
            
            onLogoutClicked: {
                root. isLoggedIn = false
                root.currentUser = ""
                root.currentEmail = ""
                root.currentRole = ""
                stackView.push(loginPage)
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
                console.log("Order placed!")
                // TODO: Call BuyerBUS:: checkout
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
            userName: root.currentUser
            
            onBackClicked: {
                stackView.pop()
            }
            
            onViewOrderDetails: {
                console.log("View order:", orderId)
                // TODO: Create OrderDetailsPage
            }
        }
    }

    // ============================================
    // SELLER ORDER MANAGEMENT PAGE
    // ============================================
    Component {
        id: sellerOrderManagementPage
        
        SellerOrderManagementPage {
            userName: root.currentUser
            
            onBackClicked: {
                stackView.pop()
            }
            
            onConfirmOrder: {
                console.log("Confirm order:", orderId, productId)
                // TODO: Call SellerBUS::confirmOrderItem
            }
            
            onShipOrder: {
                console.log("Ship order:", orderId, productId)
                // TODO: Call SellerBUS::shipOrderItem
            }
            
            onDeliverOrder: {
                console.log("Deliver order:", orderId, productId)
                // TODO: Call SellerBUS::deliverOrderItem
            }
            
            onCancelOrder: {
                console. log("Cancel order:", orderId, productId)
                // TODO:  Call SellerBUS::cancelOrderItem
            }
        }
    }
}
