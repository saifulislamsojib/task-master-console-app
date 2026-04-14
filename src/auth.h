#include "auth.structure.h"

/* Session Persistence */
int load_session(Session *session);
void clear_session_file(void);
void print_session_info(const Session *session);

/* Main Auth Menu */
int auth_menu(Session *session);