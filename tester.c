/*
 * A very simple backup/restore program - use at your own risk ;)
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdarg.h>

#define CMD_BACKUP 0
#define CMD_RESTORE 1

#define BACKUP_DIRECTORY "/usr/share/backup"
#define FORBIDDEN_DIRECTORY "/etc"

static
int copyFile(char* src, char* dst)
{
  unsigned int i, len;
  char buffer[3000];
  FILE *source, *dest;
  int c;

  source = fopen(src, "r");
  if (source == NULL) {
    fprintf(stderr, "Failed to open source file\n");
    return -1;
  }
  
  i = 0;
  c = fgetc(source);
  while (c != EOF) {
    buffer[i]  = (unsigned char) c;
    c = fgetc(source);
    i++;
  }

  len = i;
  fclose(source);
 
  dest = fopen(dst, "w");
  if (dest == NULL) {
    fprintf(stderr, "Failed to open destination file\n");
    perror("Error");
    return -1;
  }

  for(i = 0; i < len; i++) {
    fputc(buffer[i], dest);
  }

  fclose(dest);

  return 0;
}

static
int run_cmd(char * cmd, ... ) {
  va_list vl;
  char * arg[8];
  pid_t pid;
  int i, status;

  i = 1;
  va_start(vl, cmd);
  while(i < 7 && (arg[i++] = va_arg(vl, char*)));
  va_end(vl);

  arg[7] = NULL;
  arg[0] = rindex(cmd, '/');
  if (arg[0] == NULL)
    arg[0] = cmd;
  else
    arg[0]++;

  pid = fork();
  // error
  if (pid < 0) {
    fprintf(stderr, "Fork failed\n");
    return 1;
  } if (pid > 0) {
    waitpid(pid, &status, 0);
    if (WIFEXITED(status) == 0 || WEXITSTATUS(status) < 0)
      return 1;
  } else {
    execvp(cmd, arg);
    return 1;
  }
  return 0;
}


static
int restorePermissions(char* target)
{
  pid_t pid;
  int status;
  char *user, *userid, *ptr;
  FILE *file;
  char buffer[64];
  mode_t mode;

  // execute "chown" to assign file ownership to user
  pid = fork();
  
  // error
  if (pid < 0) {
    fprintf(stderr, "Fork failed\n");
    return -1;
  }

  // parent
  if (pid > 0) {
    waitpid(pid, &status, 0);
    if (WIFEXITED(status) == 0 || WEXITSTATUS(status) < 0)
      return -1;
  } else {
    // child
    // retrieve username
    user = getenv("USER");
    // retrieve corresponding userid   
    file = fopen("/etc/passwd", "r");
    if (file == NULL) {
      fprintf(stderr, "Failed to open password file\n");
      return -1;
    }

    userid = NULL;
    while (!feof(file)) {
      if (fgets(buffer, sizeof(buffer), file) != NULL) {
        ptr = strtok(buffer, ":");
        if (strcmp(ptr, user) == 0) {
          strtok(NULL, ":"); // password
          userid = strtok(NULL, ":"); // userid
          ptr = strtok(NULL, ":"); // group
          *ptr = '\0';
          break;
        }
      }
    }

    if (userid != NULL) {
      execlp("/bin/chown", "/bin/chown", userid, target, NULL);
    }

    // reached only in case of error
    return -1;
  }  

  mode = S_IRUSR | S_IWUSR | S_IEXEC;
  chmod(target, mode);

  return 0;
}

int show_files() {
  run_cmd("find", BACKUP_DIRECTORY, "-mindepth", "1", NULL);
  return 0;
}

void strip_dots(char* str) {
  char* tmp_str = malloc(strlen(str)+1);
  char* ptr = tmp_str;
  strcpy(ptr, str);
  while (strncmp("../", ptr, 3) == 0) {
    ptr += 3;
  }
  strcpy(str, ptr);
  free(tmp_str);
}

static
void usage(char* parameter) 
{
  char output[200];

  snprintf(output, sizeof(output),
        "Usage: %.110s backup|restore|ls pathname\n", parameter);

//  printf(output);
//  printf("0x%x\n", output);
}

int main(int argc, char* argv[]) 
{
  int cmd;
  int permissionsError = 0;
  char errorMsg[152];
  char *path, *path_stripped, *path_real;
  char *forbidden = FORBIDDEN_DIRECTORY;
  char *src, *dst, *buffer;
  struct stat buf;

  memset(errorMsg, 0, sizeof(errorMsg));
 
  if (argc > 1 && strcmp("backup", argv[1]) == 0) {
    if (argc != 3) {
      usage(argv[0]);
      return 1;
    }
    cmd = CMD_BACKUP;
  } else if (argc > 1 && strcmp("restore", argv[1]) == 0) {
    if (argc != 3) {
      usage(argv[0]);
      return 1;
    }
    cmd = CMD_RESTORE;
  } else if (argc > 1 && strcmp("ls", argv[1]) == 0) {
    if (argc != 2) {
      usage(argv[0]);
      return 1;
    }
    show_files();
    return 0;
  } else {
    usage(argv[0]);
    return 1;
  }

  path = argv[2];

  path_stripped = malloc(strlen(path)+1);
  strcpy(path_stripped, path);
  strip_dots(path_stripped);

  path_real = realpath(path, NULL);

  if (path_real != NULL && strstr(path_real, forbidden) == path_real) {
    permissionsError = 1;
    strcpy(errorMsg, "Not allowed to access target/source: ");
    strncpy(errorMsg+strlen(errorMsg), path, sizeof(errorMsg)-strlen(errorMsg));
  }

  // set up paths for copy operation
  buffer = malloc(strlen(BACKUP_DIRECTORY) + 1 + strlen(path_stripped) + 1);
  if (buffer == NULL) {
    fprintf(stderr, "Failed to allocate memory\n");
    return 1;
  }

  if (cmd == CMD_BACKUP) {
    if (path_real != NULL) {
      src = path_real;
    } else {
      src = path;
    }

    dst = buffer;
    strcpy(dst, BACKUP_DIRECTORY);
    strcat(dst, "/");
    strcat(dst, path_stripped);
  } else if (cmd == CMD_RESTORE) {
    src = buffer;
    strcpy(src, BACKUP_DIRECTORY);
    strcat(src, "/");
    strcat(src, path_stripped);

    if (path_real != NULL) {
      dst = path_real;
    } else {
      dst = path;
    }

    // don't overwrite existing file if we don't own it
    if (stat(dst, &buf) == 0 && buf.st_uid != getuid()) {
      permissionsError = 1;
      strcpy(errorMsg, "Not your file: ");
 //     strncpy(errorMsg+strlen(errorMsg), path, sizeof(errorMsg)-strlen(errorMsg));
      strncat(errorMsg, path, sizeof(errorMsg)-strlen(errorMsg));
    }
  }

  if (permissionsError) {
    fprintf(stderr, "Exiting due to permissions error: %s\n", errorMsg);
    return 1;
  }
  
  // perform actual backup/restore operation
  if (copyFile(src, dst) < 0) {
    return 1;
  }
    
  // grant user access to restored file
  if (cmd == CMD_RESTORE) {
    if (restorePermissions(dst) < 0) {
      return 1;
    }
  }

  return 0;
}
