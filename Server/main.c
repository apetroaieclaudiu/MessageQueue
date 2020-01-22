#include <mqueue.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <signal.h>

#define MSG_SIZE            4096
#define CLIENT_QUEUE_NAME   "Client"
#define SERVER_QUEUE_NAME   "Server"
// This handler will be called when the queue 
// becomes non-empty.

char buf[MSG_SIZE];              // A good-sized buffer
mqd_t mqrecv, mqsend;    
void handler (int sig_num) {
    if (mq_receive (mqrecv, &buf[0], MSG_SIZE, NULL) != -1) {
        printf("The token recieved is: %s", buf);
    }
    else if (errno != EAGAIN) { 
      perror ("mq_receive()");
      _exit (EXIT_FAILURE);
    }
}

void main () {

    struct mq_attr attr, old_attr;   // To store queue attributes
    struct sigevent sigevent;        // For notification
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
    mqrecv = mq_open (CLIENT_QUEUE_NAME, O_RDONLY | O_CREAT, 
                    0664, &old_attr);
    // Get the attributes for server queue
    mq_getattr (mqsend, &attr);

    // We want to be notified when something is there 
    signal (SIGUSR1, handler);
    sigevent.sigev_signo = SIGUSR1;

    if (mq_notify (mqrecv, &sigevent) == -1) {
    if (errno == EBUSY) 
        printf (
        "Another process has registered for notification.\n");
    _exit (EXIT_FAILURE);
    }

    while (1) {
        char *msg = "Token Request\n";
        strcat(msg, CLIENT_QUEUE_NAME);
        int sleepTime = rand() % 10;
        sleep(sleepTime);
        if (mq_send (mqsend, msg, strlen(msg), 0) == -1)
            perror ("mq_send()");
    }

  // Close all open message queue descriptors    
    mq_close (mqsend);
    mq_close (mqrecv);  

}