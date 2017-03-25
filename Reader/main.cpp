#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <unistd.h>          /* For ftruncate  */
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For file mode constants */
using namespace std;

#define handle_error(msg) \
           do { perror(msg); exit(EXIT_FAILURE); } while (0)

struct thread_arg{
    bool thread_ended = 0; //isn't end
    int  d_fifo;
};


static void * func1_thread(void *vp_arg){
    thread_arg* arg = (thread_arg*) vp_arg;
    char msg;
    while(!(arg->thread_ended) && (access("../super_fifo", R_OK) != -1)){ //st_nlink == 0 when fifo deleted
        //writer
        read(arg->d_fifo, (char*) &msg, sizeof(msg));
        printf("%c", msg);
        fflush(stdout);
    }
    pthread_exit((void*) "Reader thread ended");
}

int main(void)
{
    printf("Reader:\n\n");
    pthread_t thread1; thread_arg thread1_arg;

    //FIFO section
    int d_fifo = open("../super_fifo", O_RDONLY);
    if (d_fifo == -1) handle_error("open");

    //Thread section
    thread1_arg.d_fifo = d_fifo;
    if ( pthread_create( &thread1, NULL, func1_thread, &thread1_arg ) )
        handle_error("pthread_create");

    getchar();
    thread1_arg.thread_ended = 1;

    //Exit section
    char* exit_thread1_code;
    if ( pthread_join( thread1, (void**) &exit_thread1_code ) )
            return 1;
    printf("\n%s\n", exit_thread1_code);
    if (close(d_fifo)) handle_error("close");
    return EXIT_SUCCESS;
}
