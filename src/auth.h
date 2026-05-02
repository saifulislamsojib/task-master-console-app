#include "auth.structure.h"

/* Session Persistence */
int load_session(Session *session);
void clear_session_file(void);

/* Main Auth Menu */
int auth_menu(Session *session);