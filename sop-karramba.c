#include "common.h"
#include "board_utils.h"

#define BOARD_FILE "board"
#define FIFO_NAME "fifo"
#define STEP_COUNT 500
#define WAIT_N 10

#define PORT 12345

#define EPOLL_MAX_EVENTS 10


typedef struct{
    int n;
    int m;
    pthread_mutex_t mutex;
    char *board;
    
} shared_t;
void usage(char* program_name)
{
    fprintf(stderr, "Usage: \n");

    fprintf(stderr, "\t%s n m\n", program_name);
    fprintf(stderr, "\t  n, m - board width and height, respectively\n");

    exit(EXIT_FAILURE);
}

void don_work(shared_t *shared)
{
    ms_sleep(WAIT_N*100);
    srand(time(NULL)^getpid());
    int x = rand()%shared->n;
    int y = rand()%shared->m;
    for(int s=0; s<STEP_COUNT; s++)
    {

        pthread_mutex_lock(&shared->mutex);
        if(!has_trail(shared->board, x, y, shared->n, shared->m)) {
            
        set_char(shared->board, x, y, shared->n, shared->m, KARRAMBA_CHAR);
            
            printf("Carramba!\n");
        }
        else{
            
        set_char(shared->board, x, y, shared->n, shared->m, EMPTY_CHAR);
        }

        char move = get_trail_move(shared->board, x, y, shared->n, shared->m);
        move_pos(shared->board, move, shared->n, shared->m, &x, &y);
        pthread_mutex_unlock(&shared->mutex);
        ms_sleep(100);
    }

    munmap(shared->board, shared->m*(shared->n+1));
    munmap(shared, sizeof(shared_t));
}
void do_expedition(shared_t *shared)
{
    pid_t pid = fork();
    if(pid<0) {ERR("fork");}
    if(pid==0)
    {
        don_work(shared);
        exit(EXIT_SUCCESS);
    }

    else{

        int fifo_fd = mkfifo(FIFO_NAME, 0666);

        srand(time(NULL));
        int x = rand()%shared->n;
        int y = rand()%shared->m;
        for(int s=0; s<STEP_COUNT; s++)
        {
            pthread_mutex_lock(&shared->mutex);
            set_char(shared->board, x, y, shared->n, shared->m, TRAIL_CHAR);
            char move = get_random_move(shared->board, x, y, shared->n, shared->m);
            move_pos(shared->board, move, shared->n, shared->m, &x, &y);

            char fifo_send_move = get_random_move(shared->board, x, y, shared->n, shared->m);
            set_char(shared->board, x, y, shared->n, shared->m, EXPEDITION_CHAR);
            pthread_mutex_unlock(&shared->mutex);
            ms_sleep(100);
        }
    }
}
int main(int argc, char** argv) {

    if(argc!=3)
    {
        usage(argv[0]);
    }

    int n = atoi(argv[1]);
    int m = atoi(argv[2]);
    int size = m* (n+1);

    shared_t *shared = mmap(NULL, sizeof(shared_t), PROT_READ| PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
    shared->n = n;
    shared-> m =m;
    int fd = open(BOARD_FILE, O_RDWR|O_CREAT| O_TRUNC, 0666);
    if(fd==-1) { ERR("open");}
    if(ftruncate(fd, size)<0){ERR("frtunc");};

    shared->board = mmap(NULL,size, PROT_READ| PROT_WRITE, MAP_SHARED, fd, 0);
    
    if(shared->board==MAP_FAILED) {ERR("MMAP");}
    close(fd);


    fill_board(shared->board, n, m);

    //pthread_mutex_t mutex;

    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&shared->mutex,&attr);
    pthread_mutexattr_destroy(&attr);
    
    
    do_expedition(shared);
    printf("done!\n");
    pthread_mutex_destroy(&shared->mutex);

    munmap(shared->board, size);
    munmap(shared, sizeof(shared_t));

 }