# Quy Ước Lập Trình C++

## 🎯 Tóm Tắt - Tham Khảo Nhanh

>[!NOTE]
>Tuân theo [C++ core guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines)
>Một số cách đặt tên khác

```cpp
// Class: PascalCase
class StudentManager { };

// Hàm: camelCase
void calculateTotal() { }

// Thuộc tính private: _camelCase (BẮT BUỘC dùng this->)
int _studentId;
this->_studentId = 10;  // ✅ ĐÚNG

// Thuộc tính public: camelCase (tránh nếu có thể)
int age;

// Biến: camelCase
int totalScore = 0;

// Hằng số: UPPER_CASE
const int MAX_SIZE = 100;
```

---

## 📋 Quy Tắc Đặt Tên

| Loại | Định Dạng | Ví Dụ |
|------|-----------|-------|
| Class/Struct | `PascalCase` | `Student`, `CourseManager` |
| Hàm/Method | `camelCase` | `getName()`, `calculateGpa()` |
| Thuộc tính private | `_camelCase` | `_studentId`, `_name` |
| Thuộc tính protected | `_camelCase` | `_baseValue` |
| Thuộc tính public | `camelCase` | `age` (tránh!) |
| Biến cục bộ | `camelCase` | `totalScore`, `userName` |
| Tham số | `camelCase` | `studentId`, `courseName` |
| Hằng số | `UPPER_CASE` | `MAX_STUDENTS`, `PI` |
| Kiểu enum | `PascalCase` | `Status`, `Color` |
| Giá trị enum | `PascalCase` | `Status::Active` |
| Namespace | `lower_case` | `student_system` |

---

## 🔴 QUY TẮC BẮT BUỘC: Sử Dụng `this->`

### **LUÔN dùng `this->` khi truy cập private/protected members**

```cpp
class Student {
   private:
    int _studentId;
    std::string _name;

   public:
    // ✅ ĐÚNG: Dùng this->
    void setId(int studentId) {
        this->_studentId = studentId;  // Rõ ràng, phân biệt với parameter
    }

    int getId() const {
        return this->_studentId;  // Rõ ràng đây là member variable
    }

    // ❌ SAI: Không dùng this->
    void setName(const std::string& name) {
        _name = name;  // KHÔNG ĐƯỢC PHÉP!
    }
};
```

### **Lý do:**

1. ✅ **Phân biệt rõ ràng** giữa member variable và local variable
2. ✅ **Tránh nhầm lẫn** khi parameter trùng tên
3. ✅ **Dễ đọc code** - biết ngay đây là thuộc tính của object
4. ✅ **Consistency** - toàn bộ code đồng nhất
5. ✅ **Self-documenting** - code tự giải thích

---

## 📁 Cấu Trúc Thư Mục

```
project/
├── include/          # File header (.h)
├── src/              # File source (.cpp)
├── tests/            # Unit tests
├── .clang-format     # Config tự động format
├── .clang-tidy       # Config kiểm tra lỗi
└── .vscode/          # Cài đặt VS Code
```

**Đặt tên file:** `StudentManager.h`, `StudentManager.cpp`

---

## ✅ Ví Dụ Class Đầy Đủ

```cpp
// Student.h
#ifndef STUDENT_H
#define STUDENT_H

#include <string>
#include <vector>

class Student {
   private:
    int _studentId;          // Private: dùng tiền tố _
    std::string _name;
    double _gpa;
    std::vector<int> _courseIds;

   public:
    // Constructor với initializer list
    Student(int id, const std::string& name)
        : _studentId(id), _name(name), _gpa(0.0) {
        // Nếu cần logic thêm trong body, dùng this->
    }

    // Getter - BẮT BUỘC dùng this->
    int getId() const {
        return this->_studentId;
    }

    std::string getName() const {
        return this->_name;
    }

    double getGpa() const {
        return this->_gpa;
    }

    // Setter - BẮT BUỘC dùng this->
    void setId(int studentId) {
        if (studentId < 0) {
            throw std::invalid_argument("ID phải >= 0");
        }
        this->_studentId = studentId;
    }

    void setName(const std::string& name) {
        if (name.empty()) {
            throw std::invalid_argument("Tên không được rỗng");
        }
        this->_name = name;
    }

    void setGpa(double gpa) {
        if (gpa < 0.0 || gpa > 4.0) {
            throw std::invalid_argument("GPA phải trong [0, 4]");
        }
        this->_gpa = gpa;
    }

    // Method phức tạp
    void addCourse(int courseId) {
        // BẮT BUỘC dùng this-> cho member variable
        this->_courseIds.push_back(courseId);
    }

    void calculateFinalGpa(const std::vector<double>& scores) {
        double total = 0.0;
        for (const auto& score : scores) {
            total += score;
        }
        // BẮT BUỘC dùng this->
        this->_gpa = total / scores.size();
    }

    // Display
    void display() const {
        std::cout << "ID: " << this->_studentId << "\n"
                  << "Name: " << this->_name << "\n"
                  << "GPA: " << this->_gpa << std::endl;
    }
};

#endif  // STUDENT_H
```

---

## 🔧 Quy Tắc Code Style

### 1. Thụt Lề (Indentation)
- **4 dấu cách** (không dùng tab)
- Thụt lề nhất quán

### 2. Dấu Ngoặc Nhọn (Braces)
- **Cùng dòng** với khai báo
- **Luôn dùng ngoặc nhọn** (kể cả 1 dòng)

```cpp
// ✅ ĐÚNG
if (condition) {
    doSomething();
}

// ❌ SAI
if (condition)
    doSomething();  // Thiếu ngoặc nhọn
```

### 3. Độ Dài Dòng
- **Tối đa 100 ký tự**

### 4. Khoảng Trắng (Spacing)

```cpp
// ✅ ĐÚNG
int result = a + b;
function(arg1, arg2);
if (condition) { }

// ❌ SAI
int result=a+b;
function(arg1,arg2);
if(condition) { }
```

### 5. Con Trỏ/Tham Chiếu

```cpp
// ✅ ĐÚNG (dấu * hoặc & dính với kiểu)
int* ptr;
const std::string& name;

// ❌ SAI
int *ptr;
int* p, q;  // Gây nhầm lẫn!
```

---

## 💡 Thực Hành Tốt (Best Practices)

### ✅ NÊN LÀM

```cpp
// Dùng initializer list
Student::Student(int id) : _studentId(id) {}

// BẮT BUỘC dùng this-> trong method body
void Student::updateInfo(int id, const std::string& name) {
    this->_studentId = id;      // ✅ ĐÚNG
    this->_name = name;          // ✅ ĐÚNG
}

// Dùng const cho method chỉ đọc
int getId() const {
    return this->_studentId;     // ✅ ĐÚNG
}

// Dùng nullptr (không dùng NULL hoặc 0)
int* ptr = nullptr;

// Dùng range-based for loop
for (const auto& student : students) { }

// Dùng smart pointer
std::unique_ptr<Student> ptr = std::make_unique<Student>(1, "John");

// Kiểm tra đầu vào
if (studentId < 0) {
    throw std::invalid_argument("ID không hợp lệ");
}
```

### ❌ KHÔNG NÊN

```cpp
// KHÔNG: Gán giá trị trong constructor body mà không dùng this->
Student::Student(int id) {
    _studentId = id;  // SAI! Phải: this->_studentId = id;
}

// KHÔNG: Truy cập member không dùng this->
int Student::getId() const {
    return _studentId;  // SAI! Phải: return this->_studentId;
}

// KHÔNG: Quên const
int getId() { return this->_studentId; }  // Thiếu const!

// KHÔNG: Dùng con trỏ thô
Student* ptr = new Student();  // Nguy cơ memory leak!

// KHÔNG: Số ma thuật (magic number)
if (score > 85) { }  // 85 là gì?
```

---

## 🎯 Ví Dụ So Sánh: Với và Không Có `this->`

### ❌ KHÔNG DÙNG `this->` (SAI - KHÔNG ĐƯỢC PHÉP)

```cpp
class Student {
   private:
    int _id;
    std::string _name;

   public:
    void setId(int id) {
        _id = id;  // ❌ Không rõ ràng
    }

    void setInfo(int id, std::string name) {
        _id = id;        // ❌ Nhầm lẫn với parameter
        _name = name;    // ❌ Nhầm lẫn với parameter
    }

    void display() const {
        std::cout << _id << " " << _name;  // ❌ Khó đọc
    }
};
```

### ✅ DÙNG `this->` (ĐÚNG - BẮT BUỘC)

```cpp
class Student {
   private:
    int _id;
    std::string _name;

   public:
    void setId(int id) {
        this->_id = id;  // ✅ Rõ ràng: member = parameter
    }

    void setInfo(int id, std::string name) {
        this->_id = id;        // ✅ Member variable
        this->_name = name;    // ✅ Member variable
    }

    void display() const {
        std::cout << this->_id << " " << this->_name;  // ✅ Dễ đọc
    }
};
```

---

## 📝 Comment và Tài Liệu

### Comment cho Class

```cpp
/**
 * @brief Quản lý danh sách sinh viên
 * 
 * Cung cấp chức năng thêm, xóa và tìm kiếm sinh viên.
 */
class StudentManager {
```

### Comment cho Hàm

```cpp
/**
 * @brief Tính điểm GPA trung bình
 * @param students Danh sách sinh viên
 * @return Giá trị GPA trung bình
 */
double calculateAverageGpa(const std::vector<Student>& students);
```

### Comment cho Logic Phức Tạp

```cpp
// Tính điểm trung bình có trọng số dựa trên số tín chỉ
double weightedAverage = totalScore / totalCredits;
```

---

## 🚀 Quy Trình Làm Việc

### Trước khi commit:

1. **Format code:** `Ctrl+Shift+F` (hoặc lưu với auto-format)
2. **Kiểm tra lỗi:** `Ctrl+Shift+M`
3. **Kiểm tra đã dùng `this->` cho tất cả member variables**
4. **Sửa tất cả lỗi** (🔴 đỏ)
5. **Xem lại warnings** (🟡 vàng)
6. **Test code của bạn**
7. **Commit**

### Các lệnh thường dùng:

```bash
# Format tất cả file
./format.sh

# Build project
mkdir build && cd build
cmake .. && make

# Chạy tests
./tests/run_tests
```

---

## 🔍 Lỗi Thường Gặp

| ❌ Sai | ✅ Đúng | Giải Thích |
|---------|----------|-----------|
| `_studentId = 10;` | `this->_studentId = 10;` | Thiếu this-> |
| `return _name;` | `return this->_name;` | Thiếu this-> |
| `int* p; *p = 5;` | `int* p = nullptr;` | Con trỏ chưa khởi tạo |
| `class student { }` | `class Student { }` | Class phải PascalCase |
| `void GetName()` | `void getName()` | Hàm phải camelCase |
| `int _publicVar;` (public) | `int publicVar;` | Public không dùng _ |
| `if (x) doThis();` | `if (x) { doThis(); }` | Luôn dùng ngoặc nhọn |
| `Student* s = new Student();` | `auto s = make_unique<Student>();` | Dùng smart pointer |

---

## 🛠️ Công Cụ

### Tự động format (clang-format)
- File: `.clang-format` ở thư mục gốc
- VS Code: Tự động format khi save
- Thủ công: `Ctrl+Shift+F`

### Kiểm tra lỗi (clang-tidy)
- File: `.clang-tidy` ở thư mục gốc
- VS Code: Hiện cảnh báo tự động
- Thủ công: `clang-tidy file.cpp -- -std=c++23`

### Cài đặt VS Code
- File: `.vscode/settings.json`
- Đã cấu hình sẵn cho nhóm
- Không cần setup gì thêm!

---

## 📚 Tài Liệu Tham Khảo

- **C++ Core Guidelines:** https://isocpp.github.io/CppCoreGuidelines/
- **Google C++ Style Guide:** https://google.github.io/styleguide/cppguide.html
- **clang-format:** https://clang.llvm.org/docs/ClangFormat.html
- **clang-tidy:** https://clang.llvm.org/extra/clang-tidy/

---

## 🤝 Đóng Góp

### Cập nhật quy ước:

1. Thảo luận trong nhóm hoặc tạo issue
2. Cập nhật document này
3. Cập nhật `.clang-format` hoặc `.clang-tidy` nếu cần
4. Thông báo cho team
5. Commit thay đổi

### Có câu hỏi?

- Mở issue với label `coding-style`
- Tag @Hung127
- Thảo luận trong group chat

---

## ✅ Checklist Cho Thành Viên Mới

Trước commit đầu tiên, kiểm tra:

- [ ] Đã đọc document này
- [ ] Đã cài extension C/C++ trong VS Code
- [ ] Đã clone repo với `.clang-format` và `.vscode/settings.json`
- [ ] Auto-format hoạt động (`Ctrl+S` tự format code)
- [ ] Thấy warnings trong Problems panel (`Ctrl+Shift+M`)
- [ ] Hiểu quy tắc đặt tên
- [ ] Hiểu cấu trúc class (thuộc tính private dùng `_prefix`)
- [ ] **Hiểu và tuân thủ quy tắc BẮT BUỘC dùng `this->` cho member variables**

---

## 🔴 Quy Tắc Vàng

### **3 ĐIỀU BẮT BUỘC:**

1. ✅ **Private/Protected members:** `_camelCase`
2. ✅ **LUÔN dùng `this->` khi truy cập member variables**
3. ✅ **Initializer list trong constructor**

### **Ví dụ chuẩn:**

```cpp
class Example {
   private:
    int _value;
    std::string _name;

   public:
    Example(int value, const std::string& name)
        : _value(value), _name(name) {}  // ✅ Initializer list

    void setValue(int value) {
        this->_value = value;  // ✅ Dùng this->
    }

    int getValue() const {
        return this->_value;  // ✅ Dùng this->
    }
};
```

---

**Ghi nhớ:** 
- **Nhất quán** là quan trọng nhất! 
- **LUÔN dùng `this->`** cho member variables!
- Khi không chắc, xem code có sẵn hoặc hỏi nhóm.

**Auto-format sẽ xử lý phần lớn formatting** - bạn chỉ cần tập trung vào đặt tên, cấu trúc và **dùng `this->`**! 🎯
