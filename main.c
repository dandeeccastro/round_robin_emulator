#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "rr.h"

void read_processes(char *, rr_process_t *);
void read_io_events(char *, rr_io_event_t *);
bool is_end_of_process_queue(rr_process_t *);
bool is_end_of_io_queue(rr_io_event_t *);
int processes_left(rr_process_t *);
bool io_event_should_happen(rr_io_event_t *, rr_process_t *, time_t);

int main(void) {

  rr_process_t *process_queue = calloc(sizeof(rr_process_t), MAX_PROCESSES);
  rr_io_event_t *io_queue = calloc(sizeof(rr_io_event_t), MAX_PROCESSES);

  int high_queue[MAX_PROCESSES];
  int low_queue[MAX_PROCESSES];
  int io_evt_queue[MAX_PROCESSES];

  int high_index = 0;
  int low_index = 0;
  int io_index = 0;

  read_processes("processes.csv", process_queue);
  read_io_events("io.csv", io_queue);

  for (int i = 0; !is_end_of_process_queue(process_queue + i); i++)
    printf("PID %i arrives at %i and lasts %i seconds\n",
           (process_queue + i)->pid, (process_queue + i)->arrival,
           (process_queue + i)->duration);

  for (int i = 0; !is_end_of_io_queue(io_queue + i); i++)
    printf("IO event of PID %i arrives at %i of runtime\n", (io_queue + i)->pid,
           (io_queue + i)->arrival);

  printf("There are still %i processes to process\n",
         processes_left(process_queue));

  time_t start = time(NULL);
  while (processes_left(process_queue) > 0) {
    // Round the Robins
    time_t now = start - time(NULL);

    for (int i = 0; !is_end_of_process_queue(process_queue + i); i++) {
      // Tem um processo novo para chegar?
      if ((process_queue + i)->arrival >= now &&
          (process_queue + i)->status == Pending) {
        high_queue[high_index] = (process_queue + i)->pid;
        high_index++;
      }
    }

    for (int i = 0; !is_end_of_io_queue(io_queue + i); i++) {
      if (io_event_should_happen(io_queue + i, process_queue, now))
        puts("Ih caralho era pra isso estar rodando hein");
    }

    sleep(1);
  }

  free(process_queue);
  free(io_queue);

  return 0;
}

void read_processes(char *process_file, rr_process_t *proc) {
  FILE *process_file_stream = fopen(process_file, "r");

  size_t line_size = 256;
  char *line = calloc(sizeof(char), line_size);

  int i = 0;
  while (fgets(line, line_size, process_file_stream)) {
    char *pid = strtok(line, ",");
    char *arrival = strtok(NULL, ",");
    char *duration = strtok(NULL, "\n");

    *(proc + i) = (rr_process_t){.pid = atoi(pid),
                                 .arrival = atoi(arrival),
                                 .duration = atoi(duration),
                                 .status = Pending,
                                 .quantum = QUANTUM};
    i++;
  }

  *(proc + i) = (rr_process_t){
      .pid = 0, .arrival = 0, .duration = 0, .status = QUEUE_END};

  fclose(process_file_stream);
}

void read_io_events(char *io_file, rr_io_event_t *io) {
  FILE *io_file_stream = fopen(io_file, "r");

  size_t line_size = 256;
  char *line = calloc(sizeof(char), line_size);

  int i = 0;
  while (fgets(line, line_size, io_file_stream)) {
    char *pid = strtok(line, ",");
    char *type = strtok(NULL, ",");
    char *arrival = strtok(NULL, "\n");

    *(io + i) = (rr_io_event_t){.pid = atoi(pid),
                                .arrival = atoi(arrival),
                                .type = atoi(type),
                                .quantum = QUANTUM,
                                .status = Pending};
    i++;
  }

  *(io + i) =
      (rr_io_event_t){.pid = 0, .arrival = 0, .type = 0, .status = QUEUE_END};

  fclose(io_file_stream);
}

bool is_end_of_process_queue(rr_process_t *proc) {
  return proc->status == QUEUE_END;
}

bool is_end_of_io_queue(rr_io_event_t *io) { return io->status == QUEUE_END; }

int processes_left(rr_process_t *queue) {
  int result = 0;
  for (int i = 0; !is_end_of_process_queue(queue + i); i++)
    if ((queue + i)->status != Finalized || (queue + i)->status != QUEUE_END)
      result++;
  return result;
}

bool io_event_should_happen(rr_io_event_t *io_evt, rr_process_t *queue,
                            time_t now) {
  for (int i = 0; !is_end_of_process_queue(queue + i); i++) {
    if (io_evt->pid == (queue + i)->pid && (queue + i)->status == Running &&
        io_evt->arrival + (queue + i)->arrived_at >= now)
      return true;
  }
  return false;
}
