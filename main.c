#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <curses.h> 

#include "rr.h"

int MASTER_INDEX = 0;
int HIGH_INDEX = 0;
int LOW_INDEX = 0;
int IO_INDEX = 0;

int main(void) {
  // Criando os processos para serem emulados
  rr_process_t *master = calloc(sizeof(rr_process_t), MAX);
  rr_process_t *high = calloc(sizeof(rr_process_t), MAX);
  rr_process_t *low = calloc(sizeof(rr_process_t), MAX);
  rr_process_t *io = calloc(sizeof(rr_process_t), MAX);

  srand(time(NULL));
  initscr();

  // Null -> New
  for ( MASTER_INDEX = 0; MASTER_INDEX < MAX; MASTER_INDEX++ ) 
    master[MASTER_INDEX] = generateRandomProcess(); 
  
  printw("{{ MASTER QUEUE }}\n");
  printProcessTable(master, &MASTER_INDEX);
  refresh();

  bool run_once = false;

  for( int time = 0; processesLeft(high) > 0; time++ ) {
    clear();
    printw("\n>> INSTANT %i << {{ %i processes left }} \n", time, processesLeft(high));

    runQueue(master, high, time, &HIGH_INDEX);
    printw("{{ HIGH QUEUE }}\n");
    printProcessTable(high, &HIGH_INDEX);

    runQueue(master, low, time, &LOW_INDEX);
    printw("{{ LOW QUEUE }}\n");
    printProcessTable(low, &LOW_INDEX);

    refresh();
    usleep(500000);
  }

  free(master);
  free(high); 
  free(low);
  free(io);

  endwin();

  return 0;
}

rr_process_t generateRandomProcess() {
  int pid = rand() % 65535;
  int arrival = rand() % 10;
  int duration = (rand() % 25) + 1;

  return (rr_process_t) {
    .pid = pid,
    .status = New,
    .duration = duration,
    .arrival = arrival, 
    .time_left = duration,
    .runtime = 0,
  };
}

int processesLeft(rr_process_t* queue) {
  int result = 0; 
  for (int i = 0; i < MASTER_INDEX; i++ )
    if ( (queue + i)->status != Done ) 
      result++;
  return result;
}

void runQueue(rr_process_t* master, rr_process_t* queue, int instant, int *index) {

  readyNewProcesses(master, queue, instant, index);
  if ( hasAtLeastOneProcessQueued(queue, index) && hasNoProcessesRunning(queue, index) ) {
    setupProcessToRun(queue);
  } else {
    for ( int i = 0; i < *index; i++ ) {
      if ( queue[i].status == Running ) {
        runProcess(queue + i);
        if (processHasEnded(queue + i) || processHasTimedOut(queue + i))
          queueNextProcess(queue, index, i);
        break;
      }
    }
  }
}

bool hasAtLeastOneProcessQueued(rr_process_t* queue, int* index) {
  for ( int i = 0; i < *index; i++ )
    if ( queue[i].status == Ready )
      return true;
  return false;
}

bool hasNoProcessesRunning(rr_process_t* queue, int* index) {
  for ( int i = 0; i < *index; i++ )
    if ( queue[i].status == Running )
      return false;
  return true;
}

void readyNewProcesses(rr_process_t* master, rr_process_t* queue, int instant, int* index) {
  for ( int i = 0; i < MASTER_INDEX; i++ ) {
    if ( master[i].status == New && master[i].arrival <= instant ) {
      master[i].status = Ready; 
      queue[*index] = master[i];
      /* printw(">> NEW: %i \n", queue[*index].pid); */
      (*index)++; 
    }
  }
}

void setupProcessToRun(rr_process_t* process) {
  process->status = Running; 
  process->runtime = QUANTUM;
  /* printw(">> RUNNING: %i\n", process->pid); */
}

void runProcess(rr_process_t* process) {
  process->runtime--;
  process->time_left--;
  /* printw(">> EXECUTED: %i {{ %i time left, %i CPU time left }}\n", */
      /* process->pid, process->time_left, process->runtime); */
}

bool processHasEnded(rr_process_t* process) {
  if ( process->time_left == 0 ) {
    process->status = Done; 
    /* printw(">> DONE: %i\n", process->pid); */
    return true; 
  } return false; 
}

bool processHasTimedOut(rr_process_t* process) {
  if ( process->status == Running && process->runtime == 0 ) {
    process->status = Ready;
    /* printw(">> TIMEOUT: %i\n", process->pid); */
    return true; 
  } return false; 
}

void queueNextProcess(rr_process_t* queue, int* index, int current) {

  bool found = false;
  for ( int i = current + 1 == *index ? 0 : current + 1; current != i; i = (i + 1) % *index ) {
    if ( queue[i].status == Ready ) {
      setupProcessToRun(queue + i);
      found = true;
      break;
    }
  }

  if ( queue[current].status == Ready && !found )
    setupProcessToRun(queue + current);

}

void printProcessTable(rr_process_t* queue, int* index) {
  printw("[    PID    |  STATUS   | DURATION  |  ARRIVAL  | TIME_LEFT |  RUNTIME  ]\n");
  for ( int i = 0; i < *index; i++ ) {
    char* status_name = statusName(queue[i].status);

    printw("[ %9i |", queue[i].pid);
    printw(" %9s |", status_name);
    printw(" %9i |", queue[i].duration);
    printw(" %9i |", queue[i].arrival);
    printw(" %9i |", queue[i].time_left);
    printw(" %9i ]\n", queue[i].runtime);
  }
}

char* statusName(int status_id) {
  switch (status_id) {
    case 0:
      return "New";
    case 1:
      return "Ready";
    case 2:
      return "Running";
    case 3:
      return "Interrupt";
    case 4: 
      return "Done";
    default: 
      return "ERROR";
  }
}
