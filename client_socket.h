
#include <netinet/in.h>
#include <arpa/inet.h>

/**
 *
 * get socket address from domain name
 *
 **/
int getSockAddrFromDomainName(char *hostname, struct sockaddr_in *serverAddr);

/**
 * 
 * create sockets in batch to Write
 *
 **/
int createSocketInBatch (int socketArray[], int socketArraySize, struct sockaddr_in *serverAddr);


/**
 * 
 * close sockets in batch 
 *
 **/
int
closeSocketInBatch(int socketArray[], int socketArraySize);

