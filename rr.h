#define MAX 10
#define QUANTUM 2

enum STATUS { New = 0, Ready, Running, Interrupted, Done };

typedef struct rr_process_t {
  int pid;
  int status;
  int duration;
  int arrival; 
  int time_left;
  int runtime;
} rr_process_t;

rr_process_t generateRandomProcess();
int processesLeft(rr_process_t*);
void runNextHigh(rr_process_t*, int);
