#include <iostream>
#include <memory>
#include <string>

#include "include/BUS/BuyerBUS.h"
#include "include/BUS/ProductBUS.h"
#include "include/BUS/SellerBUS.h"
#include "include/DAO/ProductDAO.h"
#include "include/DTO/BuyerDTO.h"
#include "include/DTO/ProductDTO.h"
#include "include/DTO/PurchaseHistoryDTO.h"
#include "include/DTO/SellerDTO.h"
#include "include/UserFactory.h"
#include "include/Utils/Utils.h"

namespace fs = std::filesystem;

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

    cout << "===== TEST QUY TRINH ANH NHI PHAN =====" << endl;

    // 1. Khai báo đường dẫn
    std::string testImagePath = "cpu.jpg";           // Đặt file này ở thư mục gốc Project
    std::string databasePath = "../data/database.bin";  // File này sẽ nằm trong folder data

    // --- KIỂM TRA FILE ẢNH GỐC ---
    if (!fs::exists(testImagePath)) {
        cout << "[LOI] Khong tim thay file " << testImagePath << " o thu muc goc!" << endl;
        cout << "Hay copy 1 tam anh vao folder Project va doi ten thanh test_image.jpg" << endl;
        return 1;
    }

    // 2. GIAI ĐOẠN LƯU: Đọc từ đường dẫn -> Chuyển sang ảnh luôn (Binary) -> Lưu .bin
    cout << "\n[BUOC 1] Dang nap anh va dong goi vao file .bin..." << endl;
    ProductBUS bus;
    std::vector<std::string> paths = {testImagePath};

    // Hàm này sẽ gọi ImageHelper để đọc byte và DAO để ghi file .bin
    bus.processAndSaveProduct("San pham CPU", paths);
   if (fs::exists(databasePath)) {
    cout << "-> XAC NHAN: File da ton tai tai: " << fs::absolute(databasePath) << endl;
    cout << "-> Kich thuoc: " << fs::file_size(databasePath) << " bytes" << endl;
} else {
    cout << "-> [LOI THAT SU] File van chua duoc tao ra!" << endl;
}

    // 3. GIAI ĐOẠN XÓA: Xóa ảnh gốc để chứng minh "đọc sang ảnh luôn"
    /*cout << "\n[BUOC 2] Dang xoa anh goc de test tinh bao toan..." << endl;
    try {
        if (fs::remove(testImagePath)) {
            cout << "-> DA XOA THANH CONG: " << testImagePath << endl;
        }
    } catch (const fs::filesystem_error& e) {
        cout << "[LOI] Khong the xoa file: " << e.what() << endl;
    }*/

    // 4. GIAI ĐOẠN ĐỌC: Sài dữ liệu từ file .bin (Lúc nộp bài chỉ cần bước này)
    cout << "\n[BUOC 3] Dang khoi phuc anh tu file .bin (Load database)..." << endl;
    ProductDAO dao;
    ProductExtraInfoDTO loadedDto;

    // DAO sẽ giải mã file .bin nạp lại vào DTO
    dao.loadFromFile(databasePath, loadedDto);

    // 5. KIỂM TRA KẾT QUẢ
    cout << "\n===== KET QUA KIEM TRA =====" << endl;
    cout << "Mo ta san pham: " << loadedDto.getDescription() << endl;
    cout << "So luong anh trong database: " << loadedDto.getImageCount() << endl;

    if (loadedDto.getImageCount() > 0) {
        size_t imageSize = loadedDto.getImageAt(0).size();
        cout << "Kich thuoc anh thu nhat: " << imageSize << " bytes" << endl;
        cout << "=> KET LUAN: Anh van ton tai trong file .bin du anh goc da bi xoa!" << endl;
        cout << "=> Backend da san sang gui du lieu cho Qt hien thi." << endl;
    } else {
        cout << "=> THAT BAI: Khong tim thay du lieu anh trong file .bin." << endl;
    }

    cout << "========================================" << endl;

    return 0;
}
