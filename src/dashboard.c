#include "auth.h"
#include "utils.h"
#include "workspace_dashboard.h"
#include <stdio.h>
#include <string.h>

/*
 * dashboard – full project/task
 *
 * Returns: 1 → logout (go back to auth menu)
 *          0 → exit program entirely
 */
int dashboard(Session *session) {
  clear_screen();
  print_banner();

  printf("  ╔══════════════════════════════════════════════╗\n");
  printf("  ║                  DASHBOARD                   ║\n");
  printf("  ╚══════════════════════════════════════════════╝\n\n");

  printf("  Username : %s\n", session->username);
  printf("  User ID  : %d\n\n", session->user_id);

  printf("\n  Options:\n");
  printf("[1] Join a workspace\n");
  printf("[2] Create a workspace\n");
  printf("[3] View your workspaces\n");
  printf("[4] Logout\n");
  printf("[5] Exit (session saved - restored on next run)\n\n");
  printf("Enter choice: ");

  int choice;
  scanf("%d", &choice);
  flush_input();

  Workspace workspace;
  memset(&workspace, 0, sizeof(Workspace));

  switch (choice) {
  case 1:
    if (!join_workspace(&workspace, session->user_id)) {
      printf("\n  Press ENTER to go back to dashboard...");
      flush_input();
      return dashboard(session);
    }
    printf(" Press ENTER to go to workspace dashboard...\n");
    flush_input();
    break;
  case 2:
    if (!create_workspace(&workspace, session->user_id)) {
      printf("\n  Press ENTER to go back to dashboard...");
      flush_input();
      return dashboard(session);
    }
    printf(" Press ENTER to go to workspace dashboard...\n");
    flush_input();
    break;
  case 3:
    if (!view_workspaces(&workspace, session->user_id)) {
      printf("\n  Press ENTER to go back to dashboard...");
      flush_input();
      return dashboard(session);
    }
    break;
  case 4:
    /* Logout → delete session file so next run starts fresh */
    clear_session_file();
    printf("\n  Session cleared. Logged out successfully.\n");
    return 1;
  case 5: /* Exit without clearing session – will be restored next run */
    print_banner();
    printf("  Session saved. See you next time, %s!\n\n", session->username);
    return 0;
  default:
    printf("  Invalid choice. Please try again.\n");
    return dashboard(session);
  }

  choice = workspace_dashboard(&workspace, session->user_id);
  if (choice == 1) {
    return dashboard(session);
  }
  return 0;
}