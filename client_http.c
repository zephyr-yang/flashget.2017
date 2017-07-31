#include <stdio.h>
#include <sys/select.h>

#define SOCK_BUFFER_SIZE 8192
#define FILE_BUFFER_SIZE 4096


long
queryResourceSize(int socket, char *path, char *host, int useWindowStyle) {

    long retValue = 0;

    char *windowsNewLine = "\r\n";
    char *linuxNewLine = "\n";

    char *newLine = useWindowStyle ? windowsNewLine : linuxNewLine;

    if (path != NULL && host != NULL && socket >= 0) {

        int size = SOCK_BUFFER_SIZE;

        char buffer[SOCK_BUFFER_SIZE];
        char *cursor = buffer;

        int offset = 0;

        offset = sprintf(cursor, "HEAD %s HTTP/1.1%s", path, newLine);
        cursor += offset;

        offset = sprintf(cursor, "User-Agent: Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 6.0; Trident/4.0)%s", newLine);
        cursor += offset;

        offset = sprintf(cursor, "Connection: close%s", newLine);
        cursor += offset;

        offset = sprintf(cursor, "Host: %s%s", host, newLine);
        cursor += offset;

        offset = sprintf(cursor, "%s", newLine);
        cursor += offset;

        *cursor = '\0';

        if (socket >= 0) {

            //Connect to remote server
            printf("\nprobing...");

            int count = write(socket, buffer, (cursor - buffer));

            if (count < 0) {
                fprintf(stderr, "write: %d : %s\n", count, gai_strerror(count));
            }

            cursor = buffer;

            do {
                fd_set read_set;
                struct timeval timeout;

                timeout.tv_sec = 10; // Time out after a minute
                timeout.tv_usec = 0;

                FD_ZERO(&read_set);
                FD_SET(socket, &read_set);

                int r=select(socket+1, &read_set, NULL, NULL, &timeout);

                if( r<0 ) {
                    // Handle the error
                    fprintf(stderr, "poll errorrom socket %d \n", socket);
                }

                if( r==0 ) {
                    // Timeout - handle that. You could try waiting again, close the socket...
                    fprintf(stderr, "tiemout rom socket %d \n", socket);
                }

                if( r>0 ) {

                    // The socket is ready for reading - call read() on it.

                    if(size > 0) {

                        count = read(socket, cursor, size);

                        if(count > 0) {

                            cursor += count;
                            size -= count;
                        }

                        if(count == 0) {

                            *cursor = '\0';
                            break;
                        }
                    }
                }

            } while(0);

            // extract the content-length;
            char *contentLengthLine = strstr(buffer, "Content-Length:");
            if(contentLengthLine != NULL) {

                // find line.
                long contentLength = 0;
                sscanf(contentLengthLine, "Content-Length: %ld\n*", &contentLength);

                if(contentLength > 0) {
                    retValue = contentLength;
                }
            }
        }
    }

    return retValue;
}

int
getResource(int socketArray[], int fileHandlerArray[], int rangeCount, char *path, char *host, long contentLength, int useWindowStyle) {

    int retValue = 0;

    char *windowsNewLine = "\r\n";
    char *linuxNewLine = "\n";

    char *newLine = useWindowStyle ? windowsNewLine : linuxNewLine;

    if (path != NULL && host != NULL && socketArray != NULL && fileHandlerArray != NULL) {

        char writeBuffer[SOCK_BUFFER_SIZE];
        int size = SOCK_BUFFER_SIZE;

        // send out the requests

        for(int i=0; i<rangeCount; i++) {

            char *cursor = writeBuffer;

            int offset = 0;

            long begin = contentLength / rangeCount * i;
            long end = contentLength / rangeCount * (i +1) - 1;

            if(i == rangeCount -1) {
                end = contentLength -1;
            }

            printf("output range %ld -- %ld \n", begin, end);

            offset = sprintf(cursor, "GET %s HTTP/1.1%s", path, newLine);
            cursor += offset;

            offset = sprintf(cursor, "Host: %s%s", host, newLine);
            cursor += offset;

            offset = sprintf(cursor, "Range: bytes=%ld-%ld%s", begin, end, newLine);
            cursor += offset;

            offset = sprintf(cursor, "Connection: close%s", newLine);
            cursor += offset;

            offset = sprintf(cursor, "%s", newLine);
            cursor += offset;

            *cursor = '\0';

            int sock = socketArray[i];

            if (sock >= 0) {

                // write to remote server

                int count = write(sock, writeBuffer, (cursor - writeBuffer));

                if (count < 0) {
                    fprintf(stderr, "write: %d : %s\n", count, gai_strerror(count));
                }
            }
        }

        printf("\ndownloading");

        long recvSocketBitmap = 0;
        for(int i=0; i<rangeCount; i++) {

            recvSocketBitmap |= 1<<i;
        }

        do {

            fd_set read_set;
            struct timeval timeout;

            timeout.tv_sec = 10; // Time out after a minute
            timeout.tv_usec = 0;

            FD_ZERO(&read_set);

            int maxFd = -1;
            for(int i=0; i<rangeCount; i++) {
                FD_SET(socketArray[i], &read_set);

                if(socketArray[i] > maxFd) {
                    maxFd = socketArray[i];
                }
            }

            int r=select(maxFd+1, &read_set, NULL, NULL, &timeout);

            if( r<0 ) {

                // Handle the error
                fprintf(stderr, "poll errorrom socket \n");
            }

            if( r==0 ) {
                // Timeout - handle that. You could try waiting again, close the socket...
                fprintf(stderr, "tiemout rom socket \n");
            }

            if( r>0 ) {

                // The socket is ready for reading - call read() on it.

                for(int i=0; i<rangeCount; i++) {

                    int sock = socketArray[i];
                    int fd = fileHandlerArray[i];

                    if(FD_ISSET(sock, &read_set)) {

                        char rdBuffer[FILE_BUFFER_SIZE];
                        int count;

                        count = read(sock, rdBuffer, FILE_BUFFER_SIZE);

                        if(count > 0) {

                            int wcount = write(fd, rdBuffer, count);

                            // show the progress.
                            printf(".");
                        }

                        if(count == 0) {
                            recvSocketBitmap &= ~(1<<i);
                        }
                    }

                }// end of for.

            } // end of if

        } while(recvSocketBitmap != 0); // end of while
    }

    return retValue;
}
