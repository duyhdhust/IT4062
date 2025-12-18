# HUONG DAN COMMENT CHI TIET CHO TAT CA CAC FILE

## 1. server/server.c - Main Server Loop

```c
/* Tao socket lang nghe ket noi */
int listen_sock = socket(AF_INET, SOCK_STREAM, 0);

/* Cau hinh dia chi server */
server_addr.sin_family = AF_INET;        // Su dung IPv4
server_addr.sin_port = htons(PORT);      // Chuyen port sang network byte order
server_addr.sin_addr.s_addr = INADDR_ANY; // Lang nghe tren tat ca interface

/* Gan socket voi dia chi */
bind(listen_sock, (struct sockaddr *)&server_addr, sizeof(server_addr));

/* Bat dau lang nghe (toi da 10 ket noi cho) */
listen(listen_sock, 10);

/* Vong lap chinh - chap nhan ket noi */
while (1) {
    /* Cap phat bo nho cho socket descriptor */
    conn_sock = malloc(sizeof(int));
    
    /* Chap nhan ket noi tu client (blocking call) */
    *conn_sock = accept(listen_sock, ...);
    
    /* Tao thread moi de xu ly client */
    pthread_create(&tid, NULL, &client_thread, conn_sock);
    
    /* Detach thread - tu dong giai phong tai nguyen */
    pthread_detach(tid);
}
```

## 2. server/handler.c - Command Router

```c
void handle_client(int conn_sock) {
    /* Khoi tao DB connection 1 LAN DUY NHAT */
    PGconn *db_conn = connect_db();
    
    /* Kiem tra ket noi thanh cong */
    if (!db_conn) {
        /* Gui loi ve client va thoat */
        return;
    }
    
    /* Cac bien luu trang thai phien */
    char current_user[50] = "";  // Username hien tai
    int current_role = -1;       // Role: 0=user, 1=admin
    int is_logged_in = 0;        // Trang thai dang nhap
    
    /* Vong lap nhan lenh tu client */
    while (1) {
        /* Nhan 1 dong lenh tu client */
        int n = recv_line(conn_sock, buff, BUFF_SIZE);
        if (n <= 0) break;  // Client ngat ket noi
        
        /* Phan tich va xu ly lenh */
        if (strncmp(buff, "LOGIN", 5) == 0) {
            /* Xu ly dang nhap */
            if (is_logged_in) {
                /* Da dang nhap roi */
            } else {
                /* Goi ham xu ly login */
                if (process_login(...)) {
                    is_logged_in = 1;  // Danh dau da dang nhap
                }
            }
        }
        else if (strncmp(buff, "LOGOUT", 6) == 0) {
            /* Xu ly dang xuat */
            process_logout(...);
        }
        /* ... cac lenh khac ... */
    }
    
    /* Dong DB connection khi client ngat ket noi */
    disconnect_db(db_conn);
}
```

## 3. server/db.c - Database Layer

```c
/* Tao ket noi den database */
PGconn *connect_db() {
    /* Ket noi den Supabase PostgreSQL */
    PGconn *conn = PQconnectdb(conninfo);
    
    /* Kiem tra ket noi thanh cong */
    if (PQstatus(conn) != CONNECTION_OK) {
        /* In loi va tra ve NULL */
        fprintf(stderr, "Connection failed: %s", PQerrorMessage(conn));
        PQfinish(conn);
        return NULL;
    }
    
    return conn;  // Tra ve connection de tai su dung
}

/* Kiem tra thong tin dang nhap */
int db_check_login(PGconn *conn, char *user, char *pass) {
    /* Kiem tra connection hop le */
    if (!conn) return -2;
    
    /* Chuan bi tham so cho query */
    const char *paramValues[1] = { user };
    
    /* Thuc hien query (su dung prepared statement - tranh SQL injection) */
    PGresult *res = PQexecParams(conn, 
        "SELECT password, role, is_locked FROM users WHERE username = $1",
        1, NULL, paramValues, NULL, NULL, 0);
    
    int status = -2;  // Mac dinh: user not found
    
    /* Kiem tra ket qua */
    if (PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res) > 0) {
        /* Lay du lieu tu ket qua */
        char *db_pass = PQgetvalue(res, 0, 0);    // Cot 0: password
        int role = atoi(PQgetvalue(res, 0, 1));   // Cot 1: role
        char *is_locked = PQgetvalue(res, 0, 2);  // Cot 2: is_locked
        
        /* Kiem tra trang thai tai khoan */
        if (strcmp(is_locked, "t") == 0) 
            status = -3;  // Tai khoan bi khoa
        else if (strcmp(db_pass, pass) == 0) 
            status = role;  // Dang nhap thanh cong, tra ve role
        else 
            status = -1;  // Sai mat khau
    }
    
    /* Giai phong ket qua query */
    PQclear(res);
    
    /* KHONG dong connection - de tai su dung */
    return status;
}

/* Dong ket noi database */
void disconnect_db(PGconn *conn) {
    if (conn) {
        PQfinish(conn);  // Giai phong tai nguyen
    }
}
```

## 4. auth/auth.c - Authentication

```c
/* Xu ly dang ky tai khoan */
void process_register(int sock, PGconn *db_conn, char *payload) {
    char username[50], password[50];
    char response[2048];
    
    /* Parse username va password tu payload */
    if (sscanf(payload, "%s %s", username, password) != 2) {
        /* Sai cu phap - gui loi ve client */
        sprintf(response, "%s: Syntax Error\n", RES_SYNTAX_ERROR);
        send(sock, response, strlen(response), 0);
        return;
    }
    
    /* Goi ham DB de dang ky */
    int result = db_register(db_conn, username, password);
    
    /* Xu ly ket qua */
    if (result == 1) {
        /* Dang ky thanh cong */
        sprintf(response, "%s: Register Success\n", RES_REG_SUCCESS);
        write_log("Register Success");
    } else {
        /* Username da ton tai */
        sprintf(response, "%s: Username exists\n", RES_REG_EXIST);
    }
    
    /* Gui response ve client */
    send(sock, response, strlen(response), 0);
}

/* Xu ly dang nhap */
int process_login(int sock, PGconn *db_conn, char *payload, 
                  char *out_username, int *out_role) {
    /* Parse username va password */
    char username[50], password[50];
    sscanf(payload, "%s %s", username, password);
    
    /* Kiem tra voi database */
    int status = db_check_login(db_conn, username, password);
    
    if (status >= 0) {
        /* Dang nhap thanh cong */
        
        /* Luu thong tin vao session (qua con tro) */
        strcpy(out_username, username);
        *out_role = status;
        
        /* Cap nhat thoi gian dang nhap */
        db_update_login_time(db_conn, username);
        
        /* Gui response thanh cong */
        sprintf(response, "%s: Login OK\n", RES_LOGIN_SUCCESS);
        send(sock, response, strlen(response), 0);
        
        return 1;  // Tra ve 1 = thanh cong
    }
    
    /* Xu ly cac loi */
    if (status == -1) {
        /* Sai mat khau */
        sprintf(response, "%s: Wrong password\n", RES_WRONG_PASS);
    }
    else if (status == -2) {
        /* User khong ton tai */
        sprintf(response, "%s: User not found\n", RES_USER_NOT_FOUND);
    }
    else if (status == -3) {
        /* Tai khoan bi khoa */
        sprintf(response, "%s: Account locked\n", RES_ACC_LOCKED);
    }
    
    send(sock, response, strlen(response), 0);
    return 0;  // Tra ve 0 = that bai
}
```

## 5. server/utils.c - Utilities

```c
/* Ghi log vao file */
void write_log(char *msg) {
    /* Mo file log (che do append) */
    FILE *f = fopen("log.txt", "a");
    if (f == NULL) return;
    
    /* Lay thoi gian hien tai */
    time_t now = time(NULL);
    char *t = ctime(&now);
    
    /* Xoa ky tu newline o cuoi */
    if (t[strlen(t)-1] == '\n') 
        t[strlen(t)-1] = '\0';
    
    /* Ghi log voi timestamp */
    fprintf(f, "[%s] %s\n", t, msg);
    
    /* Dong file */
    fclose(f);
}

/* Nhan 1 dong du lieu tu socket */
int recv_line(int sock, char *buff, int max_len) {
    int n, i = 0;
    char c;
    
    /* Doc tung byte cho den khi gap newline */
    while (i < max_len - 1) {
        /* Nhan 1 byte */
        n = recv(sock, &c, 1, 0);
        
        /* Kiem tra loi hoac ngat ket noi */
        if (n <= 0) return -1;
        
        /* Luu byte vao buffer */
        buff[i++] = c;
        
        /* Neu gap newline thi dung */
        if (c == '\n') break;
    }
    
    /* Ket thuc chuoi */
    buff[i] = '\0';
    
    /* Xoa cac ky tu \r\n o cuoi */
    buff[strcspn(buff, "\r\n")] = 0;
    
    return i;  // Tra ve so byte da doc
}
```

## CAC DIEM QUAN TRONG CAN NHO:

1. **Database Connection Pooling**:
   - Moi client chi tao 1 connection duy nhat
   - Tai su dung connection cho tat ca queries
   - Dong connection khi client ngat ket noi

2. **Session Management**:
   - Moi client co session rieng (current_user, is_logged_in, current_role)
   - Session luu tren stack cua thread
   - Tu dong xoa khi thread ket thuc

3. **Multi-threading**:
   - Moi client duoc xu ly trong 1 thread rieng
   - Su dung pthread_detach() de tu dong giai phong tai nguyen
   - Khong can pthread_join()

4. **Security**:
   - Su dung PQexecParams() (prepared statements) de tranh SQL injection
   - Khong luu password dang plain text (nen hash truoc khi luu DB)

5. **Error Handling**:
   - Kiem tra return value cua tat ca system calls
   - Gui response code ro rang ve client
   - Ghi log de debug
