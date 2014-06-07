
#include "OpenACC/utils/sqlite/event.h"

size_t event_entry_num_fields = 8;
char * event_entry_field_names[8] = {
  "run_id", "device_id", "command_name", "command_id",
  "cl_profiling_command_queued", "cl_profiling_command_submit",
  "cl_profiling_command_start", "cl_profiling_command_end"
};
enum acc_sqlite_type_e event_entry_field_types[8] = {e_sqlite_int, e_sqlite_int, e_sqlite_text, e_sqlite_int, e_sqlite_bigint, e_sqlite_bigint, e_sqlite_bigint, e_sqlite_bigint};
size_t event_entry_field_sizes[8] = {sizeof(size_t), sizeof(size_t), sizeof(char[128]), sizeof(size_t), sizeof(long), sizeof(long), sizeof(long), sizeof(long)};
size_t event_entry_field_offsets[8] = {
  offsetof(struct acc_sqlite_event_entry_t, run_id),
  offsetof(struct acc_sqlite_event_entry_t, device_id),
  offsetof(struct acc_sqlite_event_entry_t, command_name),
  offsetof(struct acc_sqlite_event_entry_t, command_id),
  offsetof(struct acc_sqlite_event_entry_t, cl_profiling_command_queued),
  offsetof(struct acc_sqlite_event_entry_t, cl_profiling_command_submit),
  offsetof(struct acc_sqlite_event_entry_t, cl_profiling_command_start),
  offsetof(struct acc_sqlite_event_entry_t, cl_profiling_command_end)
};

struct acc_sqlite_event_entry_t * acc_sqlite_get_event_entries(sqlite3 * db, size_t * count);

