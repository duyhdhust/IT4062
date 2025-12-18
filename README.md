# Cinema Booking System

Hệ thống đặt vé xem phim sử dụng C và PostgreSQL (Supabase)

## 📁 Cấu trúc dự án

```
IT4062/
├── server/            # Server core (main, handler, db, utils)
│   ├── server.c       # Main server loop
│   ├── handler.c      # Command router
│   ├── db.c           # Database layer
│   ├── utils.c        # Utilities (logging, network)
│   ├── common.h       # Shared definitions
│   └── utils.h        # Utils declarations
├── auth/              # Authentication module
│   ├── auth.c         # Login, Register, Logout
│   └── auth.h
├── user/              # User features (booking, search)
│   ├── booking.c      # Booking APIs
│   └── booking.h
├── admin/             # Admin features
│   ├── admin.c        # Manage movies, showtimes
│   └── admin.h
├── client/            # Client application
│   └── client.c
├── Makefile
└── README.md
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
