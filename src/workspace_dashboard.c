#include "utils.h"
#include "workspace.structure.h"
#include <stdio.h>

int workspace_dashboard(Workspace *workspace, int user_id) {
  clear_screen();
  print_banner();

  printf("  ┌─────────────────────────────────────────┐\n");
  printf("  │           WORKSPACE DASHBOARD           │\n");
  printf("  └─────────────────────────────────────────┘\n\n");

  printf("  Workspace: %s\n", workspace->name);
  printf("  Description: %s\n", workspace->description);
  printf("  Code: %s\n", workspace->code);
  printf("  Owner: %s\n",
         workspace->owner_id == user_id ? "You" : "Another User");

  for (int i = 0; i < MAX_WORKSPACE_MEMBERS_COUNT; i++) {
    printf("  Member: %d\n", workspace->member_ids[i]);
  }
  printf("\n");

  printf("  1. View Tasks\n");
  printf("  2. Create Task\n");
  printf("  3. Leave Workspace\n");
  printf("  4. Back to Dashboard\n");
  printf("\n");
  flush_input();
  return 0;
  // int choice;
  // printf("  Enter your choice: ");
  // if (scanf("%d", &choice)) {
  //   switch (choice) {
  //   case 1:
  //     if (!view_tasks(workspace, user_id)) {
  //       printf("\n  Press ENTER to go back to workspace dashboard...");
  //       flush_input();
  //       return workspace_dashboard(workspace, user_id);
  //     }
  //     break;
  //   case 2:
  //     if (!create_task(workspace, user_id)) {
  //       printf("\n  Press ENTER to go back to workspace dashboard...");
  //       flush_input();
  //       return workspace_dashboard(workspace, user_id);
  //     }
  //     break;
  //   case 3:
  //     if (!leave_workspace(workspace, user_id)) {
  //       printf("\n  Press ENTER to go back to workspace dashboard...");
  //       flush_input();
  //       return workspace_dashboard(workspace, user_id);
  //     }
  //     break;
  //   case 4:
  //     return dashboard(session);
  //   default:
  //     printf("  Invalid choice. Please try again.\n");
  //     return workspace_dashboard(workspace, user_id);
  //   }
  // }
  //   return workspace_dashboard(workspace, user_id);
}