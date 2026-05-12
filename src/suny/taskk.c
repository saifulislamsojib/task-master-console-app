#include "taskk.h"
#include <stdio.h>

#define TASK_FILE "data/tasks.dat"

int add_task() {

  FILE *fp = fopen(TASK_FILE, "ab");

  if (fp == NULL) {
    printf("File open error!\n");
    return 0;
  }

  Task task;

  getchar();

  printf("\nEnter Task Title: ");
  fgets(task.title, 50, stdin);
  
  printf("Enter Description: ");
  fgets(task.description, 200, stdin);

  printf("Enter Due Date (dd-mm-yyyy): ");
  fgets(task.due_date, 20, stdin);

  printf("Enter Status (0 Pending, 1 In Progress, 2 Done): ");
  scanf("%d", &task.status);

  printf("Enter Priority (0 Low, 1 Medium, 2 High): ");
  scanf("%d", &task.priority);

  fwrite(&task, sizeof(Task), 1, fp);

  fclose(fp);

  printf("\nTask Added Successfully!\n");

  return 1;
}