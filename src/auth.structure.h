#define MAX_USERNAME_LEN 32
#define MAX_PASSWORD_LEN 64

typedef struct {
  char username[MAX_USERNAME_LEN];
  char password[MAX_PASSWORD_LEN]; /* stored XOR-encrypted */
  int user_id;
} User;

typedef struct {
  char username[MAX_USERNAME_LEN];
  int user_id;
} Session;