#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#define CLEAR "cls"
#else
#define CLEAR "clear"
#endif

void clear_screen(void) { system(CLEAR); }

void print_banner(void) {
  printf("\n");
  printf("  ╔══════════════════════════════════════════════╗\n");
  printf("  ║      M A S T E R                             ║\n");
  printf("  ║      Project & Task Management System        ║\n");
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

// validate date formate (DD/MM/YYYY)
int is_valid_date_formate(const char *date) {
  int day, month, year;
  if (sscanf(date, "%d/%d/%d", &day, &month, &year) != 3)
    return 0;

  if (year < 1900 || year > 2100)
    return 0;

  if (month < 1 || month > 12)
    return 0;

  int days_in_month[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

  // leap year check for February
  if (month == 2 && (year % 4 == 0 && year % 100 != 0 || year % 400 == 0)) {
    days_in_month[2] = 29;
  }

  if (day < 1 || day > days_in_month[month]) {
    return 0;
  }

  return 1;
}