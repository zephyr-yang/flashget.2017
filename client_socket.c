#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

  int getSockAddrFromDomainName(char * hostname, struct sockaddr_in * server) {

    struct addrinfo hints;
    struct addrinfo * result, * rp;
    int error;
    int serverAddrIsSet = 0;

    memset( & hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_UNSPEC; /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* Stream socket */
    hints.ai_flags = AI_PASSIVE; /* For wildcard IP address */
    hints.ai_protocol = 0; /* Any protocol */
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

    error = getaddrinfo(hostname, "http", & hints, & result);

    if (error != 0) {

      fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(error));
      return error;

    } else {

      struct sockaddr_in * h;

      for (rp = result; rp != NULL; rp = rp->ai_next) {

        h = (struct sockaddr_in * ) rp->ai_addr;

        if (h-> sin_addr.s_addr != 0) {

          server-> sin_addr.s_addr = h->sin_addr.s_addr;
          server-> sin_family = h-> sin_family;
          server-> sin_port = htons(80);

          serverAddrIsSet = 1;

          break;
        }
      }
    }

    // release the resource.
    freeaddrinfo(result);

    return serverAddrIsSet == 1 ? 0 : 1;
  }

int
createSocketInBatch(int socketArray[], int socketArraySize, struct sockaddr_in * serverAddr) {

  int retValue = 0;

  if (socketArray != NULL && socketArraySize > 0) {

    for (int i = 0; i < socketArraySize; i++) {

      socketArray[i] = -1;
    }

    for (int i = 0; i < socketArraySize; i++) {

      int sock = socket(AF_INET, SOCK_STREAM, 0);
      if (sock >= 0) {

        //Connect to remote server

        int error = connect(sock, (struct sockaddr * ) serverAddr, sizeof(*serverAddr));
        if (error >= 0) {

          socketArray[i] = sock;

        } else {

          fprintf(stderr, "connect: %d : %s\n", error, gai_strerror(error));

          close(sock);

        }
      }
    }
  }

  return retValue;
}

int
closeSocketInBatch(int socketArray[], int socketArraySize) {

  int retValue = 0;

  if (socketArray != NULL && socketArraySize > 0) {

    for (int i = 0; i < socketArraySize; i++) {

      int socket = socketArray[i];
      if (socket >= 0) {

        close(socket);

      }
    }
  }

  return retValue;
}
