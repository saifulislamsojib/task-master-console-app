#define MAX_SPRINT_NAME_LEN 50
#define MAX_SPRINT_DESC_LEN 200

typedef struct {
  int id;
  char name[MAX_SPRINT_NAME_LEN];
  char description[MAX_SPRINT_DESC_LEN];
  char start_date[11];
  char end_date[11];
  int workspace_id;
} Sprint;