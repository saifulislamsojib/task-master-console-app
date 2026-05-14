#include "auth.h"
#include "sprint.h"
#include "task.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>

int workspace_dashboard(Workspace *workspace, int user_id);

int Member_Dashboard(Workspace *workspace, int user_id,
                     Sprint *current_sprint) {
  printf("\n");
  printf("  1. View My Tasks In Current Sprint\n");
  printf("  2. View My All Tasks\n");
  printf("  3. View All Tasks In Current Sprint\n");
  printf("  4. View All Sprints\n");
  printf("  5. Back to Main Menu\n");
  printf("\n");
  int choice;
  printf("  Choose an option: ");
  scanf("%d", &choice);
  flush_input();
  // switch (choice) {
  // case 1:
  //   view_my_tasks_current_sprint(workspace, user_id);
  //   break;
  // case 2:
  //   view_my_all_tasks(workspace, user_id);
  //   break;
  // case 3:
  //   view_all_tasks_current_sprint(workspace);
  //   break;
  // case 4:
  //   view_all_sprints(workspace);
  //   break;
  // case 5:
  //   return 0; /* Exit */
  // default:
  //   printf("  [ERROR] Invalid option.\n");
  //   printf("\n  Press ENTER to try again...");
  //   flush_input();
  //   return Member_Dashboard(workspace, user_id);
  // }
  return choice;
}

int Owner_Dashboard(Workspace *workspace, int user_id, Sprint *sprint) {
  printf("\n");
  int no_sprint = sprint->id == 0;
  if (no_sprint) {
    printf("  [1] Create Sprint\n");
  } else {
    printf("  [1] Create Task\n");
  }
  printf("  [2] View All Sprints\n");
  printf("  [3] View All Tasks\n");
  printf("  [4] View My All Tasks\n");
  printf("  [0] Back to Main Menu\n");
  printf("\n");
  int choice;
  printf("  Choose an option: ");
  scanf("%d", &choice);
  flush_input();
  switch (choice) {
  case 0:
    return 0;
  case 1:
    if (no_sprint) {
      create_sprint(sprint, workspace->id);
      return workspace_dashboard(workspace, user_id);
    } else {
      if (create_task(workspace->id, sprint->id, user_id)) {
        return Owner_Dashboard(workspace, user_id, sprint);
      }
      printf("\n  [ERROR] Failed to create task.\n");
      printf("\n  Press ENTER to continue...");
      flush_input();
      return Owner_Dashboard(workspace, user_id, sprint);
    }
  case 2:
    view_sprints(sprint, workspace->id, 1);
    break;
  // case 3:
  //   view_all_tasks_current_sprint(workspace);
  //   break;
  // case 4:
  //   view_all_tasks(workspace);
  //   break;
  default:
    printf("  [ERROR] Invalid option.\n");
    printf("\n  Press ENTER to try again...\n");
    flush_input();
    return Owner_Dashboard(workspace, user_id, sprint);
  }
  return Owner_Dashboard(workspace, user_id, sprint);
}

int workspace_dashboard(Workspace *workspace, int user_id) {
  clear_screen();
  print_banner();

  printf("  ┌─────────────────────────────────────────┐\n");
  printf("  │           WORKSPACE DASHBOARD           │\n");
  printf("  └─────────────────────────────────────────┘\n\n");
  char owner_name[MAX_USERNAME_LEN];
  Sprint current_sprint;
  get_current_sprint(&current_sprint, workspace->id);

  printf("  Workspace: %s\n", workspace->name);
  printf("  Description: %s\n", workspace->description);
  printf("  Code: %s\n", workspace->code);

  int is_owner = workspace->owner_id == user_id;

  if (is_owner) {
    printf("  Owner: You\n");
  } else {
    get_username(owner_name, workspace->owner_id);
    printf("  Owner: %s\n", owner_name);
  }

  printf("\n");

  if (current_sprint.id != 0) {
    printf("  Current Sprint: %s\n", current_sprint.name);
    printf("  Current Sprint Start Date: %s\n", current_sprint.start_date);
    printf("  Current Sprint End Date: %s\n", current_sprint.end_date);
  } else {
    printf("  No current sprint found!\n");
  }

  int choice;
  if (is_owner) {
    choice = Owner_Dashboard(workspace, user_id, &current_sprint);
  } else {
    choice = Member_Dashboard(workspace, user_id, &current_sprint);
  }
  if (choice == 0) {
    return 1;
  }
  return workspace_dashboard(workspace, user_id);
}