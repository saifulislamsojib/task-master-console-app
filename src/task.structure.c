#define MAX_WORKSPACE_NAME_LEN 50
#define MAX_WORKSPACE_DESC_LEN 200
#define MAX_WORKSPACE_CODE_LEN 10

typedef struct {
  int task_id;
  char title[MAX_WORKSPACE_NAME_LEN];
  char description[MAX_WORKSPACE_DESC_LEN];
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