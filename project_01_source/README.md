# BÀI TẬP 01: SỐ NGUYÊN TỐ & TRAO ĐỔI KHÓA DIFFIE-HELLMAN
**Môn học:** Nhập môn Mã hóa Mật mã  
**Đơn vị:** Bộ môn Công nghệ Tri thức  

---

## 1. Mục tiêu bài tập
1. **Lũy thừa mô-đun:** Triển khai hàm `modular_exponentiation` để thực hiện phép tính $(base^{exponent}) \pmod{mod}$ một cách hiệu quả, phục vụ cho việc trao đổi khóa.
2. **Sinh số nguyên tố lớn an toàn:** Triển khai hàm `generate_safe_prime` để sinh số nguyên tố lớn an toàn $p$ (trong đó $p$ là số nguyên tố và $q = \frac{p - 1}{2}$ cũng là số nguyên tố), độ lớn tối thiểu 512 bit, tự cài đặt thuật toán kiểm tra nguyên tố xác suất Miller-Rabin không dùng thư viện ngoài.
3. **Sinh khóa riêng ngẫu nhiên:** Triển khai hàm `generate_private_key` sinh khóa riêng ngẫu nhiên trong khoảng $[2, p - 2]$.
4. **Trao đổi khóa Diffie-Hellman (DHKE):** Alice và Bob tính toán giá trị công khai, trao đổi và tính toán bí mật chung $s$, xác minh kết quả bí mật chung trùng khớp.

---

## 2. Cấu trúc mã nguồn dự án

```
project_01_source/
├── src/
│   ├── uint512.h           # Hệ thống số lớn 512-bit tự cài đặt (không dùng thư viện ngoài)
│   ├── dh.h                # Khai báo các hàm giao thức Diffie-Hellman & số học mô-đun
│   ├── dh.cpp              # Cài đặt chi tiết: AddMod, MultMod, ExpMod, Miller-Rabin, Safe Prime
│   └── main.cpp            # Chương trình chính thực hiện kịch bản trao đổi khóa Diffie-Hellman
├── lib/
│   └── fips202/            # Bộ sinh số ngẫu nhiên PRNG dựa trên chuẩn Keccak / SHAKE-128
│       ├── fips202.c / .h
│       ├── prng.h / .cpp   # Chuẩn cho môi trường Linux / macOS
│       └── prng_win.h / .cpp # Hỗ trợ bổ sung cho môi trường Windows
├── bin/                    # Thư mục chứa tệp thực thi sau khi biên dịch
├── Makefile                # Kịch bản biên dịch chuẩn C++14 bằng g++ trên Linux
└── README.md               # Tài liệu hướng dẫn chi tiết
```

---

## 3. Các thuật toán triển khai

| Tên hàm | Nhiệm vụ | Thuật toán áp dụng |
|---|---|---|
| `add_mod(x, y, p)` | $(x + y) \pmod p$ | Thuật toán `AddMod` trong Pseudocode (chống tràn số 512-bit) |
| `mul_mod(x, y, p)` | $(x \cdot y) \pmod p$ | Thuật toán `MultMod` trong Pseudocode (Double-and-Add) |
| `modular_exponentiation(base, exp, mod)` | $(base^{exp}) \pmod{mod}$ | Thuật toán `ExpMod` trong Pseudocode (Square-and-Multiply) |
| `miller_rabin(n, k, ctx)` | Kiểm tra $n$ có là số nguyên tố | Thuật toán kiểm tra nguyên tố xác suất Miller-Rabin ($k=20$ vòng) |
| `generate_safe_prime(ctx, bit_size)` | Sinh số nguyên tố an toàn $p$ | Sàng lọc số nguyên tố nhỏ (Trial division) + Miller-Rabin ($p$ và $\frac{p-1}{2}$) |
| `generate_private_key(ctx, p)` | Sinh khóa riêng | Rejection Sampling trong khoảng $[2, p - 2]$ |

---

## 4. Hướng dẫn biên dịch và chạy chương trình

### 4.1. Môi trường Linux / WSL / macOS (Môi trường nộp bài theo chuẩn đề bài)
Yêu cầu trình biên dịch `g++` hỗ trợ chuẩn C++14 trở xuống.

```bash
cd project_01_source

# 1. Biên dịch dự án
make dh

# 2. Chạy với kích thước mặc định (512 bit)
make run
# Hoặc truyền tham số số bit tùy chỉnh (ví dụ: 128 bit, 256 bit, 512 bit):
./bin/dh 512
./bin/dh 128

# 3. Dọn dẹp tệp tin sau khi chạy
make clean
```

### 4.2. Môi trường Windows (Visual Studio / MSVC)
Trong cửa sổ PowerShell:
```powershell
cd project_01_source

# Chạy tệp thực thi có sẵn:
.\bin\dh.exe 64
.\bin\dh.exe 128
.\bin\dh.exe 512
```

---

## 5. Kết quả thực thi mẫu (bám sát chương trình mẫu đề bài)

```text
--- TRAO DOI KHOA DIFFIE-HELLMAN (KICH THUOC: 512 BIT) ---

[1] Dang sinh so nguyen to an toan p (512 bit)...
So nguyen to an toan p: 0x...
Phan tu sinh g: 0x2

[2] Sinh khoa rieng cua Alice va Bob...
Khoa rieng Alice (a): 0x...
Khoa rieng Bob   (b): 0x...

[3] Tinh gia tri cong khai cua Alice va Bob...
Gia tri cong khai Alice (A = g^a % p): 0x...
Gia tri cong khai Bob   (B = g^b % p): 0x...

[4] Tinh bi mat chung...

[5] Hien thi ket qua va xac minh rang bi mat chung trung khop:
Bi mat chung Alice nhan duoc: 0x...
Bi mat chung Bob nhan duoc: 0x...
Qua trinh tinh toan dung khong? 1
```

---

## 6. Quy định nộp bài
Theo mục 3 của đề bài Lab01, sinh viên đóng gói tệp nộp bài theo định dạng:
```
<MSSV1-MSSV2-MSSV3-MSSV4>.zip
├── project_01_source/      # Thư mục mã nguồn C/C++ và Makefile
└── project_01_report/      # Thư mục chứa báo cáo định dạng PDF
```
