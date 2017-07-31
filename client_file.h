
/**
 * 
 * create the files in batch to Write
 *
 **/
int
createWRFileInBatch (int fileHandlerArray[], int fileHandlerArraySize);

/**
 * 
 * create the files in batch to read
 *
 **/
int
createRDFileInBatch (int fileHandlerArray[], int fileHandlerArraySize);

/**
 * 
 *  close the files in batch
 * 
 **/
int
closeFileInBatch (int fileHandlerArray[], int fileHandlerArraySize);

/**
 * 
 * create target file (parent file)
 * 
 **/
int
createTargetFile (char *fileName);

/**
 *
 * close the single file.
 *
 **/
int
closeTargetfile (int fd);

/**
 * 
 * copy all children file to a parent file in sequence.
 * 
 **/
int
mergeFile (int targetFileHandler, int fileHandlerArray[],
		 int fileHandlerArraySize, int useWindowStyle);


