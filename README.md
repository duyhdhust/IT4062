# Cinema Booking System

Hệ thống đặt vé xem phim sử dụng C và PostgreSQL (Supabase)

## 📁 Cấu trúc dự án

```
IT4062/
├── server.c          # Main server loop
├── handler.c         # Command router
├── db.c              # Database layer
├── auth.c            # Authentication APIs (Login, Register, Logout)
├── booking.c         # User APIs (Search, Book tickets)
├── admin.c           # Admin APIs (Manage movies, showtimes)
├── utils.c           # Utilities (Logging, network helpers)
├── common.h          # Shared definitions & response codes
├── auth.h            # Auth function declarations
├── booking.h         # Booking function declarations
├── admin.h           # Admin function declarations
├── utils.h           # Utils function declarations
└── Makefile          # Build configuration
```

## 🚀 Tính năng

### ✅ Đã hoàn thành
- **Authentication**: Login, Register, Logout
- **Session Management**: Quản lý phiên cục bộ cho mỗi client
- **Database Connection Pooling**: Mỗi client giữ 1 kết nối DB duy nhất

### 🚧 Đang phát triển
- **User Features** (booking.c):
  - Tìm kiếm phim
  - Xem lịch chiếu
  - Đặt vé
  - Xem vé đã đặt

- **Admin Features** (admin.c):
  - Quản lý phim (Thêm, Sửa, Xóa)
  - Quản lý suất chiếu
  - Quản lý người dùng

## 🛠️ Build & Run

### Linux/macOS
```bash
make
./server
```

### Windows
```bash
# TODO: Thêm script build cho Windows
```

## 📡 API Commands

### Authentication
- `REG <username> <password>` - Đăng ký tài khoản
- `LOGIN <username> <password>` - Đăng nhập
- `LOGOUT` - Đăng xuất

### User (Coming soon)
- `SEARCH <movie_name>` - Tìm kiếm phim
- `LIST_GENRE <genre>` - Xem phim theo thể loại
- `BOOK <showtime_id> <seat>` - Đặt vé

### Admin (Coming soon)
- `ADD_MOVIE <info>` - Thêm phim mới
- `ADD_SHOW <info>` - Thêm suất chiếu
- `LOCK_USER <username>` - Khóa tài khoản

## 🗄️ Database

Sử dụng PostgreSQL trên Supabase. Cấu hình connection string trong `db.c`.

## 📝 Logs

Server ghi log vào file `log.txt`.

## 👥 Contributors

- Duy Đỗ (duyhdhust)
