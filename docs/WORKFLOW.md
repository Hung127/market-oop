# Quy trình làm việc Git (Basic Git Workflow)

## 0. Thiết lập Git ban đầu (Setup)

**Mỗi thành viên cần đảm bảo cấu hình Git đúng trước khi bắt đầu làm việc nhóm:**

```bash
$ git config --global user.name "Tên của bạn"
$ git config --global user.email "email-sinhvien@domain.com"
```

**Đảm bảo thiết lập end-of-line cho mọi thành viên giống nhau (cho dự án đa nền tảng):**

```bash
$ git config --global core.autocrlf input     # Dùng cho máy Linux/Mac (khuyên dùng nếu repo dùng LF)
# hoặc
$ git config --global core.autocrlf true      # Dùng cho máy Windows (nếu muốn tự chuyển CRLF<->LF)
```

### Kiểm tra lại cấu hình:
```bash
$ git config --list
```

---

## 1. Tạo Issue và tính năng mới

- Khi làm một tính năng hoặc sửa lỗi, mỗi người tự tạo một **issue** mới trên GitHub.
- Gán issue vào Project hoặc Milestone nếu cần thiết.
- Gán mình vào issue để theo dõi trách nhiệm.

## 2. Tạo branch mới từ nhánh `main`

```bash
$ git checkout main
$ git pull origin main      # Đảm bảo main là mới nhất
$ git checkout -b feature/ten-tinh-nang-moi   # Ví dụ: feature/login-page hoặc fix/issue-42
```

**Lưu ý:** Đặt tên branch theo cấu trúc:  
- `feature/ten-tinh-nang` cho chức năng mới  
- `fix/ten-sua-loi` hoặc `fix/issue-<số>` cho sửa lỗi

## 3. Thực hiện và commit code

```bash
$ git status                     # Xem các thay đổi đang có
$ git add .                      # Đưa toàn bộ thay đổi vào chỉ mục
$ git commit -m "Tóm tắt thay đổi, close #<issue_number>"   # Commit với message ngắn gọn
$ git push origin feature/ten-tinh-nang-moi
```
- Đặt message rõ ràng.  
- Đặt `close #<issue_number>` để tự động đóng issue sau khi merge.

## 4. Tạo Pull Request (PR)

- Trên GitHub, tạo **Pull Request** từ branch mới lên nhánh `main`.
- Liên kết PR với issue hoặc project board nếu có.
- Nhờ review code (code review) từ các bạn trong nhóm.

## 5. Review & Merge

- Sau khi đã được duyệt, merge PR vào nhánh `main`.
- Nếu có conflict, tự giải quyết trước khi merge.
- Thông thường thao tác merge được thực hiện trên GitHub.

Nếu merge bằng dòng lệnh:
```bash
$ git checkout main
$ git pull origin main
$ git merge feature/ten-tinh-nang-moi
$ git push origin main
```

## 6. Cập nhật branch local

- Sau khi có cập nhật mới ở main, mỗi thành viên nên update lại branch `main` của mình:

```bash
$ git checkout main
$ git pull origin main
```

---

## **Tóm tắt quy trình**

1. Thiết lập Git cá nhân (tên, email, autocrlf)
2. Tạo issue cho mỗi công việc.
3. Tạo branch mới từ main, làm việc trên branch đó.
4. Commit, push lên remote branch.
5. Tạo PR liên kết với issue, nhờ review, rồi merge vào main.
6. Cập nhật lại branch main trên máy của từng người.

---

## Một số lệnh Git cơ bản khác

```bash
$ git branch                   # Liệt kê các branch hiện có
$ git branch -d <branch>       # Xóa branch local đã hoàn thành
$ git log                      # Xem lịch sử commit
$ git diff                     # Xem các thay đổi chưa commit
$ git stash                    # Lưu tạm thời thay đổi chưa commit
$ git stash pop                # Lấy lại thay đổi đã stash
$ git remote -v                # Xem các remote
$ git fetch                    # Lấy về các commit mới từ remote mà chưa hợp nhất
```

---
