#define MAX_WORKSPACE_NAME_LEN 50
#define MAX_WORKSPACE_DESC_LEN 200
#define MAX_WORKSPACE_CODE_LEN 10
#define MAX_WORKSPACE_MEMBERS_COUNT 20

typedef struct {
  int id;
  char name[MAX_WORKSPACE_NAME_LEN];
  char description[MAX_WORKSPACE_DESC_LEN];
  char code[MAX_WORKSPACE_CODE_LEN];
  int owner_id;
  int member_ids[MAX_WORKSPACE_MEMBERS_COUNT];
} Workspace;