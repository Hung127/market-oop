# 📦 Quản lý Mua Bán Sản Phẩm - Dự Án C++

> Dự án C++ mô phỏng hệ thống mua bán sản phẩm giữa người mua (Buyer) và người bán (Seller).

## 🌟 Tính năng chính
- Người bán có thể tạo, chỉnh sửa, xóa, đăng bán sản phẩm.
- Người mua có thể thêm vào giỏ hàng, mua sản phẩm, xem lịch sử mua hàng.
- Quản lý sản phẩm, giỏ hàng, đơn hàng, lịch sử mua.
- Tìm kiếm sản phẩm (theo tên, khoảng giá).


## 🛠️ Cách build & chạy

```bash
# Clone dự án về
git clone <link_repo>
cd <ten_thu_muc_repo>

# Compile (Cmake)
$ mkdir build
$ cd build
$ cmake ..
$ cmake --build .

./app
```

## Cách build test và chạy

```bash
rm -rf build
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build . -- -j$(nproc)

```

## 📂 Các file chính

- `src/` & `include/` : Mã nguồn.
- `Buyer.h/cpp`, `Seller.h/cpp` : Định nghĩa người mua/bán.
- `Product.h/cpp` : Định nghĩa sản phẩm.
- `Cart.h/cpp` : Quản lý giỏ hàng.
- `Market.h/cpp` : Quản lý sàn giao dịch.
- `Order.h` : Đơn hàng.
- `PurchaseHistory.h/cpp` : Lịch sử mua.

## 📝 Lưu ý
- README này chỉ là tạm thời, có thể bổ sung sau khi hoàn thiện dự án.
