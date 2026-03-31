#include "auth.h"

/*
 * dashboard_placeholder – stub replaced by the full project/task
 * dashboard in Phase 2.
 *
 * Returns: 1 → logout (go back to auth menu)
 *          0 → exit program entirely
 */
static int dashboard_placeholder(Session *session) {
  clear_screen();
  print_banner();

  printf("  ╔══════════════════════════════════════════════╗\n");
  printf("  ║            PROJECT  DASHBOARD               ║\n");
  printf("  ╚══════════════════════════════════════════════╝\n\n");

  printf("  Logged in as : %s\n", session->username);
  printf("  User ID      : %d\n\n", session->user_id);

  print_separator();
  printf("  [Dashboard functionality coming in Phase 2]\n");
  print_separator();

  printf("\n  Options:\n");
  printf("   [1]  Logout\n");
  printf("   [2]  Exit  (session saved – restored on next run)\n\n");
  printf("  Enter choice: ");
  fflush(stdout);

  int choice;
  if (scanf("%d", &choice) != 1)
    choice = 1;
  flush_input();

  if (choice == 2) {
    /* Exit without clearing session – will be restored next run */
    clear_screen();
    print_banner();
    printf("  Session saved. See you next time, %s!\n\n", session->username);
    return 0;
  }

  /* Logout → delete session file so next run starts fresh */
  clear_session_file();
  printf("\n  Session cleared. Logged out successfully.\n");
  return 1;
}

/* ─── Entry Point ─────────────────────────────────────────── */
int main(void) {
  Session session;
  memset(&session, 0, sizeof(Session));

  /* ── Try to restore a saved session on startup ── */
  if (load_session(&session)) {
    clear_screen();
    print_banner();
    print_session_info(&session);
    flush_input();

    /* Jump straight to dashboard with the restored session */
    int result = dashboard_placeholder(&session);
    if (result == 0)
      return 0; /* user chose Exit */
    memset(&session, 0, sizeof(Session));
    /* Fell through logout → drop into auth menu below */
  }

  /* ── Normal auth loop ── */
  while (1) {
    memset(&session, 0, sizeof(Session));

    int auth_result = auth_menu(&session);
    if (auth_result == 0)
      break; /* user chose Exit from menu */

    int result = dashboard_placeholder(&session);
    if (result == 0)
      break; /* user chose Exit from dashboard */
             /* result == 1 means logout → loop back to auth menu */
  }

  return 0;
}