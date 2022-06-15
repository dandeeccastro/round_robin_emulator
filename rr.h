#define MAX 5 
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
void runQueue(rr_process_t*, rr_process_t*, int, int*);
bool hasAtLeastOneProcessQueued(rr_process_t*, int*);
bool hasNoProcessesRunning(rr_process_t* , int* );
void readyNewProcesses(rr_process_t*, rr_process_t*, int, int*);
void setupProcessToRun(rr_process_t*);
void runProcess(rr_process_t*);
bool processHasEnded(rr_process_t*); 
bool processHasTimedOut(rr_process_t*);
void queueNextProcess(rr_process_t*, int*, int); 
void printProcessTable(rr_process_t*, int*); 
char* statusName(int);
