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
    SellerBUS sellerBus = sellerBusPack.value();

    auto productPack = sellerBus.createProduct("p1", "Widget", 100.0, 100);
    std::shared_ptr<ProductDTO> prod = productPack.value();
    ProductDAO::insert(prod);

    // Tạo thêm sản phẩm thứ 2 để test việc "thanh toán một phần giỏ hàng"
    auto product2Pack = sellerBus.createProduct("p2", "Gadget", 50.0, 100);
    std::shared_ptr<ProductDTO> prod2 = product2Pack.value();
    ProductDAO::insert(prod2);

    // 2) Thiết lập Voucher Shop của Alice: Giảm 10%, Min đơn 150.0
    SellerVoucherDTO aliceVoucher("v1", "ALICE_SALE", "s1", 10.0, 150.0);

    // ---------------------------------------------------------
    // SCENARIO 1: BOB (Thanh toán bình thường - Không Voucher/Xu)
    // ---------------------------------------------------------
    cout << "\n--- Scenario 1: Bob (Standard Checkout) ---" << endl;
    auto bobPack = UserBUS::registerUser(UserRole::BUYER, "Bob", "bob@example.com", "bobpwd", 500.0);
    auto bob = std::dynamic_pointer_cast<BuyerDTO>(bobPack.value());

    BuyerBUS::addToCart(*bob, prod, 1); // 100.0
    
    cout << "[CHECK] Giỏ hàng của Bob trước khi mua:" << endl;
    BuyerBUS::viewCart(*bob);

    // Thanh toán: Không voucher, không xu
    auto res1 = BuyerBUS::checkout(*bob, {"p1"}, {}, false); 
    if (res1.has_value()) {
        cout << "[SUCCESS] Bob thanh toán thành công!" << endl;
        cout << "-> Số dư cũ: 500.0 | Thực trả: 100.0 | Số dư mới: " << bob->getBalance() << endl;
        cout << "[CHECK] Giỏ hàng của Bob sau khi mua (phải trống):" << endl;
        BuyerBUS::viewCart(*bob);
    }

    // ---------------------------------------------------------
    // SCENARIO 2: DAVID (Thanh toán một phần + Voucher + Xu)
    // ---------------------------------------------------------
    cout << "\n--- Scenario 2: David (Voucher + Coins + Partial Checkout) ---" << endl;
    auto davidPack = UserBUS::registerUser(UserRole::BUYER, "David", "david@example.com", "dpwdfgh", 1000.0);
    auto david = std::dynamic_pointer_cast<BuyerDTO>(davidPack.value());

    // Giả lập David có sẵn 50 xu và bỏ 2 loại hàng vào giỏ
    david->setCoins(50.0);
    BuyerBUS::addToCart(*david, prod, 3);  // 3 x 100 = 300.0 (Widget)
    BuyerBUS::addToCart(*david, prod2, 1); // 1 x 50 = 50.0   (Gadget)

    cout << "[CHECK] Giỏ hàng của David (có 2 loại sản phẩm):" << endl;
    BuyerBUS::viewCart(*david);

    // David chỉ muốn thanh toán Widget (p1), chừa Gadget (p2) lại giỏ
    cout << "[INFO] David chọn thanh toán Widget, dùng Voucher và 50 Xu..." << endl;
    std::vector<VoucherDTO*> vouchersForDavid = { &aliceVoucher };
    
    auto res2 = BuyerBUS::checkout(*david, {"p1"}, vouchersForDavid, true);

    if (res2.has_value()) {
        cout << "[SUCCESS] David thanh toán thành công!" << endl;
        // Tính toán kiểm tra: 
        // 300 (Widget) - 10% Voucher (30) = 270.
        // 270 - 50 Xu = 220 thực trả. 
        // Balance mới: 1000 - 220 = 780.
        cout << "-> Số dư mới: " << david->getBalance() << " (Kỳ vọng: 780.0)" << endl;
        cout << "-> Xu mới tích lũy: " << david->getCoins() << " (Kỳ vọng: 0.22 xu)" << endl;
        
        cout << "[CHECK] Giỏ hàng của David sau khi mua (phải còn Gadget p2):" << endl;
        BuyerBUS::viewCart(*david);
    }

    // ---------------------------------------------------------
    // PHẦN 2: TEST QUY TRÌNH ẢNH NHỊ PHÂN (GIỮ NGUYÊN)
    // ---------------------------------------------------------
    
    cout << "\n===== TEST QUY TRINH ANH NHI PHAN =====" << endl;

 std::string testImagePath = std::string(PROJECT_ROOT_DIR) + "/assets/cpu.jpg";
    
    // SỬA DÒNG NÀY: Để file database luôn nằm trong folder data ở gốc dự án
    std::string databasePath = std::string(PROJECT_ROOT_DIR) + "/data/database.bin";

    if (!fs::exists(testImagePath)) {
        cout << "[LOI] Khong tim thay file " << testImagePath << " o thu muc goc!" << endl;
    } else {
        ProductBUS pBus;
        pBus.processAndSaveProduct("San pham CPU Chinh Hang", {testImagePath});

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

    cout << "========================================" << endl;
    return 0;
}