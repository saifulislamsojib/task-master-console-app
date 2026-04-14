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