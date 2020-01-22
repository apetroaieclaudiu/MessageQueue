#include <mqueue.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <signal.h>

#define MSG_SIZE            4096
#define SERVER_QUEUE_NAME   "Server"
// This handler will be called when the queue 
// becomes non-empty.

char buf[MSG_SIZE];              // A good-sized buffer
mqd_t mqrecv, mqsend;    

void main () {

    struct mq_attr attr, old_attr;   // To store queue attributes
             // Message queue descriptors
    unsigned int prio;               // Priority 

    // First we need to set up the attribute structure
    attr.mq_maxmsg = 300;
    attr.mq_msgsize = MSG_SIZE;
    attr.mq_flags = 0;

    old_attr.mq_maxmsg = 300;
    old_attr.mq_msgsize = MSG_SIZE;
    old_attr.mq_flags = 0;
    // Open a queue with the attribute structure
    mqrecv = mq_open (SERVER_QUEUE_NAME, O_RDONLY | O_CREAT, 
                    0664, &attr);
    // Get the attributes for server queue
    mq_getattr (mqrecv, &attr);

    while (1) {
        char    rcvmsg[MSG_SIZE];
        int     i = 0, j = 0;
        char    clientQueue[10];
        int     startWriting = 0;
        int r = rand() % 10;
        char    str[8];
        sprintf(str, "%d", r);
        mq_receive(mqrecv, rcvmsg, MSG_SIZE, NULL);
        while(*(rcvmsg + i)) {
            if (startWriting) {
                clientQueue[j] = *(rcvmsg + i);
                j++;
            }
            if (*(rcvmsg + i) == '\n') {
                startWriting = 1;
            }
            i++;
        }
        clientQueue[j] = '\0';
        mqsend = mq_open (clientQueue, O_WRONLY, 0664, &attr);
    
        if (mq_send(mqsend, str, strlen(str), 0) == -1)
            perror ("mq_send()");
            
        mq_close(mqsend);
    }

  // Close all open message queue descriptors    
    mq_close (mqsend);
    mq_close (mqrecv);  

}