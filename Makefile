# Tên file chạy đầu ra
TARGET = server

# Trình biên dịch
CC = gcc

# Các cờ chung (Common Flags)
# -Wall: Hiện tất cả cảnh báo
# -pthread: Đa luồng
CFLAGS = -Wall -pthread

# Thư viện liên kết chung
LIBS = -lpq

# --- TỰ ĐỘNG PHÁT HIỆN HỆ ĐIỀU HÀNH ---
UNAME_S := $(shell uname -s)

# 1. CẤU HÌNH CHO MACOS (Darwin) - Chip M1/M2/M3
ifeq ($(UNAME_S),Darwin)
    # Homebrew trên Apple Silicon nằm ở /opt/homebrew
    # Chỉ định rõ đường dẫn include và lib của libpq
    INCLUDES = -I/opt/homebrew/opt/libpq/include -Iserver -Iauth -Iuser -Iadmin
    LDFLAGS  = -L/opt/homebrew/opt/libpq/lib
    
    # In ra thông báo để biết đang chạy mode nào
    $(info [INFO] Phat hien macOS (Apple Silicon). Dang cau hinh Homebrew...)
endif

# 2. CẤU HÌNH CHO LINUX (Ubuntu/WSL)
ifeq ($(UNAME_S),Linux)
    # Trên Ubuntu, header Postgres thường nằm ở đây
    INCLUDES = -I/usr/include/postgresql -Iserver -Iauth -Iuser -Iadmin
    # Lib nằm ở đường dẫn chuẩn hệ thống nên k cần LDFLAGS đặc biệt
    LDFLAGS  = 
    
    $(info [INFO] Phat hien Linux/Ubuntu. Dang cau hinh mac dinh...)
endif

# Danh sách file nguồn theo folder
SRCS = server/server.c server/handler.c server/db.c \
       auth/auth.c user/booking.c admin/admin.c server/utils.c

# --- QUY TẮC BIÊN DỊCH ---
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(INCLUDES) $(SRCS) -o $(TARGET) $(LDFLAGS) $(LIBS)
	@echo "--------------------------------------"
	@echo "  BIEN DICH THANH CONG!  "
	@echo "  Chay server bang lenh: ./$(TARGET)"
	@echo "--------------------------------------"

# Dọn dẹp
clean:
	rm -f $(TARGET)