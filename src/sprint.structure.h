#define MAX_WORKSPACE_NAME_LEN 50
#define MAX_WORKSPACE_DESC_LEN 200
#define MAX_WORKSPACE_CODE_LEN 10

typedef struct {
  int sprint_id;
  char name[MAX_WORKSPACE_NAME_LEN];
  char description[MAX_WORKSPACE_DESC_LEN];
  char start_date[20];
  char end_date[20];
  int workspace_id;
} Sprint;