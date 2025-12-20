#include <iostream>
#include <memory>
#include <string>

#include "include/BUS/BuyerBUS.h"
#include "include/BUS/SellerBUS.h"
#include "include/DAO/ProductDAO.h"
#include "include/DTO/BuyerDTO.h"
#include "include/DTO/ProductDTO.h"
#include "include/DTO/PurchaseHistoryDTO.h"
#include "include/DTO/SellerDTO.h"
#include "include/UserFactory.h"

int main() {
    using std::cout;
    using std::endl;

    cout << "=== MARKET APP SMOKE TEST ===" << endl;
    // 1) Tạo seller và SellerBUS
    auto seller = std::make_shared<SellerDTO>("s1", "Alice", "alice@example.com", "alicepwd");
    auto sellerBusPack = SellerBUS::create(seller);
    if (!sellerBusPack.has_value()) {
        cout << "[ERROR] Cannot create SellerBUS" << endl;
        return 1;
    }
    SellerBUS sellerBus = sellerBusPack.value();

    // 2) Seller tạo 1 product
    auto productPack = sellerBus.createProduct("p1", "Widget", 10.0, 5);
    if (!productPack.has_value()) {
        cout << "[ERROR] createProduct failed" << endl;
        return 1;
    }
    std::shared_ptr<ProductDTO> prod = productPack.value();
    cout << "[INFO] Product created: " << prod->getName() << " (ID=" << prod->getID()
         << ") price=" << prod->getPrice() << " stock=" << prod->getStock() << endl;

    // 3) Lưu product vào DAO (mock)
    if (!ProductDAO::insert(*prod)) {
        cout << "[ERROR] ProductDAO::insert failed (maybe duplicate id)" << endl;
        return 1;
    }
    cout << "[INFO] Product inserted into ProductDAO mock DB" << endl;
    //
    // 4) Tạo buyer qua BuyerBUS
    auto buyerPack =
        UserBUS::registerUser(UserRole::BUYER, "Bob", "bob@example.com", "bobpwd", 100.0);
    if (!buyerPack.has_value()) {
        cout << "[ERROR] BuyerBUS::create failed: " << buyerPack.error() << endl;
        return 1;
    }
    std::shared_ptr<BuyerDTO> buyer = std::dynamic_pointer_cast<BuyerDTO>(buyerPack.value());
    cout << "[INFO] Buyer created: " << buyer->getName() << " Balance=" << buyer->getBalance()
         << endl;
    //
    // 5) Lấy product từ DAO (đảm bảo lấy đúng shared_ptr)
    auto prodPack = ProductDAO::getProductById("p1");
    if (!prodPack.has_value()) {
        cout << "[ERROR] ProductDAO::getProductById failed: " << prodPack.error() << endl;
        return 1;
    }
    std::shared_ptr<ProductDTO> prodFromDao = prodPack.value();

    // 6) Buyer addToCart 2 cái product
    auto addRes = BuyerBUS::addToCart(*buyer, prodFromDao, 2);
    if (!addRes.has_value()) {
        cout << "[ERROR] addToCart failed: " << addRes.error() << endl;
        return 1;
    }
    cout << "[INFO] Added 2 items to cart" << endl;

    // 7) View cart (prints)
    cout << "---- Cart before checkout ----" << endl;
    BuyerBUS::viewCart(*buyer);

    // 8) Checkout
    auto ck = BuyerBUS::checkout(*buyer);
    if (!ck.has_value()) {
        cout << "[ERROR] Checkout failed: " << ck.error() << endl;
        return 1;
    }
    cout << "[INFO] Checkout SUCCESS" << endl;

    // 9) Post-checkout info: buyer balance, product stock, purchase history
    cout << "Buyer balance after checkout: " << buyer->getBalance() << endl;
    auto prodAfterPack = ProductDAO::getProductById("p1");
    if (prodAfterPack.has_value()) {
        cout << "Product stock after checkout: " << prodAfterPack.value()->getStock() << endl;
    }
    cout << "Purchase history:" << endl;
    buyer->getPurchasesHistory().printHistory();

    cout << "=== SMOKE TEST END ===" << endl;
    return 0;
}
