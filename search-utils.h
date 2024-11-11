int len(char str[]);
int lower(char *str, char *ret);
int sanitize(char *str, char *ret);
int in(char s, char *li);
int strip(char *str, char *ret);
int strong_sanitize(char *str, char *ret);
int copy_str(char *str, char *copy);
int hash_string(char *str);
int count_char(char *str, char ch);
int str_split(char *str, char delim, char **l);
int str_equal(char* s1, char* s2);
int* in_where(int* l, int x, int length);
int bubble2dsort(float** unsorted, int l);
int swap(float** x, int i, int j);
int in_str_list(char* str, char** l, int len_l);