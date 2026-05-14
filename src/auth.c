#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "auth.lib.h"
#include "auth.structure.h"
#include "utils.h"

// /* ─── Constants ─────────────────────────────────────────────── */
#define MAX_USERS 100
#define USER_DB_FILE "data/users.dat"
#define SESSION_FILE "data/session.dat"
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

/*
 * save_session – writes the session to session.dat.
 * Returns 1 on success, 0 on failure.
 */
int save_session(const Session *session) {
  FILE *fp = fopen(SESSION_FILE, "wb");
  if (!fp)
    return 0;
  fwrite(session, sizeof(Session), 1, fp);
  fclose(fp);
  return 1;
}

/*
 * load_session – reads session.dat, decrypts.
 * Populates *session and returns 1 if a valid session exists.
 * Returns 0 if no file or file is corrupt.
 */
int load_session(Session *session) {
  FILE *fp = fopen(SESSION_FILE, "rb");
  if (!fp)
    return 0;

  if (fread(session, sizeof(Session), 1, fp) != 1) {
    fclose(fp);
    return 0;
  }
  fclose(fp);
  return 1;
}

/*
 * clear_session_file – deletes session.dat (called on logout).
 */
void clear_session_file(void) { remove(SESSION_FILE); }

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
 *   - At least one lowercase letter
 *   - At least one special character
 */
int is_valid_password(const char *password) {
  int len = (int)strlen(password);
  if (len < 6 || len >= MAX_PASSWORD_LEN)
    return 0;

  int has_digit = 0;
  int has_upper = 0;
  int has_lower = 0;
  int has_special = 0;

  for (int i = 0; i < len; i++) {
    char pass = (unsigned char)password[i];
    if (isdigit(pass))
      has_digit = 1;
    if (isupper(pass))
      has_upper = 1;
    if (islower(pass))
      has_lower = 1;
    if (ispunct(pass))
      has_special = 1;
  }
  return has_digit && has_upper && has_lower && has_special;
}

/*
 * username_exists - duplicate check.
 */
int username_exists(User users[], int count, const char *username) {
  for (int i = 0; i < count; i++) {
    if (strcmp(users[i].username, username) == 0) {
      return 1;
    }
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
  printf("\n  Password rules: min 6 chars, 1 uppercase, 1 lower, 1 special "
         "char and 1 digit.\n\n");
  while (1) {
    printf("  Password  : ");
    if (!fgets(password, sizeof(password), stdin))
      continue;
    password[strcspn(password, "\n")] = '\0'; /* strip newline */

    if (!is_valid_password(password)) {
      printf("  [!] Password too weak. Try again.\n\n");
      fflush(stdout);
      continue;
    }

    printf("  Confirm   : ");
    if (!fgets(confirm, sizeof(confirm), stdin))
      continue;
    confirm[strcspn(confirm, "\n")] = '\0'; /* strip newline */

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
    if (!fgets(password, sizeof(password), stdin))
      continue;
    password[strcspn(password, "\n")] = '\0'; /* strip newline */

    /* Find matching user */
    int found = -1;
    for (int i = 0; i < count; i++) {
      if (strcmp(users[i].username, username) == 0) {
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

    printf("[1]  Register\n");
    printf("[2]  Login\n");
    printf("[3]  Exit\n");
    printf("\n  Enter choice: ");

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

int get_username(char *username, int user_id) {
  FILE *fp = fopen(USER_DB_FILE, "rb");
  if (fp == NULL) {
    printf("Error opening user file!\n");
    return 0;
  }
  User user;
  while (fread(&user, sizeof(User), 1, fp)) {
    if (user.user_id == user_id) {
      strcpy(username, user.username);
      fclose(fp);
      return 1;
    }
  }
  fclose(fp);
  return 0;
}