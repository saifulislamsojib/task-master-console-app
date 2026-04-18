#define MAX_SPRINT_NAME_LEN 50
#define MAX_SPRINT_DESC_LEN 200

typedef struct {
  int sprint_id;
  char name[MAX_SPRINT_NAME_LEN];
  char description[MAX_SPRINT_DESC_LEN];
  char start_date[20];
  char end_date[20];
  int workspace_id;
} Sprint;