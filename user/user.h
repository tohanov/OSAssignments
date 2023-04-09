struct stat;

// system calls
int fork(void);
// int exit(int) __attribute__((noreturn));
// int wait(int*);
int pipe(int*);
int write(int, const void*, int);
int read(int, void*, int);
int close(int);
int kill(int);
int exec(const char*, char**);
int open(const char*, int);
int mknod(const char*, short, short);
int unlink(const char*);
int fstat(int fd, struct stat*);
int link(const char*, const char*);
int mkdir(const char*);
int chdir(const char*);
int dup(int);
int getpid(void);
char* sbrk(int);
int sleep(int);
int uptime(void);

// added for assignments 
int memsize(void); // as1ts2
int exit(int, char*) __attribute__((noreturn)); // as1ts3
int wait(int*, char*); // as1ts3
void set_ps_priority(int); // as1ts5
int get_ps_priority(void); // as1ts5
int set_cfs_priority(int); // as1ts6
void get_cfs_priority(int, uint64*); // as1ts6
// =====================


// ulib.c
int stat(const char*, struct stat*);
char* strcpy(char*, const char*);
void *memmove(void*, const void*, int);
char* strchr(const char*, char c);
int strcmp(const char*, const char*);
void fprintf(int, const char*, ...);
void printf(const char*, ...);
char* gets(char*, int max);
uint strlen(const char*);
void* memset(void*, int, uint);
void* malloc(uint);
void free(void*);
int atoi(const char*);
int memcmp(const void *, const void *, uint);
void *memcpy(void *, const void *, uint);
