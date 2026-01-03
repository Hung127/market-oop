#include <QApplication>

#include "homepage.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Set application-wide style
    app.setStyle("Fusion");

    // Create and show homepage
    HomePage homePage;
    homePage.setWindowTitle("E-Commerce - Buyer Dashboard");
    homePage.resize(900, 700);
    homePage.show();

    return app.exec();
}
