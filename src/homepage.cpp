#include "homepage.h"

#include <QGridLayout>
#include <QMessageBox>

HomePage::HomePage(QWidget* parent) : QWidget(parent) {
    setupUI();
}

HomePage::~HomePage() {}

void HomePage::setupUI() {
    // Main layout
    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // Create sections
    createHeader();
    createSearchBar();
    createProductGrid();

    // Add mock products for testing
    addMockProduct("Laptop", 999.99, 5);
    addMockProduct("Mouse", 25.50, 20);
    addMockProduct("Keyboard", 75.00, 15);
    addMockProduct("Monitor", 299.99, 8);
    addMockProduct("Headphones", 150.00, 12);
    addMockProduct("Webcam", 89.99, 10);

    setLayout(mainLayout);
}

void HomePage::createHeader() {
    // Header layout
    headerLayout = new QHBoxLayout();

    // Welcome label
    welcomeLabel = new QLabel("Welcome, John Doe", this);
    QFont font = welcomeLabel->font();
    font.setPointSize(14);
    font.setBold(true);
    welcomeLabel->setFont(font);

    // Balance label
    balanceLabel = new QLabel("Balance: $1,500.00", this);
    QFont balanceFont = balanceLabel->font();
    balanceFont.setPointSize(12);
    balanceLabel->setFont(balanceFont);
    balanceLabel->setStyleSheet("color: green;");

    // Cart button
    cartButton = new QPushButton("🛒 Cart (3)", this);
    cartButton->setMinimumWidth(100);
    cartButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #4CAF50;"
        "   color: white;"
        "   padding: 8px 16px;"
        "   border: none;"
        "   border-radius: 4px;"
        "   font-size: 12px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #45a049;"
        "}");
    connect(cartButton, &QPushButton::clicked, this, &HomePage::onCartClicked);

    // Logout button
    logoutButton = new QPushButton("Logout", this);
    logoutButton->setMinimumWidth(80);
    logoutButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #f44336;"
        "   color: white;"
        "   padding: 8px 16px;"
        "   border: none;"
        "   border-radius: 4px;"
        "   font-size:  12px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #da190b;"
        "}");
    connect(logoutButton, &QPushButton::clicked, this, &HomePage::onLogoutClicked);

    // Add widgets to header
    headerLayout->addWidget(welcomeLabel);
    headerLayout->addWidget(balanceLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(cartButton);
    headerLayout->addWidget(logoutButton);

    mainLayout->addLayout(headerLayout);

    // Separator line
    QFrame* line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    mainLayout->addWidget(line);
}

void HomePage::createSearchBar() {
    searchLayout = new QHBoxLayout();

    // Search input
    searchInput = new QLineEdit(this);
    searchInput->setPlaceholderText("Search products...");
    searchInput->setMinimumHeight(35);
    searchInput->setStyleSheet(
        "QLineEdit {"
        "   padding: 8px;"
        "   border: 2px solid #ddd;"
        "   border-radius: 4px;"
        "   font-size: 13px;"
        "}"
        "QLineEdit:focus {"
        "   border:  2px solid #4CAF50;"
        "}");

    // Search button
    searchButton = new QPushButton("Search", this);
    searchButton->setMinimumWidth(100);
    searchButton->setMinimumHeight(35);
    searchButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #2196F3;"
        "   color: white;"
        "   padding: 8px 16px;"
        "   border: none;"
        "   border-radius: 4px;"
        "   font-size: 13px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #0b7dda;"
        "}");
    connect(searchButton, &QPushButton::clicked, this, &HomePage::onSearchClicked);

    searchLayout->addWidget(searchInput);
    searchLayout->addWidget(searchButton);

    mainLayout->addLayout(searchLayout);
}

void HomePage::createProductGrid() {
    // Scroll area for products
    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("QScrollArea { border: none; }");

    // Product container
    productContainer = new QWidget();
    productGridLayout = new QVBoxLayout(productContainer);
    productGridLayout->setSpacing(15);
    productGridLayout->setContentsMargins(0, 10, 0, 10);

    scrollArea->setWidget(productContainer);
    mainLayout->addWidget(scrollArea);
}

void HomePage::addMockProduct(const QString& name, double price, int stock) {
    // Create product card
    QFrame* productCard = new QFrame(productContainer);
    productCard->setFrameShape(QFrame::Box);
    productCard->setStyleSheet(
        "QFrame {"
        "   background-color: white;"
        "   border: 1px solid #ddd;"
        "   border-radius: 8px;"
        "   padding: 15px;"
        "}"
        "QFrame: hover {"
        "   border: 1px solid #4CAF50;"
        "   box-shadow: 0 2px 5px rgba(0,0,0,0.1);"
        "}");

    QHBoxLayout* cardLayout = new QHBoxLayout(productCard);

    // Product info (left side)
    QVBoxLayout* infoLayout = new QVBoxLayout();

    QLabel* nameLabel = new QLabel(name, productCard);
    QFont nameFont = nameLabel->font();
    nameFont.setPointSize(13);
    nameFont.setBold(true);
    nameLabel->setFont(nameFont);

    QLabel* priceLabel = new QLabel(QString("$%1").arg(price, 0, 'f', 2), productCard);
    QFont priceFont = priceLabel->font();
    priceFont.setPointSize(12);
    priceLabel->setFont(priceFont);
    priceLabel->setStyleSheet("color: #4CAF50;");

    QLabel* stockLabel = new QLabel(QString("In Stock: %1").arg(stock), productCard);
    stockLabel->setStyleSheet("color:  #666;");

    QLabel* sellerLabel = new QLabel("Seller: TechStore", productCard);
    sellerLabel->setStyleSheet("color:  #888; font-size: 11px;");

    infoLayout->addWidget(nameLabel);
    infoLayout->addWidget(priceLabel);
    infoLayout->addWidget(stockLabel);
    infoLayout->addWidget(sellerLabel);
    infoLayout->addStretch();

    // Add to cart button (right side)
    QPushButton* addToCartBtn = new QPushButton("Add to Cart", productCard);
    addToCartBtn->setMinimumWidth(120);
    addToCartBtn->setMinimumHeight(40);
    addToCartBtn->setStyleSheet(
        "QPushButton {"
        "   background-color:  #4CAF50;"
        "   color: white;"
        "   padding: 10px 20px;"
        "   border: none;"
        "   border-radius: 5px;"
        "   font-size: 12px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background-color: #45a049;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #3d8b40;"
        "}");
    connect(addToCartBtn, &QPushButton::clicked, this, &HomePage::onProductClicked);

    // Add to card layout
    cardLayout->addLayout(infoLayout, 1);
    cardLayout->addWidget(addToCartBtn);

    // Add card to grid
    productGridLayout->addWidget(productCard);
}

// Slot implementations
void HomePage::onSearchClicked() {
    QString searchText = searchInput->text();
    QMessageBox::information(
        this, "Search",
        QString("Searching for: %1").arg(searchText.isEmpty() ? "(empty)" : searchText));
}

void HomePage::onCartClicked() {
    QMessageBox::information(this, "Cart", "Opening cart page.. .");
}

void HomePage::onLogoutClicked() {
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Logout", "Are you sure you want to logout?", QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        QMessageBox::information(this, "Logout", "Logged out successfully!");
    }
}

void HomePage::onProductClicked() {
    QPushButton* btn = qobject_cast<QPushButton*>(sender());
    if (btn) {
        QMessageBox::information(this, "Add to Cart", "Product added to cart!");

        // Update cart count (mock)
        cartButton->setText("🛒 Cart (4)");
    }
}
