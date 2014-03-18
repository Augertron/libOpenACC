
#include "OpenACC/utils/profiling.h"

#include <assert.h>

sqlite3 * profiling_db_file;
char * profiling_db_file_name;
int DbErr;
char *DbErrMsg;
char Dbstr[8192];

void init_profiling() {

  //Give a name to profiling_db_file
  char db_file_name[256];
  char hostname[64];
  char username[64];

  char year[5];
  char month[3];
  char day[3];
  char hour[3];
  char minute[3];
  char second[3];

  time_t rawtime;
  struct tm *info;
  time( &rawtime );
  info = localtime( &rawtime );

  gethostname (hostname, 64);
  getlogin_r (username, 64);

  sprintf(year, "%4d",info->tm_year+1900);
  sprintf(month, "%02d",info->tm_mon+1);
  sprintf(day, "%02d",info->tm_mday);
  sprintf(hour, "%02d",info->tm_hour);
  sprintf(minute, "%02d",info->tm_min);
  sprintf(second, "%02d",info->tm_sec);

  strcpy (db_file_name, username);
  strcat (db_file_name, "@");
  strcat (db_file_name, hostname);
  strcat (db_file_name, "YYYYMMDDHHMMSS");
  strcat (db_file_name, year);
  strcat (db_file_name, month);
  strcat (db_file_name, day);
  strcat (db_file_name, hour);
  strcat (db_file_name, minute);
  strcat (db_file_name, second);
  strcat (db_file_name, ".sl3");  

  profiling_db_file_name = &db_file_name[0];
  printf("profiling_db_file: %s\n",profiling_db_file_name);


  //Create Database file
  FILE *file;
  if (file = fopen (profiling_db_file_name, "r"))
    {   
      if (remove (profiling_db_file_name) != 0)
        printf (__FILE__,__LINE__,"Can not delete file");
      else
        printf ("File successfully deleted\n");
    }


  DbErr = sqlite3_open (profiling_db_file_name, &profiling_db_file);
  if (DbErr)
    {   
      printf (__FILE__,__LINE__,"Can not open database");
    }   
  else
    {   
      printf ("Opened database successfully\n");
    }


}//r \todo [profiling] init_profiling()

void exit_profiling() {
  //assert(profiling_db_file != NULL);

  /// \todo [profiling] exit_profiling()
}

/// \todo [profiling] add profiling function for kernel and data transfert

