#include "utils.h"
#include "workspace.structure.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <time.h>

#define WORKSPACE_FILE "data/workspaces.dat"
#define MAX_USER_WORKSPACES 100

int generate_workspace_id() {
  FILE *fp = fopen(WORKSPACE_FILE, "rb");
  if (fp == NULL) {
    return 1;
  }
  Workspace workspace;
  int max_id = 0;
  while (fread(&workspace, sizeof(Workspace), 1, fp)) {
    if (workspace.id > max_id) {
      max_id = workspace.id;
    }
  }
  fclose(fp);
  return max_id + 1;
}

char *generate_workspace_code() {
  char *code = malloc(7 * sizeof(char));
  // srand(time(NULL));
  for (int i = 0; i < 6; i++) {
    code[i] = 'A' + (rand() % 26);
  }
  code[6] = '\0';

  // check if code already exists
  FILE *fp = fopen(WORKSPACE_FILE, "rb");
  if (fp == NULL) {
    return code;
  }
  Workspace workspace;
  while (fread(&workspace, sizeof(Workspace), 1, fp)) {
    if (strcmp(workspace.code, code) == 0) {
      return generate_workspace_code();
    }
  }
  fclose(fp);
  return code;
}

int count_user_workspaces(int user_id) {
  FILE *fp = fopen(WORKSPACE_FILE, "rb");
  if (fp == NULL) {
    return 0;
  }
  Workspace workspace;
  int workspace_count = 0;
  while (fread(&workspace, sizeof(Workspace), 1, fp)) {
    for (int i = 0; i < MAX_WORKSPACE_MEMBERS_COUNT; i++) {
      if (workspace.member_ids[i] == user_id) {
        workspace_count++;
        break;
      }
    }
  }
  fclose(fp);
  return workspace_count;
}

int create_workspace(Workspace *workspace, int user_id) {
  int workspace_count = count_user_workspaces(user_id);
  if (workspace_count >= MAX_USER_WORKSPACES) {
    printf("You have reached the maximum number of workspaces!\n");
    return 0;
  }

  FILE *fp = fopen(WORKSPACE_FILE, "ab");
  if (fp == NULL) {
    printf("Error opening workspace file!\n");
    return 0;
  }

  memset(workspace, 0, sizeof(Workspace));

  printf("Enter workspace name: ");
  fgets(workspace->name, sizeof(workspace->name), stdin);
  workspace->name[strcspn(workspace->name, "\n")] = 0;

  printf("Enter workspace description: ");
  fgets(workspace->description, sizeof(workspace->description), stdin);
  workspace->description[strcspn(workspace->description, "\n")] = 0;

  workspace->id = generate_workspace_id();
  strcpy(workspace->code, generate_workspace_code());
  workspace->owner_id = user_id;
  workspace->member_ids[0] = user_id;

  printf("workspace->member_ids[0] is = %d\n", workspace->member_ids[0]);

  fwrite(workspace, sizeof(Workspace), 1, fp);
  fclose(fp);

  printf("Workspace created successfully!\n");
  return 1;
}

int join_workspace(Workspace *workspace, int user_id) {
  int workspace_count = count_user_workspaces(user_id);
  if (workspace_count >= MAX_USER_WORKSPACES) {
    printf("You have reached the maximum number of workspaces!\n");
    return 0;
  }

  FILE *fp = fopen(WORKSPACE_FILE, "rb");
  if (fp == NULL) {
    printf("No workspaces found!\n");
    return 0;
  }

  char code[MAX_WORKSPACE_CODE_LEN];

  printf("Enter workspace code: ");
  fgets(code, sizeof(code), stdin);
  code[strcspn(code, "\n")] = 0;

  memset(workspace, 0, sizeof(Workspace));

  while (fread(workspace, sizeof(Workspace), 1, fp)) {
    if (strcmp(workspace->code, code) == 0) {
      for (int i = 0; i < MAX_WORKSPACE_MEMBERS_COUNT; i++) {
        if (workspace->member_ids[i] == user_id) {
          fclose(fp);
          memset(workspace, 0, sizeof(Workspace));
          printf("You are already a member of this workspace!\n");
          return 0;
        }
        if (workspace->member_ids[i] == 0) {
          workspace->member_ids[i] = user_id;
          fwrite(workspace, sizeof(Workspace), 1, fp);
          fclose(fp);
          printf("Workspace joined successfully!\n");
          return 1;
        }
      }
      fclose(fp);
      memset(workspace, 0, sizeof(Workspace));
      printf("Workspace full!\n");
      return 0;
    }
  }
  fclose(fp);
  memset(workspace, 0, sizeof(Workspace));
  printf("Workspace not found!\n");
  return 0;
}

int view_workspaces(Workspace *workspace, int user_id) {
  FILE *fp = fopen(WORKSPACE_FILE, "rb");
  if (fp == NULL) {
    printf("Error opening workspace file!\n");
    return 0;
  }
  int workspace_index = -1;
  Workspace workspaces[MAX_USER_WORKSPACES];
  printf("\n=================================================\n");
  printf("%-5s %-20s %-10s\n", "ID", "Name", "Code");
  printf("=================================================\n");
  while (fread(workspace, sizeof(Workspace), 1, fp)) {
    for (int i = 0; i < MAX_WORKSPACE_MEMBERS_COUNT; i++) {
      if (workspace->member_ids[i] == user_id) {
        workspace_index++;
        workspaces[workspace_index] = *workspace;
        printf("%-5d %-20s %-10s\n", workspace->id, workspace->name,
               workspace->code);
        break;
      }
    }
  }
  printf("=================================================\n");
  fclose(fp);
  if (workspace_index == -1) {
    printf("You are not a member of any workspace!\n");
    return 0;
  } else {
    printf("Enter workspace ID to view: ");
    int workspace_id;
    if (scanf("%d", &workspace_id)) {
      for (int i = 0; i <= workspace_index; i++) {
        if (workspaces[i].id == workspace_id) {
          *workspace = workspaces[i];
          return 1;
        }
      }
      printf("Workspace not found! Press ENTER to try again.\n");
      flush_input();
    }
    return view_workspaces(workspace, user_id);
  }
}