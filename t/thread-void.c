#if 0
#!/bin/sh
s=$0; o=${s%.*}; gcc -lpthread -o $o $s -Wall && ./$o; exit
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

static int concurrent = 5;

void* thread_main(void* arg) {
  int i = 0;
  for(i = 0; i < 100; i++) {
    system("./t/void >/dev/null");
  }
  pthread_exit(NULL);
}

int main(int argc, char *argv[]){
  int i = 0;
  pthread_t threadid[concurrent];
  
  for (i = 0; i < concurrent; i++) {
    if (pthread_create(&threadid[i], NULL, thread_main, (void*) (i + 1))) {
      fprintf(stderr, "pthread_create() error\n");
      return 1;
    }
  }
  
  for (i = 0; i < concurrent; i++) {
    void* ret = NULL;
    if (pthread_join(threadid[i], &ret)) {
      printf("pthread_join() error.\n");
      return 1;
    }
  }

  exit(0);
}
