#ifndef AUTH_H
#define AUTH_H

#define _POSIX_C_SOURCE 200809L
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#else
#include <termios.h>
#include <unistd.h>
#endif

/* ─── Constants ─────────────────────────────────────────────── */
#define MAX_USERNAME_LEN 32
#define MAX_PASSWORD_LEN 64
#define MAX_USERS 100
#define USER_DB_FILE "data/users.dat"
#define SESSION_FILE "data/session.dat"
#define XOR_KEY 0x5A         /* simple XOR cipher key for passwords   */
#define SESSION_XOR_KEY 0x3F /* separate XOR key for session file      */
#define SESSION_MAGIC                                                          \
  0xDEADBEEF /* sentinel to detect corrupt/tampered file                       \
              */

/* ─── Structs ────────────────────────────────────────────────── */
typedef struct {
  char username[MAX_USERNAME_LEN];
  char password[MAX_PASSWORD_LEN]; /* stored XOR-encrypted */
  int user_id;
} User;

typedef struct {
  char username[MAX_USERNAME_LEN];
  int user_id;
} Session;

/*
 * SessionRecord – what is physically written to session.dat.
 * Every byte except `magic` is XOR-encrypted with SESSION_XOR_KEY
 * before writing, and decrypted after reading.
 */
typedef struct {
  unsigned int magic; /* SESSION_MAGIC – integrity check  */
  char username[MAX_USERNAME_LEN];
  int user_id;
  long login_time; /* time_t stored as long            */
} SessionRecord;

/* ─── Function Prototypes ────────────────────────────────────── */

/* Utility */
void clear_screen(void);
void print_banner(void);
void print_separator(void);
void flush_input(void);
void get_masked_input(char *buf, int max_len);

/* Encryption */
void xor_encrypt(char *text, char *result, int len);
void xor_decrypt(char *cipher, char *result, int len);

/* File Handling */
int load_users(User users[], int *count);
int save_users(User users[], int count);

/* Session Persistence */
int save_session(const Session *session);
int load_session(Session *session);
void clear_session_file(void);
void print_session_info(const Session *session);

/* Validation */
int is_valid_username(const char *username);
int is_valid_password(const char *password);
int username_exists(User users[], int count, const char *username);

/* Auth Actions */
int register_user(User users[], int *count);
int login_user(User users[], int count, Session *session);

/* Main Auth Menu */
int auth_menu(Session *session);

#endif /* AUTH_H */