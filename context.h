
#define MAX_PROC 32768
//from /proc/sys/kernel/pid_max

struct  process {
unsigned int pid;
unsigned int vctxts;	//voluntary
unsigned int nvctxts;	//non-voluntary
};
