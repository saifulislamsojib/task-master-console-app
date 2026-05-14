#include "task.structure.h"
#include "workspace.structure.h"

int create_task(int workspace_id, int sprint_id, int user_id);
int view_my_tasks(int workspace_id, int user_id,
                  int member_ids[MAX_WORKSPACE_MEMBERS_COUNT]);
int view_all_tasks(int workspace_id,
                   int member_ids[MAX_WORKSPACE_MEMBERS_COUNT], int is_edit);