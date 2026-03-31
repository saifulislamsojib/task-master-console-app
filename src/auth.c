#include "auth.h"

/* ══════════════════════════════════════════════════════════════
   PORTABLE CASE-INSENSITIVE COMPARE
   ══════════════════════════════════════════════════════════════ */
static int str_icmp(const char *a, const char *b) {
  while (*a && *b) {
    int diff = tolower((unsigned char)*a) - tolower((unsigned char)*b);
    if (diff)
      return diff;
    a++;
    b++;
  }
  return tolower((unsigned char)*a) - tolower((unsigned char)*b);
}

/* ══════════════════════════════════════════════════════════════
   UTILITY FUNCTIONS
   ══════════════════════════════════════════════════════════════ */

void clear_screen(void) { system("clear"); }

void print_banner(void) {
  printf("\n");
  printf("  ╔══════════════════════════════════════════════╗\n");
  printf("  ║                                              ║\n");
  printf("  ║    ████████╗ █████╗ ███████╗██╗  ██╗        ║\n");
  printf("  ║       ██╔══╝██╔══██╗██╔════╝██║ ██╔╝        ║\n");
  printf("  ║       ██║   ███████║███████╗█████╔╝         ║\n");
  printf("  ║       ██║   ██╔══██║╚════██║██╔═██╗         ║\n");
  printf("  ║       ██║   ██║  ██║███████║██║  ██╗        ║\n");
  printf("  ║       ╚═╝   ╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝        ║\n");
  printf("  ║                                              ║\n");
  printf("  ║      M A S T E R                            ║\n");
  printf("  ║      Project & Task Management System       ║\n");
  printf("  ║                                              ║\n");
  printf("  ╚══════════════════════════════════════════════╝\n");
  printf("\n");
}

void print_separator(void) {
  printf("  ──────────────────────────────────────────────\n");
}

void flush_input(void) {
  int c;
  while ((c = getchar()) != '\n' && c != EOF)
    ;
}

/*
 * get_masked_input – reads password char-by-char, printing '*'
 * Works on Windows (conio.h) and Linux/macOS (termios).
 */
void get_masked_input(char *buf, int max_len) {
  int i = 0;

#ifdef _WIN32
  char ch;
  while ((ch = _getch()) != '\r' && i < max_len - 1) {
    if (ch == '\b') { /* backspace */
      if (i > 0) {
        i--;
        printf("\b \b");
      }
    } else {
      buf[i++] = ch;
      printf("*");
    }
  }
  buf[i] = '\0';
  printf("\n");
#else
  struct termios oldt, newt;
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ECHO | ICANON); /* disable echo & line-buffering */
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);

  char ch;
  while (read(STDIN_FILENO, &ch, 1) == 1 && ch != '\n' && i < max_len - 1) {
    if (ch == 127 || ch == '\b') { /* backspace / DEL */
      if (i > 0) {
        i--;
        printf("\b \b");
        fflush(stdout);
      }
    } else {
      buf[i++] = ch;
      printf("*");
      fflush(stdout);
    }
  }
  buf[i] = '\0';
  printf("\n");

  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif
}

/* ══════════════════════════════════════════════════════════════
   ENCRYPTION – Simple XOR Cipher
   ══════════════════════════════════════════════════════════════ */

void xor_encrypt(char *text, char *result, int len) {
  for (int i = 0; i < len; i++)
    result[i] = text[i] ^ XOR_KEY;
  result[len] = '\0';
}

void xor_decrypt(char *cipher, char *result, int len) {
  /* XOR is symmetric – same operation decrypts */
  xor_encrypt(cipher, result, len);
}

/* ══════════════════════════════════════════════════════════════
   FILE HANDLING
   ══════════════════════════════════════════════════════════════ */

/*
 * load_users – reads all User records from binary file.
 * Returns 1 on success, 0 if file doesn't exist yet (first run).
 */
int load_users(User users[], int *count) {
  FILE *fp = fopen(USER_DB_FILE, "rb");
  if (!fp) {
    *count = 0;
    return 0; /* no file yet – that's OK on first run */
  }

  *count = 0;
  while (*count < MAX_USERS &&
         fread(&users[*count], sizeof(User), 1, fp) == 1) {
    (*count)++;
  }

  fclose(fp);
  return 1;
}

/*
 * save_users – writes all User records to binary file.
 * Returns 1 on success, 0 on failure.
 */
int save_users(User users[], int count) {
  FILE *fp = fopen(USER_DB_FILE, "wb");
  if (!fp) {
    printf("\n  [ERROR] Could not open database file for writing.\n");
    return 0;
  }

  for (int i = 0; i < count; i++)
    fwrite(&users[i], sizeof(User), 1, fp);

  fclose(fp);
  return 1;
}

/* ══════════════════════════════════════════════════════════════
   SESSION PERSISTENCE
   ══════════════════════════════════════════════════════════════

   Layout of session.dat (binary, all bytes XOR-encrypted):
   ┌──────────────────────────────────────┐
   │  unsigned int  magic        (4 B)    │  → SESSION_MAGIC
   │  char          username    (32 B)    │
   │  int           user_id     (4 B)     │
   │  long          login_time  (8 B)     │  → time() at login
   └──────────────────────────────────────┘
   The entire struct is XOR-encrypted byte-by-byte with
   SESSION_XOR_KEY before writing, and decrypted after reading.
   ══════════════════════════════════════════════════════════════ */

static void encrypt_session_record(SessionRecord *rec) {
  unsigned char *p = (unsigned char *)rec;
  for (size_t i = 0; i < sizeof(SessionRecord); i++)
    p[i] ^= SESSION_XOR_KEY;
}
/* decrypt is identical (XOR is symmetric) */
#define decrypt_session_record encrypt_session_record

/*
 * save_session – encrypts and writes the session to session.dat.
 * Returns 1 on success, 0 on failure.
 */
int save_session(const Session *session) {
  SessionRecord rec;
  memset(&rec, 0, sizeof(SessionRecord));

  rec.magic = SESSION_MAGIC;
  rec.user_id = session->user_id;
  rec.login_time = (long)time(NULL);
  strncpy(rec.username, session->username, MAX_USERNAME_LEN - 1);

  encrypt_session_record(&rec); /* encrypt in-place before writing */

  FILE *fp = fopen(SESSION_FILE, "wb");
  if (!fp)
    return 0;
  fwrite(&rec, sizeof(SessionRecord), 1, fp);
  fclose(fp);
  return 1;
}

/*
 * load_session – reads session.dat, decrypts, validates magic.
 * Populates *session and returns 1 if a valid session exists.
 * Returns 0 if no file, file is corrupt, or magic mismatches.
 */
int load_session(Session *session) {
  FILE *fp = fopen(SESSION_FILE, "rb");
  if (!fp)
    return 0;

  SessionRecord rec;
  if (fread(&rec, sizeof(SessionRecord), 1, fp) != 1) {
    fclose(fp);
    return 0;
  }
  fclose(fp);

  decrypt_session_record(&rec); /* decrypt in-place after reading */

  if (rec.magic != SESSION_MAGIC)
    return 0; /* tampered / corrupt */

  strncpy(session->username, rec.username, MAX_USERNAME_LEN - 1);
  session->user_id = rec.user_id;
  return 1;
}

/*
 * clear_session_file – deletes session.dat (called on logout).
 */
void clear_session_file(void) { remove(SESSION_FILE); }

/*
 * print_session_info – shows restored-session banner.
 */
void print_session_info(const Session *session) {
  /* Read login time from file for display */
  FILE *fp = fopen(SESSION_FILE, "rb");
  long login_time = 0;
  if (fp) {
    SessionRecord rec;
    if (fread(&rec, sizeof(SessionRecord), 1, fp) == 1) {
      decrypt_session_record(&rec);
      if (rec.magic == SESSION_MAGIC)
        login_time = rec.login_time;
    }
    fclose(fp);
  }

  print_separator();
  printf("\n  ✔  Session restored!\n");
  printf("     User     : %s\n", session->username);
  printf("     ID       : %d\n", session->user_id);
  if (login_time) {
    char timebuf[64];
    time_t t = (time_t)login_time;
    strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", localtime(&t));
    printf("     Logged in: %s\n", timebuf);
  }
  print_separator();
  printf("\n  Press ENTER to continue to dashboard...");
  /* caller handles flush */
}

/*
 * is_valid_username
 *   - 3 to MAX_USERNAME_LEN-1 chars
 *   - Only letters, digits, and underscores
 *   - Must start with a letter
 */
int is_valid_username(const char *username) {
  int len = (int)strlen(username);
  if (len < 3 || len >= MAX_USERNAME_LEN)
    return 0;
  if (!isalpha((unsigned char)username[0]))
    return 0;
  for (int i = 1; i < len; i++) {
    if (!isalnum((unsigned char)username[i]) && username[i] != '_')
      return 0;
  }
  return 1;
}

/*
 * is_valid_password
 *   - 6 to MAX_PASSWORD_LEN-1 chars
 *   - At least one digit
 *   - At least one uppercase letter
 */
int is_valid_password(const char *password) {
  int len = (int)strlen(password);
  if (len < 6 || len >= MAX_PASSWORD_LEN)
    return 0;

  int has_digit = 0;
  int has_upper = 0;

  for (int i = 0; i < len; i++) {
    if (isdigit((unsigned char)password[i]))
      has_digit = 1;
    if (isupper((unsigned char)password[i]))
      has_upper = 1;
  }
  return has_digit && has_upper;
}

/*
 * username_exists – case-insensitive duplicate check.
 */
int username_exists(User users[], int count, const char *username) {
  for (int i = 0; i < count; i++) {
    if (str_icmp(users[i].username, username) == 0)
      return 1;
  }
  return 0;
}

/* ══════════════════════════════════════════════════════════════
   REGISTER
   ══════════════════════════════════════════════════════════════ */

int register_user(User users[], int *count) {
  clear_screen();
  print_banner();

  printf("  ┌─────────────────────────────────────────┐\n");
  printf("  │           CREATE NEW ACCOUNT            │\n");
  printf("  └─────────────────────────────────────────┘\n\n");

  if (*count >= MAX_USERS) {
    printf("  [ERROR] User limit reached. Cannot register more users.\n");
    printf("\n  Press ENTER to return...");
    flush_input();
    return 0;
  }

  char username[MAX_USERNAME_LEN];
  char password[MAX_PASSWORD_LEN];
  char confirm[MAX_PASSWORD_LEN];

  /* ── Username ── */
  while (1) {
    printf("  Username  : ");
    if (!fgets(username, sizeof(username), stdin))
      continue;
    username[strcspn(username, "\n")] = '\0'; /* strip newline */

    if (!is_valid_username(username)) {
      printf("  [!] Username must be 3-31 chars, start with a letter,\n");
      printf("      and contain only letters, digits, or underscores.\n\n");
      continue;
    }
    if (username_exists(users, *count, username)) {
      printf("  [!] Username already taken. Choose another.\n\n");
      continue;
    }
    break;
  }

  /* ── Password ── */
  printf("\n  Password rules: min 6 chars, 1 uppercase, 1 digit.\n\n");
  while (1) {
    printf("  Password  : ");
    fflush(stdout);
    get_masked_input(password, MAX_PASSWORD_LEN);

    if (!is_valid_password(password)) {
      printf("  [!] Password too weak. Try again.\n\n");
      fflush(stdout);
      continue;
    }

    printf("  Confirm   : ");
    fflush(stdout);
    get_masked_input(confirm, MAX_PASSWORD_LEN);

    if (strcmp(password, confirm) != 0) {
      printf("  [!] Passwords do not match. Try again.\n\n");
      continue;
    }
    break;
  }

  /* ── Build User record ── */
  User new_user;
  memset(&new_user, 0, sizeof(User));

  strncpy(new_user.username, username, MAX_USERNAME_LEN - 1);
  new_user.user_id = *count + 1; /* simple auto-increment ID */

  int pw_len = (int)strlen(password);
  xor_encrypt(password, new_user.password, pw_len);

  users[*count] = new_user;
  (*count)++;

  if (!save_users(users, *count)) {
    (*count)--; /* rollback */
    printf("\n  [ERROR] Failed to save user. Please try again.\n");
    printf("\n  Press ENTER to return...");
    flush_input();
    return 0;
  }

  printf("\n  ✔  Account created successfully!\n");
  printf("     Welcome aboard, %s (ID: %d)\n", new_user.username,
         new_user.user_id);
  printf("\n  Press ENTER to continue...");
  flush_input();
  return 1;
}

/* ══════════════════════════════════════════════════════════════
   LOGIN
   ══════════════════════════════════════════════════════════════ */

#define MAX_LOGIN_ATTEMPTS 3

int login_user(User users[], int count, Session *session) {
  clear_screen();
  print_banner();

  printf("  ┌─────────────────────────────────────────┐\n");
  printf("  │              USER  LOGIN                │\n");
  printf("  └─────────────────────────────────────────┘\n\n");

  if (count == 0) {
    printf("  [!] No accounts registered yet.\n");
    printf("      Please register first.\n");
    printf("\n  Press ENTER to return...");
    flush_input();
    return 0;
  }

  char username[MAX_USERNAME_LEN];
  char password[MAX_PASSWORD_LEN];
  int attempts = 0;

  while (attempts < MAX_LOGIN_ATTEMPTS) {
    int remaining = MAX_LOGIN_ATTEMPTS - attempts;

    if (attempts > 0) {
      printf("  [!] Invalid credentials. %d attempt(s) remaining.\n\n",
             remaining);
    }

    printf("  Username  : ");
    fflush(stdout);
    if (!fgets(username, sizeof(username), stdin))
      continue;
    username[strcspn(username, "\n")] = '\0';

    printf("  Password  : ");
    fflush(stdout);
    get_masked_input(password, MAX_PASSWORD_LEN);

    /* Find matching user */
    int found = -1;
    for (int i = 0; i < count; i++) {
      if (str_icmp(users[i].username, username) == 0) {
        found = i;
        break;
      }
    }

    if (found != -1) {
      /* Decrypt stored password and compare */
      char decrypted[MAX_PASSWORD_LEN];
      int pw_len = (int)strlen(users[found].password);
      xor_decrypt(users[found].password, decrypted, pw_len);

      if (strcmp(password, decrypted) == 0) {
        /* ── Successful Login ── */
        strncpy(session->username, users[found].username, MAX_USERNAME_LEN - 1);
        session->user_id = users[found].user_id;

        /* Persist session to disk */
        if (!save_session(session)) {
          printf("\n  [WARN] Could not save session file.\n");
        }

        clear_screen();
        print_banner();
        print_separator();
        printf("\n  ✔  Login successful!\n");
        printf("     Welcome back, %s  (ID: %d)\n", session->username,
               session->user_id);
        print_separator();
        printf("\n  Press ENTER to enter the dashboard...");
        flush_input();
        return 1;
      }
    }

    attempts++;
    printf("\n");
  }

  /* ── Locked Out ── */
  printf("\n  ✖  Too many failed attempts. Access denied.\n");
  printf("     Please try again later.\n");
  printf("\n  Press ENTER to return...");
  flush_input();
  return 0;
}

/* ══════════════════════════════════════════════════════════════
   MAIN AUTHENTICATION MENU
   ══════════════════════════════════════════════════════════════ */

/*
 * auth_menu – drives the pre-login loop.
 *
 * Returns:  1  → user authenticated  (session is populated)
 *           0  → user chose to exit
 */
int auth_menu(Session *session) {
  User users[MAX_USERS];
  int user_count = 0;

  load_users(users, &user_count); /* load existing accounts */

  int choice;

  while (1) {
    clear_screen();
    print_banner();

    printf("  ┌─────────────────────────────────────────┐\n");
    printf("  │              MAIN  MENU                 │\n");
    printf("  ├─────────────────────────────────────────┤\n");
    printf("  │  [1]  Register                          │\n");
    printf("  │  [2]  Login                             │\n");
    printf("  │  [3]  Exit                              │\n");
    printf("  └─────────────────────────────────────────┘\n\n");
    printf("  Enter choice: ");

    if (scanf("%d", &choice) != 1) {
      flush_input();
      continue;
    }
    flush_input(); /* consume leftover newline */

    switch (choice) {

    case 1:
      register_user(users, &user_count);
      break;

    case 2:
      if (login_user(users, user_count, session))
        return 1; /* ← authenticated: hand control to dashboard */
      break;

    case 3:
      clear_screen();
      print_banner();
      printf("  Thank you for using TaskMaster. Goodbye!\n\n");
      return 0; /* ← user chose to exit */

    default:
      printf("\n  [!] Invalid option. Please enter 1, 2, or 3.\n");
      printf("  Press ENTER to continue...");
      flush_input();
      break;
    }
  }
}