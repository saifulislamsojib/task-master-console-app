#define MAX_TASK_TITLE_LEN 50
#define MAX_TASK_DESC_LEN 200

typedef struct {
  int task_id;
  char title[MAX_TASK_TITLE_LEN];
  char description[MAX_TASK_DESC_LEN];
  char due_date[20];
  int sprint_id;
  int assigned_to;
  int status;
  int priority;
  int created_by;
  int created_at;
  int updated_at;
  int workspace_id;
} Task;