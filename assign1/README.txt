                  Storage Manager Assignment
--------------------------------------------------------------------------------------------------------------------

Group Members:
--------------
Saurabh Tiwari
Karthik Shivaram
Sukrit Gulati
Seungho Han
--------------------------------------------------------------------------------------------------------------------

List of Files for the Assignment
---------------------------------
Makefile
dberror.c
dberror.h
storage_mgr.c
storage_mgr.h
test_assign1_1.c
test_assign1_2.c
test_helper.h
Note: There is an additional test file for this assignment.
------------------------------------------------------------------------------------------------------------------

Changes done in each method:
----------------------------

NAME :              createPageFile (char *fileName)

[1]  Create Empty File of File Name.
[2]  Allocate Memmory of one Page Size to this empty file and Initialize it with zero.
[3]  Check the size of the created file.
[4]  Throw error if file is of not right size.
[5]  Return RC_OK if file was created successfully of one PAGE_SIZE.


NAME :            openPageFile (char *fileName,, SM_FileHandle *fHandle)
[1]  Opens the file using File Name.
[2]  If the file stream returns null then the file does not exist.
[3]  Else we seek to the end of the file and obtain file size.
[4]  Then we initialize all the fields of SM_FileHandle.
[5]  After successful initialization we Return RC_OK.

NAME :            closePageFile (SM_FileHandle *fHandle)
[1]  Obtains current file Stream from the "mgmtinfo" from *fHandle.
[2]  Checks if the file stream is Null, or if the Filename and File Stream is null.
[3]  If NULL then we return File Handle not initialized error.
[4]  Else we close the File, using the File Stream.
[5]  After successful closing we Return RC_OK.


NAME :            destroyPageFile (char *fileName)
[1]  Deletes the file using filename.
[2]  If error occured during deleting then we return File not deleted error.
[3]  If File was deleted successfully we return RC_OK.


NAME :              readBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage)
[1]  Obtain file stream from mgmtInfo, check if the stream is null , if it is null return RC_FILE_NOT_FOUND.
[2]  Check to see the totalNumPages is greater than pageNum, if it isn't then return RC_READ_NON_EXISTING_PAGE.
[3]  Then we calculate offset to seek by, using the pageNum and then we read that page.
[4]  We check to see if read returns the proper blocksize read. If not we return  RC_BLOCK_READ_ERROR.
[5]  If no error is encountered till here then we update the current Page Position of the file.
[6]  We Return RC_OK.

NAME :              getBlockPos (SM_FileHandle *fHandle)
PROCESS :
[1]  Check to see if fHandle is initialized, if it isn't then we retun RC_FILE_HANDLE_NOT_INIT.
[2]  If it is initialized then we return the fhandle->curPagePos.

NAME :              readFirstBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
PROCESS :
[1]  Obtain file stream from mgmtInfo, check if the stream is null , if it is null return RC_FILE_NOT_FOUND.
[2]  Then we seek from the beginning. And we read the block of the file.
[3]  We check to see if read returns the proper blocksize read. If not we return  RC_BLOCK_READ_ERROR.
[4]  We Return RC_OK.

NAME :              readPreviousBlock(SM_FileHandle *fHandle, SM_PageHandle memPage)
PROCESS :
[1]  Obtain file stream from mgmtInfo, check if the stream is null , if it is null return RC_FILE_NOT_FOUND.
[2]  We check if the current Page position of the file is 0, if it is then no previous block exists and we return RC_PREV_BLOCK_NOT_EXIST
[3]  Then we calculate the offset by reducing the current page number, and we read the block.
[4]  We check to see if read returns the proper blocksize read. If not we return  RC_BLOCK_READ_ERROR.
[5]  If all methods are successful We Return RC_OK.

NAME :              readCurrentBlock(SM_FileHandle *fHandle, SM_PageHandle memPage)
PROCESS :
[1]  Obtain file stream from mgmtInfo, check if the stream is null , if it is null return RC_FILE_NOT_FOUND.
[2]  Then we calculate the offset by using current page position, and we read the block.
[3]  We check to see if read returns the proper blocksize read. If not we return  RC_BLOCK_READ_ERROR.
[4]  If all methods are successful We Return RC_OK.

NAME :              readNextBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
PROCESS :
[1]  Obtain file stream from mgmtInfo, check if the stream is null , if it is null return RC_FILE_NOT_FOUND.
[2]  Then we check to see if next block exists , ie current block is not the last block. If false we return RC_NEXT_BLOCK_NOT_EXIST.
[2]  Then we calculate the offset from current page position + 1, and we read the block.
[3]  We check to see if read returns the proper blocksize read. If not we return  RC_BLOCK_READ_ERROR.
[4]  If all methods are successful We Return RC_OK.

NAME :              readLastBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
PROCESS :
[1]  Obtain file stream from mgmtInfo, check if the stream is null , if it is null return RC_FILE_NOT_FOUND.
[2]  Then we seek to the beginning of the last block of the file, and we read the block.
[3]  We check to see if read returns the proper blocksize read. If not we return  RC_BLOCK_READ_ERROR.
[4]  If all methods are successful We Return RC_OK.

NAME :              writeBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage)
PROCESS :
[1]  Obtain file stream from mgmtInfo, check if the stream is null , if it is null return RC_FILE_NOT_FOUND.
[2]  If the pageNum is greater than totalNumPages then we try to write a page that does not exist, hence we return RC_WRITE_NON_EXISTING_PAGE
[3]  We check to see if write returns the proper blocksize read. If not we return  RC_WRITE_FAILED.
[4]  We update the curPagePos in the fHandle .
[5]  If all methods are successful We Return RC_OK.


NAME :              writeCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
PROCESS :
[1]  Obtain file stream from mgmtInfo, check if the stream is null , if it is null return RC_FILE_NOT_FOUND.
[2]  We update the offset using curPagePos. And then we write to that position.
[3]  We check to see if write returns the proper blocksize read. If not we return  RC_WRITE_FAILED.
[4]  If all methods are successful We Return RC_OK.

NAME :              appendEmptyBlock (SM_FileHandle *fHandle)
PROCESS :
[1]  we check if the fHandle has been initialized, if not we return RC_FILE_HANDLE_NOT_INIT.
[2]  We calculate the offset length by using totalNumPages and incrementing it by 1.
[3]  We truncate the file , to increase it by a block and append /0 bytes to this new appended block.
[4]  If the truncate_status is not 0 (successful) then we return RC_BLOCK_APPEND_ERROR.
[5]  If all methods are successful We Return RC_OK.

NAME :              ensureCapacity (int numberOfPages, SM_FileHandle *fHandle)
PROCESS :
[1]  we check if the fHandle has been initialized, if not we return RC_FILE_HANDLE_NOT_INIT.
[2]  We check to see if the totalNumPages of the file is less than numberOfPages, if it is we increase the file size to that of numberOfPages.
[3]  If all methods are successful We Return RC_OK.


-------------------------------------------------------------------------------------------------------------

Execution Instructions:
-----------------------
There is a Makefile in this assignment, make use of that to compile and run.
Step 1: unzip the file.
Step 2: go in the directory from the commandline.
Step 3: Make sure you have the standard GCC compiler installed on your system. Fourier server is a nice way to test.
Step 4: run $ make
Step 5: run $ ./test_assign1 to run the assignment1 test
Step 6: run $ ./test_assign1_2 to run the assignment1 additional test


Additional Test cases description:
----------------------------------
It checks readLastBlock, writeBlock and appendEmptyBlock functionality
following the analogy of the test file provided by the instructor.
First we write on the second block and read the first block to check if its null.
Then we read the last block which is the second block in our case to verify the block has been written properly.
We append an empty block and read the last block to verify that our appended block was added to the file.
We ensured capacity and checked if the capacity was increased.
-----------------------------------------------------------------------------------------------------------------
