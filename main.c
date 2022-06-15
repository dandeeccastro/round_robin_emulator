#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

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

  // Null -> New
  for ( MASTER_INDEX = 0; MASTER_INDEX < MAX; MASTER_INDEX++ ) 
    master[MASTER_INDEX] = generateRandomProcess(); 

  bool run_once = false;

  for( int time = 0; processesLeft(high) > 0; time++ ) {
    printf("\n>> INSTANT %i << {{ %i processes left }} \n", time, processesLeft(high));

    // New -> Ready
    for ( int i = 0; i < MASTER_INDEX; i++ ) {
      if ( master[i].status == New && master[i].arrival <= time ) {
        master[i].status = Ready; 
        high[HIGH_INDEX] = master[i];
        printf(">> NEW: %i \n", (high + HIGH_INDEX)->pid);
        HIGH_INDEX++; 
      }
    }

    if ( HIGH_INDEX > 0 && !run_once ) {
      printf(">> RUNNING: %i\n", high[0].pid);
      high[0].status = Running; 
      high[0].runtime = QUANTUM;
      run_once = true; 
    } else {
      for ( int i = 0; i < HIGH_INDEX; i++ ) {
        if ( high[i].status == Running ) {

          // Running round
          high[i].runtime--;
          high[i].time_left--;

          printf(">> EXECUTED: %i {{ %i time left, %i CPU time left }}\n",
              high[i].pid, high[i].time_left, high[i].runtime);

          if ( high[i].runtime == 0 || high[i].time_left == 0) {

            // Running -> Done
            if ( high[i].time_left == 0 ) {
              printf(">> DONE: %i\n", high[i].pid);
              high[i].status = Done; 
            }

            // Running -> Ready 
            else if ( high[i].runtime == 0 ) {
              printf(">> TIMEOUT: %i\n", high[i].pid);
              high[i].status = Ready; 
            } 

            bool found = false;
            for ( int j = i + 1 == HIGH_INDEX ? 0 : i + 1; j != i; j = (j + 1) % HIGH_INDEX ) {
              if ( high[j].status == Ready ) {
                printf(">> RUNNING: %i\n", high[j].pid);
                high[j].status = Running;
                high[j].runtime = QUANTUM;
                found = true;
                break;
              }
            }

            if ( high[i].status == Ready && !found ) {
              printf(">> RUNNING: %i\n", high[i].pid);
              high[i].status = Running;
              high[i].runtime = QUANTUM;
            }
            /* int next = i + 1 == HIGH_INDEX ? 0 : i + 1; */
            /* if ( high[next].status == Ready ) { */
            /*   printf(">> RUNNING: %i\n", high[next].pid); */
            /*   high[next].status = Running; */
            /*   high[next].runtime = QUANTUM; */
            /*   break; */
            /* }  */
            break;
          }
        }
      }
    }

    usleep(1500);
  }

  free(master);
  free(high); 
  free(low);
  free(io);

  return 0;
}

rr_process_t generateRandomProcess() {
  int pid = rand() % 65535;
  int arrival = rand() % 10;
  int duration = (rand() % 25) + 1;

  printf("PID %i arrives at %i and lasts %i\n", pid, arrival, duration);

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
