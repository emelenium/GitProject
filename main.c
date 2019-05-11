#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>

#define STR_SIZE 50


long get_ppid(int n) {
    FILE *fd = NULL;
    
    long *ppid = NULL; // for ppid found
    ppid = (long*)calloc(1, sizeof(long));

    char *result = NULL; // for absolute path of a pid
    result = (char*)calloc(STR_SIZE, sizeof(char));
  
    snprintf(result, STR_SIZE, "/proc/%d/stat", n);
    
    fd = fopen(result, "r");
    
    if (fd != NULL) {
        fscanf(fd, "%*d %*s %*c %ld", ppid); // reading ppid from stat file
        fclose(fd);
    }

    if (ppid != NULL) {
      return *ppid;
    } else { //totally unnecessary part, but what the hell, the program is long already
      printf("*NULL\n"); // why not make it even longer?..
      return -1;
    }

    free(result);
    free(ppid);
    return 0;
}

bool isNum(char *str) {
    char *next  = NULL; // string can be converted to long and next char is \0
    strtol(str, &next, 10); // meaning nothing left to read
    bool isnum = (*next == '\0');
    return isnum;
}

void get_list_dirs(long pids[1000], long ppids[1000]) {
    DIR *dp  = NULL;
    int c = 0;
    char path[100] = "/proc";
    struct dirent *ep  = NULL;

    dp = opendir (path);
    if (dp != NULL) {
        while ((ep = readdir (dp))) {
            if (isNum(ep->d_name)) { // folder if numeric (so as process id, coincidence? don't think so)
                char *next = NULL;
                long i_pid = strtol(ep->d_name, &next, 10); // filling up pids
                pids[c] = i_pid;
                long i_ppid = get_ppid(i_pid); // and ppids
                ppids[c] = i_ppid;
                // printf("%ld\t%ld\n", i_pid, i_ppid); // list of pids ppids
                c++;
            }
        }
    }
    closedir(dp);
}

bool pid_exists(long n) {
    char result[STR_SIZE] = "";
    snprintf(result, sizeof(result), "/proc/%ld", n); 
    DIR * dp = opendir(result);
    bool exists = (dp != NULL); // checking if i can open a folder with a name /proc/pid
    closedir(dp);
    return exists;
}

void find_kids(long pids[1000], long ppids[1000], long parent, int *count) {
  int i; // wtf again

  for (i = 0; i < 1000 && ppids[i] != -1; ++i) { // recursivly counting kids
    if (ppids[i] == parent) {
      (*count)++;
      find_kids(pids, ppids, pids[i], count);
    }
  }
}

int main(int argc, char **argv) {

    if (argc < 2) { // if no argument --> exit
        printf("provide pid to start\n");
        exit(1);
    }
    
    int  *counter = (int*) malloc(1 * sizeof(int)); // kid counter
    long *pids    = (long*) malloc(1000 * sizeof(long)); // process ids
    long *ppids   = (long*) malloc(1000 * sizeof(long)); // parent process ids
    
    int i; // WTF?.. loop initial declaration used outside C99 mode GCC error?

    for (i = 0; i < 1000; ++i) { //fill arrays of pids and ppids with -1s
        pids[i] = -1;
        ppids[i] = -1;
    }

    long pid = 0;
    char *endptr = NULL;

    pid = strtol(argv[1], &endptr, 10); // get long from argument (not checking for validity)

    if (pid_exists(pid)) { // checking if pid actually exists
        get_list_dirs(pids, ppids); // filling pids and ppids

        find_kids(pids, ppids, pid, counter); // searching for kids of a process
        printf("%d\n", (*counter)+1); // output counter

    }

    free(ppids); // cleaning up after myself
    free(pids);
    free(counter);
    
    return 0;
}