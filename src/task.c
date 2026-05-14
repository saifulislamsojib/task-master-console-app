#include "auth.h"
#include "task.structure.h"
#include "utils.h"
#include "workspace.structure.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

#define TASK_FILE "data/tasks.dat"

char task_status[6][20] = {
    "BACKLOG", "TODO", "IN PROGRESS", "QA/TESTING", "REVIEW", "DONE",
};
char task_priority[3][20] = {"LOW", "MEDIUM", "HIGH"};

int generate_task_id() {
  FILE *fp = fopen(TASK_FILE, "rb");
  if (fp == NULL) {
    return 1;
  }

  Task task;
  int max_id = 0;
  while (fread(&task, sizeof(Task), 1, fp)) {
    if (task.id > max_id) {
      max_id = task.id;
    }
  }
  fclose(fp);
  return max_id + 1;
}

void view_task(Task *task, int member_ids[MAX_WORKSPACE_MEMBERS_COUNT]) {
  printf("[1] Task Title: %s\n", task->title);
  printf("[2] Task Description: %s\n", task->description);
  printf("[3] Task Due Date: %s\n", task->due_date);
  if (task->assigned_to == -1) {
    printf("[4] Task Assigned To: %s\n", "Not Assigned");
  } else {
    char username[MAX_USERNAME_LEN];
    get_username(username, task->assigned_to);
    printf("[4] Task Assigned To: %s\n", username);
  }
  printf("[5] Task Priority: %s\n", task_priority[task->priority]);
  printf("[6] Task Status: %s\n", task_status[task->status]);

  printf("[7] Task Tags: ");
  for (int i = 0; i < 5; i++) {
    printf("%s ", task->tags[i]);
  }
  printf("\n");
}

int update_task(Task *task, int member_ids[MAX_WORKSPACE_MEMBERS_COUNT]) {
  view_task(task, member_ids);
  printf("[0] Back\n");

  printf("Enter your choice to update: ");
  int choice;
  scanf("%d", &choice);
  flush_input();

  if (choice == 0) {
    return 0;
  }

  switch (choice) {
  case 1:
    printf("Enter new task title: ");
    fgets(task->title, sizeof(task->title), stdin);
    task->title[strcspn(task->title, "\n")] = 0;
    break;
  case 2:
    printf("Enter new task description: ");
    fgets(task->description, sizeof(task->description), stdin);
    task->description[strcspn(task->description, "\n")] = 0;
    break;
  case 3:
    printf("Enter new task due date (DD/MM/YYYY): ");
    fgets(task->due_date, sizeof(task->due_date), stdin);
    task->due_date[strcspn(task->due_date, "\n")] = 0;

    flush_input();

    if (is_valid_date_formate(task->due_date) == 0) {
      printf("Invalid due date format! (DD/MM/YYYY)\n");
      printf("Press Enter to try again\n");
      flush_input();
      return update_task(task, member_ids);
    }

    break;
  case 4: {
    int count;
    char *usernames[MAX_WORKSPACE_MEMBERS_COUNT];
    get_usernames_by_ids(usernames, member_ids, &count);

    for (int i = 0; i < count; i++) {
      printf("  [%d] %s\n", member_ids[i], usernames[i]);
    }
    printf("  [%d] Unassigned\n", -1);

    printf("Enter new task assigned to (ID): ");

    int assigned_to;
    scanf("%d", &assigned_to);

    int valid = 0;
    for (int i = 0; i < count; i++) {
      if (member_ids[i] == assigned_to) {
        task->assigned_to = assigned_to;
        valid = 1;
        break;
      }
    }

    if (valid == 0) {
      printf("Invalid user ID!\n");
      printf("Press Enter to try again\n");
      flush_input();
      return update_task(task, member_ids);
    }
    break;
  }
  case 5:
    printf("Enter new task priority (1-3): ");
    for (int i = 0; i < 3; i++) {
      printf("  %d. %s\n", i + 1, task_priority[i]);
    }
    scanf("%d", &task->priority);
    if (task->priority < 1 || task->priority > 3) {
      printf("Invalid priority!\n");
      printf("Press Enter to try again\n");
      flush_input();
      return update_task(task, member_ids);
    }
    task->priority--;
    break;
  case 6:
    printf("Enter new task status (1-6): ");
    for (int i = 0; i < 6; i++) {
      printf("  %d. %s\n", i + 1, task_status[i]);
    }
    scanf("%d", &task->status);
    if (task->status < 1 || task->status > 6) {
      printf("Invalid status!\n");
      printf("Press Enter to try again\n");
      flush_input();
      return update_task(task, member_ids);
    }
    task->status--;
    break;
  case 7:
    printf("Enter new task tags (one by one, enter blank to stop): \n");
    for (int i = 0; i < 5; i++) {
      printf("  %d. ", i + 1);
      fgets(task->tags[i], sizeof(task->tags[i]), stdin);
      task->tags[i][strcspn(task->tags[i], "\n")] = 0;
      if (strlen(task->tags[i]) == 0) {
        break;
      }
    }
    break;
  default:
    printf("Invalid choice!\n");
    printf("Press Enter to try again\n");
    flush_input();
    return update_task(task, member_ids);
  }

  FILE *fp = fopen(TASK_FILE, "rb+");

  if (fp == NULL) {
    printf("Error opening task file!\n");
    printf("Press Enter to go back\n");
    flush_input();
    return 0;
  }

  Task temp_task;
  while (fread(&temp_task, sizeof(Task), 1, fp)) {
    if (temp_task.id == task->id) {
      fseek(fp, -sizeof(Task), SEEK_CUR);
      fwrite(task, sizeof(Task), 1, fp);
      printf(" [SUCCESS] Task updated successfully!\n");
      fclose(fp);
      printf("Press Enter to continue updating\n");
      flush_input();
      return update_task(task, member_ids);
    }
  }
  fclose(fp);
  printf("Press Enter to go back\n");
  flush_input();
  return 0;
}

int create_task(int workspace_id, int sprint_id, int user_id) {
  Task task;
  FILE *fp = fopen(TASK_FILE, "ab");
  if (fp == NULL) {
    printf("Error opening task file!\n");
    return 0;
  }

  memset(&task, 0, sizeof(Task));

  printf("Enter task title: ");
  fgets(task.title, sizeof(task.title), stdin);
  task.title[strcspn(task.title, "\n")] = 0;

  printf("Enter task description: ");
  fgets(task.description, sizeof(task.description), stdin);
  task.description[strcspn(task.description, "\n")] = 0;

  printf("Enter task due date (DD/MM/YYYY): ");
  fgets(task.due_date, sizeof(task.due_date), stdin);
  task.due_date[strcspn(task.due_date, "\n")] = 0;

  flush_input();

  if (is_valid_date_formate(task.due_date) == 0) {
    printf("Invalid due date format! (DD/MM/YYYY)\n");
    printf("Press Enter to try again\n");
    flush_input();
    fclose(fp);
    return create_task(workspace_id, sprint_id, user_id);
  }

  printf("\n[Task Priority]\n");
  for (int i = 0; i < 3; i++) {
    printf("  %d. %s\n", i + 1, task_priority[i]);
  }
  printf("Enter task priority (1-3): ");
  scanf("%d", &task.priority);

  if (task.priority < 1 || task.priority > 3) {
    printf("Invalid priority!\n");
    printf("Press Enter to try again\n");
    flush_input();
    fclose(fp);
    return create_task(workspace_id, sprint_id, user_id);
  }
  task.priority--;

  printf("\n[Task Status]\n");
  for (int i = 0; i < 6; i++) {
    printf("  %d. %s\n", i + 1, task_status[i]);
  }
  printf("Enter task status (1-6): ");
  scanf("%d", &task.status);

  if (task.status < 1 || task.status > 6) {
    printf("Invalid status!\n");
    printf("Press Enter to try again\n");
    flush_input();
    fclose(fp);
    return create_task(workspace_id, sprint_id, user_id);
  }
  task.status--;

  flush_input();

  printf("Enter task tags (one by one, enter blank to stop): \n");
  for (int i = 0; i < 5; i++) {
    printf("  %d. ", i + 1);
    fgets(task.tags[i], sizeof(task.tags[i]), stdin);
    task.tags[i][strcspn(task.tags[i], "\n")] = 0;
    if (strlen(task.tags[i]) == 0) {
      break;
    }
  }

  task.id = generate_task_id();
  task.workspace_id = workspace_id;
  task.sprint_id = sprint_id;
  task.created_by = user_id;
  time_t t = time(NULL);
  task.created_at = t;
  task.updated_at = t;
  task.assigned_to = -1;

  fwrite(&task, sizeof(Task), 1, fp);
  fclose(fp);

  printf("\n  [SUCCESS] Task created successfully!\n");
  printf("Press Enter to continue\n");
  flush_input();
  return 1;
}

int update_task_user(Task *task, int member_ids[MAX_WORKSPACE_MEMBERS_COUNT]) {
  view_task(task, member_ids);

  print_separator();

  printf("\n  [1] Update Task Status\n");
  printf("  [0] Back\n");

  int choice;
  scanf("%d", &choice);
  flush_input();

  if (choice == 0) {
    return 0;
  }

  if (choice == 1) {
    printf("Enter new task status (1-6): ");
    for (int i = 0; i < 6; i++) {
      printf("  %d. %s\n", i + 1, task_status[i]);
    }
    scanf("%d", &task->status);
    if (task->status < 1 || task->status > 6) {
      printf("Invalid status!\n");
      printf("Press Enter to try again\n");
      flush_input();
      return update_task_user(task, member_ids);
    }
    task->status--;

    FILE *fp = fopen(TASK_FILE, "rb+");
    if (fp == NULL) {
      printf("Error opening task file!\n");
      return 0;
    }

    Task temp_task;
    while (fread(&temp_task, sizeof(Task), 1, fp)) {
      if (temp_task.id == task->id) {
        fseek(fp, -sizeof(Task), SEEK_CUR);
        fwrite(task, sizeof(Task), 1, fp);
        printf(" [SUCCESS] Task status updated successfully!\n");
        fclose(fp);
        printf("Press Enter to continue updating\n");
        flush_input();
        return update_task_user(task, member_ids);
      }
    }
    fclose(fp);
    printf(" [ERROR] Task not found!\n");
    printf("Press Enter to try again\n");
    flush_input();
    return update_task_user(task, member_ids);
  }

  printf("Invalid choice!\n");
  printf("Press Enter to try again\n");
  flush_input();
  return update_task_user(task, member_ids);
}

int view_my_tasks(int user_id) {
  FILE *fp = fopen(TASK_FILE, "rb");
  if (fp == NULL) {
    printf("Error opening task file!\n");
    return 0;
  }

  Task task;
  int found = 0;
  while (fread(&task, sizeof(Task), 1, fp)) {
    if (task.assigned_to == user_id) {
      view_task(&task, NULL);
      found = 1;
    }
  }
  fclose(fp);

  if (!found) {
    printf("No tasks assigned to you!\n");
  }

  return found;
}
