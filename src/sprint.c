#include "sprint.structure.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

#define SPRINT_FILE "data/sprints.dat"

int generate_sprint_id() {
  FILE *fp = fopen(SPRINT_FILE, "rb");
  if (fp == NULL) {
    return 1;
  }

  Sprint sprint;
  int max_id = 0;
  while (fread(&sprint, sizeof(Sprint), 1, fp)) {
    if (sprint.id > max_id) {
      max_id = sprint.id;
    }
  }
  fclose(fp);
  return max_id + 1;
}

int create_sprint(Sprint *sprint, int workspace_id) {
  FILE *fp = fopen(SPRINT_FILE, "ab");
  if (fp == NULL) {
    printf("Error opening sprint file!\n");
    return 0;
  }

  memset(sprint, 0, sizeof(Sprint));

  printf("Enter sprint name: ");
  fgets(sprint->name, sizeof(sprint->name), stdin);
  sprint->name[strcspn(sprint->name, "\n")] = 0;

  printf("Enter sprint description: ");
  fgets(sprint->description, sizeof(sprint->description), stdin);
  sprint->description[strcspn(sprint->description, "\n")] = 0;

  printf("Enter start date (DD/MM/YYYY): ");
  fgets(sprint->start_date, sizeof(sprint->start_date), stdin);
  sprint->start_date[strcspn(sprint->start_date, "\n")] = 0;

  flush_input();

  printf("Enter end date (DD/MM/YYYY): ");
  fgets(sprint->end_date, sizeof(sprint->end_date), stdin);
  sprint->end_date[strcspn(sprint->end_date, "\n")] = 0;

  flush_input();

  if (is_valid_date_formate(sprint->start_date) == 0 ||
      is_valid_date_formate(sprint->end_date) == 0) {
    printf("Invalid date format! (DD/MM/YYYY)\n");
    printf("Press Enter to try again\n");
    flush_input();
    return create_sprint(sprint, workspace_id);
  }

  sprint->id = generate_sprint_id();
  sprint->workspace_id = workspace_id;

  fwrite(sprint, sizeof(Sprint), 1, fp);
  fclose(fp);

  printf("\n  [SUCCESS] Sprint created successfully!\n");
  return 1;
}

int update_sprint(Sprint *sprint) {
  printf("[1] Sprint Name: %s\n", sprint->name);
  printf("[2] Sprint Description: %s\n", sprint->description);
  printf("[3] Sprint Start Date: %s\n", sprint->start_date);
  printf("[4] Sprint End Date: %s\n", sprint->end_date);
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
    printf("Enter new sprint name: ");
    fgets(sprint->name, sizeof(sprint->name), stdin);
    sprint->name[strcspn(sprint->name, "\n")] = 0;
    break;
  case 2:
    printf("Enter new sprint description: ");
    fgets(sprint->description, sizeof(sprint->description), stdin);
    sprint->description[strcspn(sprint->description, "\n")] = 0;
    break;
  case 3:
    printf("Enter new sprint start date (DD/MM/YYYY): ");
    fgets(sprint->start_date, sizeof(sprint->start_date), stdin);
    sprint->start_date[strcspn(sprint->start_date, "\n")] = 0;

    flush_input();

    if (is_valid_date_formate(sprint->start_date) == 0) {
      printf("Invalid date format! (DD/MM/YYYY)\n");
      printf("Press Enter to try again\n");
      flush_input();
      return update_sprint(sprint);
    }
    break;
  case 4:
    printf("Enter new sprint end date (DD/MM/YYYY): ");
    fgets(sprint->end_date, sizeof(sprint->end_date), stdin);
    sprint->end_date[strcspn(sprint->end_date, "\n")] = 0;

    flush_input();

    if (is_valid_date_formate(sprint->end_date) == 0) {
      printf("Invalid date format! (DD/MM/YYYY)\n");
      printf("Press Enter to try again\n");
      flush_input();
      return update_sprint(sprint);
    }
    break;
  default:
    printf("Invalid choice!\n");
    printf("Press Enter to try again\n");
    flush_input();
    return update_sprint(sprint);
  }

  FILE *fp = fopen(SPRINT_FILE, "rb+");

  if (fp == NULL) {
    printf("Error opening sprint file!\n");
    printf("Press Enter to go back\n");
    flush_input();
    return 0;
  }

  Sprint temp_sprint;
  while (fread(&temp_sprint, sizeof(Sprint), 1, fp)) {
    if (temp_sprint.id == sprint->id) {
      fseek(fp, -sizeof(Sprint), SEEK_CUR);
      fwrite(sprint, sizeof(Sprint), 1, fp);
      printf(" [SUCCESS] Sprint updated successfully!\n");
      fclose(fp);
      printf("Press Enter to continue updating\n");
      flush_input();
      return update_sprint(sprint);
    }
  }
  fclose(fp);
  printf("Press Enter to go back\n");
  flush_input();
  return 0;
}

int view_sprints(int workspace_id, int is_edit) {
  Sprint *sprint = NULL;
  FILE *fp = fopen(SPRINT_FILE, "rb");
  if (fp == NULL) {
    printf("Error opening sprint file!\n");
    return 0;
  }
  int sprint_index = -1;
  Sprint sprints[100];
  printf("\n=================================================\n");
  printf("%-5s %-20s %-10s %-10s\n", "ID", "Name", "Start Date", "End Date");
  printf("=================================================\n");
  while (fread(sprint, sizeof(Sprint), 1, fp)) {
    if (sprint->workspace_id == workspace_id) {
      sprint_index++;
      sprints[sprint_index] = *sprint;
      printf("%-5d %-20s %-10s %-10s\n", sprint->id, sprint->name,
             sprint->start_date, sprint->end_date);
    }
  }
  printf("=================================================\n");
  fclose(fp);
  if (sprint_index == -1) {
    printf("No sprints found in this workspace!\n");
  } else if (is_edit) {
    printf("[0] Back\n");
    printf("Enter sprint ID to edit: ");
    int sprint_id;
    scanf("%d", &sprint_id);
    if (sprint_id == 0) {
      return 0;
    }
    for (int i = 0; i <= sprint_index; i++) {
      if (sprints[i].id == sprint_id) {
        *sprint = sprints[i];
        return update_sprint(sprint);
      }
    }
    flush_input();
    printf("Invalid sprint id!\n");
    printf("Press Enter to try again\n");
    flush_input();
    return view_sprints(workspace_id, is_edit);
  }
  flush_input();
  printf("Press Enter to go back\n");
  flush_input();
  return 0;
}

int get_current_date(char *current_date) {
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  strftime(current_date, 10, "%d/%m/%Y", &tm);
  return 1;
}

void get_current_sprint(Sprint *sprint, int workspace_id) {
  FILE *fp = fopen(SPRINT_FILE, "rb");
  if (fp == NULL) {
    printf("Error opening sprint file!\n");
    return;
  }

  char current_date[10];
  get_current_date(current_date);

  Sprint temp_sprint;
  while (fread(&temp_sprint, sizeof(Sprint), 1, fp)) {
    if (temp_sprint.workspace_id == workspace_id) {
      if (strcmp(temp_sprint.start_date, current_date) <= 0 &&
          strcmp(temp_sprint.end_date, current_date) >= 0) {
        *sprint = temp_sprint;
        return;
      }
    }
  }
  fclose(fp);
}