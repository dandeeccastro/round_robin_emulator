#define QUANTUM 2
#define MAX_PROCESSES 1024

enum STATUSES { Pending = 0, Running, Stopped, Finalized, QUEUE_END };
enum IO_TYPES { Disk = 0, MagneticTape, Printer };

typedef struct {
  int pid;
  int arrival;
  int duration;
  int status;
  int arrived_at;
  int quantum;
} rr_process_t;

typedef struct {
  int pid;
  int type;
  int arrival;
  int status;
  int quantum;
} rr_io_event_t;
