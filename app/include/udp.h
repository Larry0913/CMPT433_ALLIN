#ifndef _UDP_H_
#define _UDP_H_

#define PORT 12345 // The port the server will listen on
#define BUFFER_MAX_SIZE 1000

void UDP_init();
void UDP_wait();
void UDP_cleanup(void);

void *UDPServerThread(void *);

void stop(void);


#endif