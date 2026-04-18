#include "dashboard.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>

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
    int result = dashboard(&session);
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

    int result = dashboard(&session);
    if (result == 0)
      break; /* user chose Exit from dashboard */
             /* result == 1 means logout → loop back to auth menu */
  }

  return 0;
}