
#include <OpenACC/openacc.h>

#include "OpenACC/private/runtime.h"
#include <OpenACC/internal/compiler.h>

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define LIST_MODE 2
#define NUMBER_DEVICES_MODE 4

acc_compiler_data_t compiler_data = {NULL, NULL, NULL, 0, NULL};

void usage(int argc, char ** argv) {
  printf("usage: %s --list|-l\n", argv[0]);
  printf("           Display the list of the devices available.\n");
  printf("       %s --number-devices|-n\n", argv[0]);
  printf("           Display the number of the devices available.\n");
}

unsigned short read_args(int argc, char ** argv) {
  unsigned short mode = 0;

  size_t cnt = 1;
  while (cnt < argc) {
    if (strcmp(argv[cnt], "--list") == 0 || strcmp(argv[cnt], "-l") == 0)
      mode |= LIST_MODE;
    else if (strcmp(argv[cnt], "--number-devices") == 0 || strcmp(argv[cnt], "-n") == 0)
      mode |= NUMBER_DEVICES_MODE;
    else
      usage(argc, argv);

    cnt++;
  }

  if (mode == 0) {
    usage(argc, argv);
    exit(0);
  }

  if (mode & ~(NUMBER_DEVICES_MODE) != 0 || mode & ~(LIST_MODE) != 0) {
    usage(argc, argv);
    exit(-1);
  }

  return mode;
}

int main(int argc, char ** argv) {
  acc_init_once();

  unsigned short mode = read_args(argc, argv);

  size_t num_devices = acc_runtime.devices[acc_device_any].num;

  if ((mode & LIST_MODE) == LIST_MODE) {
    size_t dev_idx;
    printf("----------------------------------------------------------------------------------------\n");
    printf(" ID |      Env. name       | # | Full Name\n");
    printf("----|----------------------|---|--------------------------------------------------------\n");
    for (dev_idx = 0; dev_idx < num_devices; dev_idx++) {
      acc_device_t dev = acc_get_device_type_by_device_idx(dev_idx);
      unsigned num = dev_idx - acc_runtime.devices[dev].first;
      char env_name[20];
      sprintf(env_name, "%s", acc_device_env_name[dev]);
      
      printf(" %2zd | %+20s | %u | %s\n", dev_idx, acc_device_env_name[dev], num, acc_device_name[dev]);
    }
    printf("----------------------------------------------------------------------------------------\n\n");
  }

  if ((mode & NUMBER_DEVICES_MODE) == NUMBER_DEVICES_MODE)
    printf("%zd\n", num_devices);

  return 0;
}

