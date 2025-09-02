#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
  pid_t pid = getpid();

  printf("pid: %lu\n", pid);
  sleep(30) ;
  printf("Sleep ends") ; 
  return 0 ; 
}
