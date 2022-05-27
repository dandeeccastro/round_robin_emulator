#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum PROCESS_PRIORITIES { Low = 0, High };
enum IO_EVENT_TYPES { Disk = 0, MagneticTape, Printer };
enum PROCESS_STATUSESS { Running = 0, Stopped, AcessingIO };

const unsigned int MAX_PROCESSES = 1024;
const unsigned int MAX_IO_EVENTS = 1024;
const unsigned int MAX_ARRIVAL_TIME = 60;
const unsigned int MAX_PID = 65535;
const unsigned int MAX_DURATION_S = 1000;
const unsigned int QUANTUM_MS = 100;
const unsigned int PRIORITIES = High - Low;
const unsigned int IO_EVT_TYPES = Printer - Disk;
const unsigned int STATUSES = AcessingIO - Running;

unsigned int INDEX = 0;

typedef struct {
  short unsigned int pid;
  short unsigned int duration;
  short unsigned int priority;
  short unsigned int status;
  short unsigned int arrival_time;
} process_t;

typedef struct {
  short unsigned int type;
  short unsigned int pid;
} io_evt_t;

void print_process_info(process_t *proc) {
  char priority[10];

  switch (proc->priority) {
  case Low:
    strcpy(priority, "Low");
    break;
  case High:
    strcpy(priority, "High");
    break;
  }

  printf("> Process %i\n", proc->pid);
  printf(">> Duration: %i seconds\n", proc->duration);
  printf(">> Priority: %s\n", priority);
  printf(">> Arrives at %i seconds\n\n", proc->arrival_time);
}

process_t generate_random_process() {
  process_t result;

  result.pid = rand() % MAX_PID;
  result.duration = rand() % MAX_DURATION_S;
  result.priority = rand() % PRIORITIES;
  result.status = rand() % STATUSES;
  result.arrival_time = rand() % MAX_ARRIVAL_TIME;

  return result;
}

bool read_processes_from_csv(process_t *start) {
  FILE *stream = fopen("process_table.csv", "r");
  char *line;
  size_t len = 1024;

  if (stream != NULL) {
    while (getline(&line, &len, stream) != -1) {
      char *arrival_time = strtok(line, ",");
      char *duration = strtok(NULL, ",");
      char *priority = strtok(NULL, ",");

      *(start + INDEX) = (process_t){.arrival_time = atoi(arrival_time),
                                     .duration = atoi(duration),
                                     .priority = atoi(priority),
                                     .status = Stopped};
      INDEX++;

      strtok(NULL, ",\n");
    }

    fclose(stream);
    return true;
  } else
    return false;
}

void generate_random_process_list(process_t *start) {
  for (int i = 0; i < MAX_PROCESSES; i++) {
    *(start + i) = generate_random_process();
  }
}

int main(void) {
  printf("{{ Round Robin Emulator }}\n");
  process_t process_queue[MAX_PROCESSES];

  if (!read_processes_from_csv(process_queue))
    generate_random_process_list(process_queue);

  printf(">> Random Process List generated, printing proc data\n");

  for (int i = 0; i < INDEX; i++) {
    print_process_info(&process_queue[i]);
  }

  return 0;
}
