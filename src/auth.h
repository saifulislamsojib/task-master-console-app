#include "auth.structure.h"

/* Session Persistence */
int load_session(Session *session);
void clear_session_file(void);

/* Main Auth Menu */
int auth_menu(Session *session);

void get_username(char *username, int user_id);

int get_usernames_by_ids(char *usernames[], int user_ids[], int *count);