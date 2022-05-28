#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ENUMS
enum PROCESS_PRIORITIES { Low = 0, High };
enum IO_EVENT_TYPES { Disk = 0, MagneticTape, Printer };
enum PROCESS_STATUSESS { Running = 0, Stopped, AcessingIO };

// DEFINES
#define MAX_IO_EVENTS 1024

// CONSTANTS
const unsigned int MAX_PROCESSES = 1024;
const unsigned int MAX_ARRIVAL_TIME = 60;
const unsigned int MAX_PID = 65535;
const unsigned int MAX_DURATION_S = 1000;
const unsigned int QUANTUM_MS = 100;
const unsigned int PRIORITIES = High - Low;
const unsigned int IO_EVT_TYPES = Printer - Disk;
const unsigned int STATUSES = AcessingIO - Running;

// GLOBALS
unsigned int INDEX = 0;

// TYPES
typedef struct {
  short unsigned int type;
  short unsigned int pid;
  short unsigned int at;
} io_evt_t;

typedef struct {
  short unsigned int pid;
  short unsigned int duration;
  short unsigned int priority;
  short unsigned int status;
  short unsigned int arrival_time;
  short unsigned int total_io_events;
  io_evt_t io_events[MAX_IO_EVENTS];
} process_t;

// FUNCTION DECLARATIONS
void print_process_info(process_t *proc);
process_t generate_random_process();
void allocate_process_on_master_queue(process_t *memory_pos, char *line);
void allocate_io_events_on_process(process_t *start, char *event);
bool read_processes_from_csv(process_t *start);
bool read_io_events_from_csv(process_t *start);
void generate_random_process_list(process_t *start);

int main(void) {
  printf("{{ Round Robin Emulator }}\n");
  process_t process_queue[MAX_PROCESSES];

  if (!read_processes_from_csv(process_queue))
    generate_random_process_list(process_queue);

  read_io_events_from_csv(process_queue);
  printf(">> Random Process List generated, printing proc data\n");

  for (int i = 0; i < INDEX; i++) {
    print_process_info(&process_queue[i]);
  }

  return 0;
}

bool read_processes_from_csv(process_t *start) {
  FILE *process_source = fopen("process_table.csv", "r");
  char *line;
  size_t len = 1024;

  if (process_source != NULL) {
    while (getline(&line, &len, process_source) != -1) {
      allocate_process_on_master_queue(start + INDEX, line);
      INDEX++;
      strtok(NULL, ",\n");
    }
    fclose(process_source);
    return true;
  } else
    return false;
}

bool read_io_events_from_csv(process_t *start) {
  FILE *io_evt_source = fopen("io_table.csv", "r");
  char *line;
  size_t len = 1024;

  if (io_evt_source != NULL) {
    while (getline(&line, &len, io_evt_source) != -1) {
      allocate_io_events_on_process(start, line);
    }
  }
}

void allocate_process_on_master_queue(process_t *memory_pos, char *line) {
  const char *pid = strtok(line, ",");
  const char *arrival_time = strtok(NULL, ",");
  const char *duration = strtok(NULL, ",");
  const char *priority = strtok(NULL, ",");

  *memory_pos = (process_t){
      .pid = atoi(pid),
      .arrival_time = atoi(arrival_time),
      .duration = atoi(duration),
      .priority = atoi(priority),
      .status = Stopped,
      .io_events = (io_evt_t *)calloc(sizeof(io_evt_t), MAX_IO_EVENTS),
      .total_io_events = 0};
}

void generate_random_process_list(process_t *start) {
  for (int i = 0; i < MAX_PROCESSES; i++) {
    *(start + i) = generate_random_process();
  }
}

process_t generate_random_process() {
  process_t result;

  result.pid = rand() % MAX_PID;
  result.duration = rand() % MAX_DURATION_S;
  result.priority = rand() % PRIORITIES;
  result.status = rand() % STATUSES;
  result.arrival_time = rand() % MAX_ARRIVAL_TIME;
  result.total_io_events = 0;

  return result;
}

void allocate_io_events_on_process(process_t *start, char *event) {
  const char *pid = strtok(event, ",");
  const char *type = strtok(NULL, ",");
  const char *at = strtok(NULL, ",");

  int j = 0;
  for (int i = 0; i < INDEX; i++) {
    if (atoi(pid) == (start + i)->pid) {
      puts("OK, é pra botar aqui");
      (start + i)->io_events[j] =
          (io_evt_t){.pid = atoi(pid), .type = atoi(type), .at = atoi(at)};
      (start + i)->total_io_events += 1;
      j++;
    }
  }
}

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
  printf(">> Arrives at %i seconds\n", proc->arrival_time);
  printf(">> Has %i IO events\n\n", proc->total_io_events);
}
