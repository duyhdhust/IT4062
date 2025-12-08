#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <libpq-fe.h> // Thư viện Postgres

#define PORT 8080
#define BUFF_SIZE 2048 

// ====================================================
//                  RESPONSE CODES
// ====================================================

// --- KẾT NỐI ---
#define RES_CONNECT_SUCCESS   "100" 

// --- AUTH (1xx) ---
#define RES_REG_SUCCESS       "110"
#define RES_REG_EXIST         "111"

#define RES_LOGIN_SUCCESS     "120"
#define RES_ACC_LOCKED        "121"
#define RES_USER_NOT_FOUND    "122"
#define RES_WRONG_PASS        "123"
#define RES_ALREADY_LOGGED_IN "124" 

#define RES_LOGOUT_SUCCESS    "130"
#define RES_NOT_LOGGED_IN     "131"

// --- SEARCH & LIST (2xx) ---
#define RES_SEARCH_FOUND      "200"
#define RES_SEARCH_NOT_FOUND  "201"
#define RES_LIST_GENRE_OK     "210"
#define RES_LIST_CINEMA_OK    "220"
#define RES_LIST_TIME_OK      "230"

// --- BOOKING (3xx) ---
#define RES_BOOK_OK           "300"
#define RES_ERR_CINEMA_EMPTY  "301" 
#define RES_ERR_CINEMA_404    "302" 
#define RES_ERR_SHOW_FULL     "303" 
#define RES_ERR_SHOW_404      "304" 
#define RES_ERR_SEAT_BOOKED   "305" 
#define RES_ERR_SEAT_404      "306" 
#define RES_ERR_MOVIE_INVALID "307" 

// --- ADMIN (4xx - 5xx) ---
#define RES_ADD_MOVIE_OK      "410"
#define RES_ADD_MOVIE_EXIST   "411"
#define RES_ADD_SHOW_OK       "420"
#define RES_ADD_SHOW_MOVIE_404 "421"
#define RES_ADD_SHOW_CINEMA_404 "422"

#define RES_EDIT_SHOW_OK      "510"
#define RES_EDIT_SHOW_404     "511"
#define RES_EDIT_HAS_TICKET   "512"
#define RES_EDIT_CINEMA_404   "513"

#define RES_DEL_MOVIE_OK      "520"
#define RES_DEL_MOVIE_404     "521"
#define RES_DEL_MOVIE_FAIL    "522"

#define RES_DEL_SHOW_OK       "530"
#define RES_DEL_SHOW_404      "531"
#define RES_DEL_SHOW_FAIL     "532"

#define RES_SYNTAX_ERROR      "999"

#endif