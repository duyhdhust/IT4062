/* ============================================================================
 * File: db.c
 * Description: Database Layer - Xu ly tat ca tuong tac voi PostgreSQL
 * ============================================================================
 */

#include "common.h"

// Connection string den Supabase PostgreSQL
// Format: postgresql://username:password@host:port/database
const char *conninfo = "postgresql://postgres.mhccdysgnmzzvoysxikz:IT4062@aws-0-ap-south-1.pooler.supabase.com:5432/postgres";

/* ----------------------------------------------------------------------------
 * Function: connect_db
 * Description: Tao ket noi moi den database
 * Returns: Con tro PGconn* neu thanh cong, NULL neu that bai
 * ----------------------------------------------------------------------------
 */
PGconn *connect_db() {
    // Tao ket noi den database su dung connection string
    PGconn *conn = PQconnectdb(conninfo);
    
    // Kiem tra trang thai ket noi
    if (PQstatus(conn) != CONNECTION_OK) {
        // In thong bao loi ra stderr
        fprintf(stderr, "Connection to database failed: %s", PQerrorMessage(conn));
        
        // Dong ket noi va giai phong tai nguyen
        PQfinish(conn);
        
        // Tra ve NULL de bao loi
        return NULL;
    }
    
    // Tra ve connection de tai su dung
    return conn;
}

/* ----------------------------------------------------------------------------
 * Function: db_check_login
 * Description: Kiem tra thong tin dang nhap cua user
 * Parameters:
 *   - conn: Database connection (TAI SU DUNG, khong tao moi)
 *   - user: Username can kiem tra
 *   - pass: Password can kiem tra
 * Returns:
 *   >= 0: Dang nhap thanh cong, tra ve role (0=user, 1=admin)
 *   -1: Sai mat khau
 *   -2: User khong ton tai
 *   -3: Tai khoan bi khoa
 * ----------------------------------------------------------------------------
 */
int db_check_login(PGconn *conn, char *user, char *pass) {
    // Kiem tra connection hop le
    if (!conn) return -2;

    // Chuan bi tham so cho prepared statement (tranh SQL injection)
    const char *paramValues[1] = { user };
    
    // Thuc hien query voi prepared statement
    // $1 se duoc thay the boi paramValues[0] (user)
    PGresult *res = PQexecParams(conn, 
        "SELECT password, role, is_locked FROM users WHERE username = $1",
        1,              // So luong tham so
        NULL,           // Khong chi dinh kieu du lieu
        paramValues,    // Gia tri tham so
        NULL,           // Khong chi dinh do dai
        NULL,           // Khong chi dinh format
        0);             // Ket qua dang text (khong phai binary)

    // Mac dinh: user not found
    int status = -2; 

    // Kiem tra query thanh cong va co ket qua
    if (PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res) > 0) {
        // Lay password tu database (cot 0, dong 0)
        char *db_pass = PQgetvalue(res, 0, 0);
        
        // Lay role tu database (cot 1, dong 0) va chuyen sang int
        int role = atoi(PQgetvalue(res, 0, 1));
        
        // Lay trang thai khoa (cot 2, dong 0)
        char *is_locked = PQgetvalue(res, 0, 2);

        // Kiem tra tai khoan co bi khoa khong (PostgreSQL tra ve 't' cho true)
        if (strcmp(is_locked, "t") == 0) 
            status = -3;  // Tai khoan bi khoa
        
        // Kiem tra password co khop khong
        else if (strcmp(db_pass, pass) == 0) 
            status = role;  // Dang nhap thanh cong, tra ve role
        
        // Password khong khop
        else 
            status = -1;  // Sai mat khau
    }

    // Giai phong ket qua query
    PQclear(res);
    
    // KHONG dong connection - de tai su dung cho cac query khac
    // Tra ve status code
    return status;
}

/* ----------------------------------------------------------------------------
 * Function: db_register
 * Description: Dang ky tai khoan moi
 * Parameters:
 *   - conn: Database connection
 *   - user: Username moi
 *   - pass: Password moi
 * Returns: 1 neu thanh cong, 0 neu that bai (username da ton tai)
 * ----------------------------------------------------------------------------
 */
int db_register(PGconn *conn, char *user, char *pass) {
    // Kiem tra connection hop le
    if (!conn) return 0;

    // Chuan bi tham so: username va password
    const char *paramValues[2] = { user, pass };
    
    // Thuc hien INSERT query
    // Role mac dinh = 0 (user binh thuong, khong phai admin)
    PGresult *res = PQexecParams(conn,
        "INSERT INTO users (username, password, role) VALUES ($1, $2, 0)",
        2,              // 2 tham so
        NULL, 
        paramValues, 
        NULL, 
        NULL, 
        0);

    // Kiem tra INSERT thanh cong
    int success = (PQresultStatus(res) == PGRES_COMMAND_OK) ? 1 : 0;
    
    // Giai phong ket qua
    PQclear(res);
    
    // Tra ve ket qua
    return success;
}

/* ----------------------------------------------------------------------------
 * Function: db_update_login_time
 * Description: Cap nhat thoi gian dang nhap cuoi cung cua user
 * Parameters:
 *   - conn: Database connection
 *   - username: Username can cap nhat
 * Returns: void
 * ----------------------------------------------------------------------------
 */
void db_update_login_time(PGconn *conn, char *username) {
    // Kiem tra connection hop le
    if (!conn) return;
    
    // Chuan bi tham so
    const char *paramValues[1] = { username };
    
    // Cap nhat last_login = thoi gian hien tai (NOW())
    PGresult *res = PQexecParams(conn,
        "UPDATE users SET last_login = NOW() WHERE username = $1",
        1, 
        NULL, 
        paramValues, 
        NULL, 
        NULL, 
        0);
    
    // Giai phong ket qua
    PQclear(res);
    
    // Khong can tra ve gia tri
}

/* ----------------------------------------------------------------------------
 * Function: disconnect_db
 * Description: Dong ket noi database va giai phong tai nguyen
 * Parameters:
 *   - conn: Database connection can dong
 * Returns: void
 * ----------------------------------------------------------------------------
 */
void disconnect_db(PGconn *conn) {
    // Kiem tra connection hop le
    if (conn) {
        // Dong ket noi va giai phong tai nguyen
        PQfinish(conn);
    }
}
