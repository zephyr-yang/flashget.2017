#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/**
 * 
 * create the files in batch
 *
 **/
int
createWRFileInBatch (int fileHandlerArray[], int fileHandlerArraySize)
{

  int retValue = 0;

  if (fileHandlerArray != NULL && fileHandlerArraySize > 0)
    {

      for (int i = 0; i < fileHandlerArraySize; i++)
	{

	  fileHandlerArray[i] = -1;
	}

      for (int i = 0; i < fileHandlerArraySize; i++)
	{

	  char buffer[64];
	  int n = sprintf (buffer, "tmp.%03d", i);

	  fileHandlerArray[i] = open (buffer, O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
	}
    }

  return retValue;
}

/**
 * 
 * create the files in batch
 *
 **/
int
createRDFileInBatch (int fileHandlerArray[], int fileHandlerArraySize)
{

  int retValue = 0;

  if (fileHandlerArray != NULL && fileHandlerArraySize > 0)
    {

      for (int i = 0; i < fileHandlerArraySize; i++)
	{

	  fileHandlerArray[i] = -1;
	}

      for (int i = 0; i < fileHandlerArraySize; i++)
	{

	  char buffer[64];
	  int n = sprintf (buffer, "tmp.%03d", i);

	  fileHandlerArray[i] = open (buffer, O_RDONLY);
	}
    }

  return retValue;
}

/**
 * 
 *  close the files in batch
 * 
 **/
int
closeFileInBatch (int fileHandlerArray[], int fileHandlerArraySize)
{

  int retValue = 0;

  if (fileHandlerArray != NULL && fileHandlerArraySize > 0)
    {

      for (int i = 0; i < fileHandlerArraySize; i++)
	{

	  int fd = fileHandlerArray[i];
	  if (fd >= 0)
	    {

	      close (fd);

	      fileHandlerArray[i] = -1;
	    }
	}
    }

  return retValue;
}

/**
 * 
 * create target file (parent file)
 * 
 **/
int
createTargetFile (char *fileName)
{

  int retValue = -1;

  retValue = open (fileName, O_WRONLY | O_CREAT | O_APPEND, S_IRWXU);

  return retValue;
}

int
closeTargetfile (int fd)
{

  int retValue = 0;

  if (fd > -1)
    {
      close (fd);
    }

  return retValue;
}

/**
 * 
 * copy all children file to a parent file in sequence.
 * 
 **/
int
mergeFile (int targetFileHandler, int fileHandlerArray[],
                 int fileHandlerArraySize, int useWindowsStyle)
{
    int retValue = 0;

	printf("\nmerging");

    if (fileHandlerArray != NULL && fileHandlerArraySize > 0
            && targetFileHandler > 0)
    {

        char buffer[8096];

        for (int i = 0; i < fileHandlerArraySize; i++)
        {

            int childFd = fileHandlerArray[i];

			long writeCount = 0;

            if (childFd >= 0)
            {
                char buf[8192];

				int foundNewLine = 0;

                while (1)
                {

                    ssize_t result = read (childFd, &buf[0], sizeof (buf));

                    if (!result)
                        break;

                   	//fprintf(stderr, " the read result is %d \n", result);

					if(foundNewLine){
                    	int count = write (targetFileHandler, &buf[0], result);

						writeCount += count;
						printf(".");
					}else{

						for(size_t j=0; j< result -4 ; j++){

							if(useWindowsStyle){
			
								if(buf[j] == '\r' && buf[j+1] == '\n' && buf[j+2] == '\r' && buf[j+3] == '\n'){

					               	//printf ("windows %d found end at %d \n", i, j);
				
									int count = write(targetFileHandler, &buf[j+4], (result - j - 4));

									foundNewLine = 1;

									writeCount += count;
									break;								
								}

							}else{					            

								if(buf[j] == '\n' && buf[j+1] == '\n'){

									//printf (" linux %d found end at %d \n", i, j);
				
									int count = write(targetFileHandler, &buf[j+2], (result - j - 2));

									foundNewLine = 1;								

									writeCount += count;
									break;
								}
							}
						}										
					}
                }

               	//printf (" %d => %ld \n", i, writeCount);
            }
        }
    }

    return retValue;
}



