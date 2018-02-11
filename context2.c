#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/select.h>
#include "context.h"

struct process *proc_list;
unsigned char first_run = 1;

unsigned int a_vctxts = 0;    //voluntary
unsigned int a_nvctxts = 0;   //non-voluntary

void context_exit(int sig) {
  free(proc_list);
  printf("%d\n", sig);
  exit(0);
}

void get_context() {
  FILE *fd;
  char path[50], line[255];
  DIR* fd_d;
  struct dirent *dirp;
  int pid;
  unsigned int vctxts = 0, nvctxts = 0, diff_vctxts = 0, diff_nvctxts = 0;

  if (!(fd_d = opendir("/proc/30681/task"))) {
    printf("Unable to open proc/30681/task\n");
  }
  

  printf("Scanning parent\n");
  snprintf(path, sizeof(path), "/proc/30681/status");
//  printf("path = %s\n", path);
  if ((fd=fopen(path,"r")) != NULL) {
    while ((fgets(line, 255, fd) != NULL)) {
      if ((strncmp(line, "voluntary_ctxt_switches:",24)) == 0) {
        sscanf(line, "%*s %u", &vctxts);
//        printf("in parent vctxts = %u\n", vctxts);
      }
      if ((strncmp(line, "nonvoluntary_ctxt_switches:",27)) == 0) {
        sscanf(line, "%*s %u", &nvctxts);
//        printf("in parent nvctxts = %u\n", nvctxts);
      }
    }
//  fclose(fd);
  }
  fclose(fd);
//  printf("poor man\n");
  if (first_run == 0) {
    diff_vctxts = vctxts - a_vctxts;
      printf("diff_vctxts = %u\n", diff_vctxts);
    diff_nvctxts = nvctxts - a_nvctxts;
      printf("diff_nvctxts = %u\n", diff_nvctxts);
    if ((diff_vctxts != 0) & (diff_nvctxts != 0)) {
      printf("parent voluntary = %d\t\t non-voluntary = %d\n", diff_vctxts, diff_nvctxts);
    }
    a_vctxts = vctxts;
    a_nvctxts = nvctxts;
  }


  printf("scaning children\n");
  while ((dirp = readdir(fd_d)) != NULL) {
    if ((pid = atoi(dirp->d_name))) {
//      printf("found %s\n", dirp->d_name);
      snprintf(path, sizeof(path), "/proc/%d/status", pid);
      if ((fd=fopen(path,"r")) != NULL) {	//process is still there
        while ((fgets(line, 255, fd) != NULL)) {

          if ((strncmp(line, "voluntary_ctxt_switches:",24)) == 0) {
            sscanf(line, "%*s %u", &vctxts);
          }
          if ((strncmp(line, "nonvoluntary_ctxt_switches:",27)) == 0) {
            sscanf(line, "%*s %u", &nvctxts);
          }

        }

        if (first_run == 0) {
          diff_vctxts = vctxts - proc_list[pid].vctxts;
          diff_nvctxts = nvctxts - proc_list[pid].nvctxts;
          if ((diff_vctxts != 0) || (diff_nvctxts != 0))
            printf("pid %d\t voluntary = %d\t\t non-voluntary = %d\n", pid, diff_vctxts, diff_nvctxts);
        }

        proc_list[pid].vctxts = vctxts;
        proc_list[pid].nvctxts = nvctxts;
      }
    fclose(fd);
    }
  }
closedir(fd_d);
printf("\n\n\n\n");
}


int main(int argc, char *argv[]) {

  struct timeval tv;
  signal(SIGTERM, context_exit);
  signal(SIGINT, context_exit);
  
  proc_list = (struct process *) malloc(MAX_PROC *sizeof(struct process));
  memset(proc_list, 0, MAX_PROC * sizeof(struct process));

  printf("sizeof(process) = %d\n", sizeof(struct process));
  
  for (;;) {
    get_context();
//    getchar();
    first_run = 0;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    select(0, NULL, NULL, NULL, &tv);
  }

  return 0;
}
