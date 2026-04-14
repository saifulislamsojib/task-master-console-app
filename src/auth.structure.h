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

/*
 * SessionRecord – what is physically written to session.dat.
 * Every byte except `magic` is XOR-encrypted with SESSION_XOR_KEY
 * before writing, and decrypted after reading.
 */
typedef struct {
  unsigned int magic; /* SESSION_MAGIC – integrity check  */
  char username[MAX_USERNAME_LEN];
  int user_id;
  long login_time; /* time_t stored as long            */
} SessionRecord;