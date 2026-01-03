#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>

class HomePage : public QWidget {
    Q_OBJECT

   public:
    explicit HomePage(QWidget* parent = nullptr);
    ~HomePage();

   private slots:
    void onSearchClicked();
    void onCartClicked();
    void onLogoutClicked();
    void onProductClicked();

   private:
    void setupUI();
    void createHeader();
    void createSearchBar();
    void createProductGrid();
    void addMockProduct(const QString& name, double price, int stock);

    // Layouts
    QVBoxLayout* mainLayout;
    QHBoxLayout* headerLayout;
    QHBoxLayout* searchLayout;
    QVBoxLayout* contentLayout;

    // Header widgets
    QLabel* welcomeLabel;
    QLabel* balanceLabel;
    QPushButton* cartButton;
    QPushButton* logoutButton;

    // Search widgets
    QLineEdit* searchInput;
    QPushButton* searchButton;

    // Content widgets
    QScrollArea* scrollArea;
    QWidget* productContainer;
    QVBoxLayout* productGridLayout;
};

#endif  // HOMEPAGE_H
