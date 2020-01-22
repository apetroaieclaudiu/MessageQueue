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
    char*   clientQueue = "Client";
    int r = rand() % 10;
    char    str[8];
    sprintf(str, "%d", r);
    strcat(clientQueue, str);
    struct  mq_attr attr, old_attr;   // To store queue attributes
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
    mqsend = mq_open (SERVER_QUEUE_NAME, O_WRONLY, 
                    0664, &attr);
    mqrecv = mq_open (clientQueue, O_RDONLY | O_CREAT, 
                    0664, &old_attr);
    // Get the attributes for server queue
    mq_getattr (mqsend, &attr);

    while (1) {
        char *msg = "Token Request\n";
        strcat(msg, clientQueue);
        int sleepTime = rand() % 10;
        sleep(sleepTime);
        if (mq_send(mqsend, msg, strlen(msg), 0) == -1)
            perror ("mq_send()");

        mq_receive(mqrecv, buf, MSG_SIZE, NULL);
        printf("Received the token with number: %d\n", buf);
    }

  // Close all open message queue descriptors    
    mq_close (mqsend);
    mq_close (mqrecv);  

}