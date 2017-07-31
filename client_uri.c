#include <stdio.h>

int
extractHostFromUri(char *resourceUri, char *host, int size) {

    if(resourceUri != NULL && host != NULL) {

        char *ptr = resourceUri;

        int len = 0;

        while( *ptr != '\0' && len < size) {

            int offset = ptr -resourceUri;

            if(*ptr != '/' && offset > 6) {

                host[len++] = *ptr;
            }

            if(*ptr == '/' && (ptr-resourceUri) > 7) {

                host[len] = '\0';

                break;
            }

            ptr++;
        }
    }

    return 0;
}

int
extractPathFromUri(char *resourceUri, char *path, int size) {

    if(resourceUri != NULL && path != NULL) {

        char *ptr = resourceUri;

        int len = 0;

        int findPath = 0;

        while( *ptr != '\0' && len < size) {

            int offset = ptr -resourceUri;

            if(findPath == 1) {

                path[len++] = *ptr;
            }

            if(*ptr == '/' && (ptr-resourceUri) > 7 && findPath == 0) {

                path[len++] = *ptr;

                findPath = 1;
            }

            ptr++;
        }

        if(len < size) {
            path[len] = '\0';
        } else {
            path[size-1] = '\0';
        }
    }

    return 0;
}

int
extractFilenameFromUri(char *resourceUri, char *filename, int size) {

    if(resourceUri != NULL && filename != NULL) {

        char *ptr = resourceUri;

        int len = 0;

        int findFile = 0;

        while(*ptr != '\0' && len < size) {

            if(findFile == 1) {
                filename[len++] = *ptr;
            }

            if(*ptr == '/') {
                findFile = 1;
                len =0;
            }

            ptr++;
        }

        if(len < size) {
            filename[len] = '\0';
        } else {
            filename[size-1] = '\0';
        }
    }

    return 0;
}

