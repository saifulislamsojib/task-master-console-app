#define TASKK_H

typedef struct {
  char title[50];
  char description[200];
  char due_date[20];
  int status;
  int priority;
} Task;
