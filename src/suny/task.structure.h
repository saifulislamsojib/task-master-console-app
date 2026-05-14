#define MAX_TASK_TITLE_LEN 50
#define MAX_TASK_DESC_LEN 200

typedef struct {
  int id;
  char title[MAX_TASK_TITLE_LEN];
  char description[MAX_TASK_DESC_LEN];
  char due_date[11];
  int status;
  int priority;
  int created_by;
  int created_at;
  int updated_at;
  int assigned_to;
  int workspace_id;
  int sprint_id;
  char tags[5][20];
} Task;