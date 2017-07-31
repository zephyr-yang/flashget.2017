#include <unistd.h>

#include "client_socket.h"
#include "client_file.h"
#include "client_http.h"
#include "client_uri.h"

#define STR_LENGTH 256
#define MAX_PARALLEL_NUMBER 64

int main(int argc, char *argv[]) {

    char *uri = "http://dist.pravala.com/coding/multiGet-example.zip";

	int USE_WINDOWS_STYLE = 1;
	int USE_LINUX_STYLE = 0;

	int newLineStyle = USE_LINUX_STYLE;

	int parallelNumber = 4;

    int fileHandlerArraySize = parallelNumber;
    int socketArraySize = parallelNumber;

    int fileHandlerArray[MAX_PARALLEL_NUMBER];
    int socketArray[MAX_PARALLEL_NUMBER];

    char host[STR_LENGTH];
    char path[STR_LENGTH];
    char filename[STR_LENGTH];

    struct sockaddr_in server;

    long contentLength;

	printf("\n");

	//
	// parse incoming parameters.
	// 
	int opt;
	int countIn;
	while ((opt = getopt (argc, argv, "u:p:h")) != -1)
	{
    	switch (opt)
    	{
      		case 'u':

				uri = optarg;
				
                printf ("Network resource to fetch is %s \n", optarg);

                break;

      		case 'p':

				sscanf(optarg, "%d", &countIn);

				if(countIn > 0 && countIn < MAX_PARALLEL_NUMBER){

		    		fileHandlerArraySize = countIn;
    				socketArraySize = countIn;
				}

                printf ("Parallel number is set to %d \n", socketArraySize);
                break;

			case 'h':
		
	         	printf("Usage: %s [OPTIONS]\n", argv[0]);
         		printf("  -u uri                    optional, network resouce to download \n");
         		printf("  -p parallel number        optional, how many parrel download jobs to execute, default is 4\n");
         		printf("  -h                        print this help and exit\n");
         		printf("\n");
         		return(0);
    	}
  	}

	//
	// parse the input uri
	//
    extractHostFromUri(uri, host, STR_LENGTH);
    extractPathFromUri(uri, path, STR_LENGTH);
    extractFilenameFromUri(uri, filename, STR_LENGTH);

    printf("host = %s \n", host);
    printf("path = %s \n", path);
    printf("filename = %s \n", filename);

	//
    // get the target file size
	//
    getSockAddrFromDomainName(host, &server);
    createSocketInBatch(socketArray, 1, &server);
    contentLength = queryResourceSize(socketArray[0], path, host, newLineStyle);
    closeSocketInBatch(socketArray, 1);

    printf("\nContentLength = %ld \n" , contentLength);

	//
	// download file in parallel 
	//

    createWRFileInBatch(fileHandlerArray, fileHandlerArraySize);
    createSocketInBatch(socketArray, socketArraySize, &server);
	getResource(socketArray, fileHandlerArray, fileHandlerArraySize, path, host, contentLength, newLineStyle);
    closeFileInBatch(fileHandlerArray, fileHandlerArraySize);
    closeSocketInBatch(socketArray, socketArraySize);

	//
	// merge file.		
	//

    int targetFd = createTargetFile(filename);
    createRDFileInBatch(fileHandlerArray, fileHandlerArraySize);
    mergeFile(targetFd, fileHandlerArray, fileHandlerArraySize, USE_WINDOWS_STYLE);
    closeFileInBatch(fileHandlerArray, fileHandlerArraySize);
    closeTargetfile(targetFd);

    // end

    printf("\nEnd \n");

    return 0;
}
