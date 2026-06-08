#include "l8_common.h"
#define BACKLOG 16
#define LOGIN_MAX 16
#define COMMAND_MAX 8
#define PARAM_MAX 10
#define MIN_SIZE (LOGIN_MAX+COMMAND_MAX)



typedef struct{
    char login[LOGIN_MAX];
    char command[COMMAND_MAX];
    uint32_t params[PARAM_MAX];
} message_t;

int isValideMessage(message_t *message, int read_len)
{
    //check login
    int isValidLogin=0, isValidCMD=0, isValidLen=0;
    for(int u=0; u<USERS; u++)
    {
        if(strncmp(message->login, LOGINS[u], LOGIN_MAX)==0)
        {
            isValidLogin=1;
        }

    }

    for(int c=0; c<CMDS; c++)
    {
        if(strncmp(message->command, COMMANDS[c], COMMAND_MAX)==0)
        {
            isValidCMD=1;
        }
    }

    if((read_len-LOGIN_MAX-COMMAND_MAX)%8==0) {isValidLen=1;}

    return isValidCMD && isValidLen && isValidLogin;

}

void usage(char* name)
{
    printf("%s <in_port>\n", name);
    printf("  in_port - port that accepts messages\n");
    exit(EXIT_FAILURE);
}


void handleExit()
{

}
int main(int argc, char** argv) { 

    if(argc!=2)
    {
        usage(argv[0]);
    }
    int socketfd = bind_inet_socket(atoi(argv[1]), SOCK_DGRAM, BACKLOG);
    for(;;)
    {   
        message_t message;
        struct sockaddr_in addr;
        socklen_t len = sizeof(addr);
        memset(&message, 0, sizeof(message_t));
        int count = recvfrom(socketfd, &message, sizeof(message_t), 0, (struct sockaddr*)&addr.sin_addr, &len);
        if(count<0) {ERR("recvfrom");}
        if(count<MIN_SIZE) {continue;}
        if(!isValideMessage(&message, count)) {printf("WRONG TRY AGAN!\n"); continue;}

        //if(count>MIN_SIZE){
            
        printf("%16s: %8s ", message.login, message.command);
        for(int p =0; p<(count-MIN_SIZE)/4; p++)
        
        {
            message.params[p]= ntohl(message.params[p]);
            printf("%u ", message.params[p]);
        }
        printf("\n");

        if(strncmp(message.command, "EXIT", 4)==0)
        {
            break;
        }
        //}


    }

    close(socketfd);
 }
