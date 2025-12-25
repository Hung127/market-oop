#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <filesystem>

#include "include/BUS/BuyerBUS.h"
#include "include/BUS/ProductBUS.h"
#include "include/BUS/SellerBUS.h"
#include "include/BUS/UserBUS.h"
#include "include/DAO/ProductDAO.h"
#include "include/DTO/BuyerDTO.h"
#include "include/DTO/ProductDTO.h"
#include "include/DTO/PurchaseHistoryDTO.h"
#include "include/DTO/SellerDTO.h"
#include "include/DTO/VoucherDTO.h"
#include "include/UserFactory.h"
#include "include/Utils/Utils.h"

namespace fs = std::filesystem;

int main() {
    using std::cout;
    using std::endl;

    cout << "=== MARKET APP SMOKE TEST ===" << endl;

    // 1) Thiết lập Seller và Sản phẩm mẫu
    auto seller = std::make_shared<SellerDTO>("s1", "Alice", "alice@example.com", "alicepwd");
    auto sellerBusPack = SellerBUS::create(seller);
    
    // Kiểm tra an toàn trước khi lấy value()
    if (!sellerBusPack) return 1; 
    SellerBUS sellerBus = sellerBusPack.value();

    auto productPack = sellerBus.createProduct("p1", "Widget", 100.0, 100);
    if (productPack) ProductDAO::insert(productPack.value());
    std::shared_ptr<ProductDTO> prod = productPack.value();

    auto product2Pack = sellerBus.createProduct("p2", "Gadget", 50.0, 100);
    if (product2Pack) ProductDAO::insert(product2Pack.value());
    std::shared_ptr<ProductDTO> prod2 = product2Pack.value();

    // 2) Thiết lập Voucher
    SellerVoucherDTO aliceVoucher("v1", "ALICE_SALE", "s1", 10.0, 150.0);

    // ---------------------------------------------------------
    // SCENARIO 1: BOB
    // ---------------------------------------------------------
    cout << "\n--- Scenario 1: Bob (Standard Checkout) ---" << endl;
    auto bobPack = UserBUS::registerUser(UserRole::BUYER, "Bob", "bob@example.com", "bobpwd", 500.0);
    
    if (bobPack) {
        auto bob = std::dynamic_pointer_cast<BuyerDTO>(bobPack.value());

        BuyerBUS::addToCart(*bob, prod, 1); 
        
        cout << "[CHECK] Giỏ hàng của Bob trước khi mua:" << endl;
        BuyerBUS::viewCart(*bob);
         
        // Gọi checkout để xem hóa đơn
        BuyerBUS::checkout(*bob, {"p1"}, {}, true); 
        
        // Thực hiện thanh toán thực sự
        auto res1 = BuyerBUS::finalOrder(true, *bob, {"p1"}, {}, true); 

        if (res1.has_value()) {
            cout << "[SUCCESS] Bob thanh toán thành công!" << endl;
            cout << "-> Số dư mới: " << bob->getBalance() << endl;
            cout << "[CHECK] Giỏ hàng của Bob sau khi mua (phải trống):" << endl;
            BuyerBUS::viewCart(*bob);
        }
    }

    // ---------------------------------------------------------
    // SCENARIO 2: DAVID
    // ---------------------------------------------------------
    cout << "\n--- Scenario 2: David (Voucher + Coins + Partial Checkout) ---" << endl;
    auto davidPack = UserBUS::registerUser(UserRole::BUYER, "David", "david@example.com", "dpwdfgh", 1000.0);
    
    if (davidPack) {
        auto david = std::dynamic_pointer_cast<BuyerDTO>(davidPack.value());

        david->setCoins(50.0);
        BuyerBUS::addToCart(*david, prod, 3);  
        BuyerBUS::addToCart(*david, prod2, 1); 

        cout << "[CHECK] Giỏ hàng của David (có 2 loại sản phẩm):" << endl;
        BuyerBUS::viewCart(*david);

        cout << "[INFO] David chọn thanh toán Widget, dùng Voucher và 50 Xu..." << endl;
        std::vector<VoucherDTO*> vouchersForDavid = { &aliceVoucher };
        
        // Xem hóa đơn tạm tính
        BuyerBUS::checkout(*david, {"p1"}, vouchersForDavid, false);

        // Chốt đơn hàng
        auto res2 = BuyerBUS::finalOrder(true, *david, {"p1"}, vouchersForDavid, false);

        if (res2.has_value()) {
            cout << "[SUCCESS] David thanh toán thành công!" << endl;
            cout << "-> Số dư mới: " << david->getBalance() << " (Kỳ vọng: 780.0)" << endl;
            cout << "-> Xu mới tích lũy: " << david->getCoins() << endl;
            
            cout << "[CHECK] Giỏ hàng của David sau khi mua (phải còn Gadget p2):" << endl;
            BuyerBUS::viewCart(*david);
        }
    }

    // ---------------------------------------------------------
    // PHẦN 2: TEST QUY TRÌNH ẢNH NHỊ PHÂN
    // ---------------------------------------------------------
    /*cout << "\n===== TEST QUY TRINH ANH NHI PHAN =====" << endl;

    std::string path1 = std::string(PROJECT_ROOT_DIR) + "/assets/cpu.jpg";
    std::string path2 = std::string(PROJECT_ROOT_DIR) + "/assets/ram.jpg";
    std::string databasePath = std::string(PROJECT_ROOT_DIR) + "/data/database.bin";

    if (!fs::exists(!fs::exists(path1) && !fs::exists(path2) ? path1 : path2)) {
        cout << "[LOI] Khong tim thay file anh nao trong thu muc assets!" << endl;
    } else {
        ProductBUS pBus;
        std::vector<std::string> paths = {path1, path2};
        pBus.processAndSaveProduct("Danh sach linh kien ", paths);
        if (fs::exists(databasePath)) {
            cout << "-> XAC NHAN: Da luu database nhi phan, kich thuoc: " << fs::file_size(databasePath) << " bytes" << endl;
        }

        ProductDAO dao;
        ProductExtraInfoDTO loadedDto;
        dao.loadFromFile(databasePath, loadedDto);

        cout << "Mo ta san pham load tu file: " << loadedDto.getDescription() << endl;
        if (loadedDto.getImageCount() > 0) {
            cout << "=> KET LUAN: Anh da duoc bao toan trong file .bin!" << endl;
        }
    }

    cout << "========================================" << endl;*/
    return 0;
}