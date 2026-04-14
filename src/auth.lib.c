#include <ctype.h>
#include <stdio.h>

#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

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

#define XOR_KEY 0x5A /* simple XOR cipher key for passwords   */

void xor_encrypt(char *text, char *result, int len) {
  for (int i = 0; i < len; i++)
    result[i] = text[i] ^ XOR_KEY;
  result[len] = '\0';
}

void xor_decrypt(char *cipher, char *result, int len) {
  /* XOR is symmetric – same operation decrypts */
  xor_encrypt(cipher, result, len);
}