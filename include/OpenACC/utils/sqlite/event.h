
#ifndef __LIBOPENACC_SQLITE_EVENT_H__
#define __LIBOPENACC_SQLITE_EVENT_H__

#include "sqlite3.h"

#ifdef __cplusplus
extern "C" {
#endif

struct acc_sqlite_event_entry_t {
  size_t run_id;
  size_t device_id;
  char command_name[128];
  size_t command_id;
  long cl_profiling_command_queued;
  long cl_profiling_command_submit;
  long cl_profiling_command_start;
  long cl_profiling_command_end;
};
extern size_t event_entry_num_fields;
extern char * event_entry_field_names[8];
extern enum acc_sqlite_type_e event_entry_field_types[8];
extern size_t event_entry_field_sizes[8];
extern size_t event_entry_field_offsets[8];

struct acc_sqlite_event_entry_t * acc_sqlite_get_event_entries(sqlite3 * db, size_t * count);

#ifdef __cplusplus
}
#endif

#endif /* __LIBOPENACC_SQLITE_EVENT_H__ */

