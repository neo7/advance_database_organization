                  Buffer Manager Assignment
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
buffer_mgr.c
buffer_mgr.h
buffer_mgr_stat.c
buffer_mgr_stat.h
caching_mechanism.c
caching_mechanism.h
dt.h
dberror.c
dberror.h
storage_mgr.c
storage_mgr.h
test_assign1_1.c
test_assign1_2.c
test_assign2_2.c
test_helper.h
Note: There is an additional test file for this assignment.
------------------------------------------------------------------------------------------------------------------

Changes done in each method:
----------------------------
NAME : 		    InitBufferPool (BM_BufferPool *const bm, const char *const pageFileName,
		  const int numPages, ReplacementStrategy strategy, void *stratData)

[1]  Takes a BM_BufferPool information as arguments.
[2]  Throws error if page file name is null or number of pages is less than 0.
[3]  Creates new status object to store real-time status information.
[3]  Initializes members of BM_BufferPool.
[4]  Creates memory space as much as Frame size by number of pages.
[5]  Initializes all frames with default values.
[6]  Sets hit, read and write count to 0.


NAME :		    shutdownBufferPool (BM_BufferPool *const bm)

[1]  Takes a BM_BufferPool object.
[2]  Calls forcdFlushPool to write all of the dirty frames.
[3]  Iterates all pages in buffer and check whether the pages are being referenced or not.
[3]  Set isPageInBuffer flag true if number of reference to page is not 0.
[4]  Throws error if Page is not in buffer


NAME :		    forceFlushPool (BM_BufferPool *const bm)

[1]  Takes an object of BM_BufferPool to write all dirty frames.
[2]  Throws RC_BUFFER_NOTINITIALIZED error when object is empty.
[3]  Gets status information from buffer object.
[4]  If page is dirty and not being referenced, write block to page.


NAME :		    makeDirty (BM_BufferPool *const bm, BM_PageHandle *const page)

[1]  Take buffer pool object and a page from BM_BufferPool and BM_PageHandle.
[2]  Throws RC_BUFFER_NOTINITIALIZED error if management data is empty.
[3]  If the page is already dirty, throws RC_MARK_DIRTY_ERROR error.
[4]  Otherwise, makes page dirty.
[5]  Checks whether finding a page to be dirty was successful or not.
[6]  If not, throws RC_MARK_DIRTY_ERROR.


NAME :		    unpinPage (BM_BufferPool *const bm, BM_PageHandle *const page)

[1]  Takes a BM_BufferPool object and a page from BM_PageHandle.
[2]  Throws RC_BUFFER_NOTINITIALIZED error if management data is empty.
[3]  Iterates buffers and decreases reference count of frames.
[4]  Makes page to be unpinned from buffer.
[5]  If unpinning was not successful, throws RC_UNPIN_PAGE_ERROR.


NAME :		    forcePage (BM_BufferPool *const bm, BM_PageHandle *const page)

[1]  Throws RC_BUFFER_NOTINITIALIZED if buffer management data is empty.
[2]  If number of pages is invalid or page number is bigger than number of pages.
[3]  Iterates buffer and writes all available block to page by calling 'writeBlockToPage'.
[4]  Throws error if writing was not successful.


NAME :		    pinPage (BM_BufferPool *const bm, BM_PageHandle *const page, const PageNumber pageNum)

[1]  Gets buffer management data and status data and starts interation.
[2]  Pin pages into frames in buffer, only if frames are initial state.
     (Opens page flie, stores page data, reads blocks, store page number,
      set reference count, last position and read count 1, stores current status and decreases ranking for pages.
[3]  Otherwise, write block to page when page is dirty. Then, set reference count, last position, hit count into 1 and decreases ranking for page.
[4]  After iterating buffer pool, makes new frame which will be inserted into buffer.
[5]  Make new frame ready to be inserted into buffer.
[6]  Caches new frame into buffer accroding to the desired strategy.


NAME :		    getFameContents (BM_BufferPool *const bm)

[1]  Gets Buffer Pool information.
[2]  Makes an array to store information of buffer pool.
[3]  Copy buffer pool content into the array iterating buffer pool.
[4]  Returns Array of page number.


NAME :		    getDirtyFlags (BM_BufferPool *const bm)

[1]  Returns false if buffer management data is empty.
[2]  Creates an array to store dirty bits of pages in buffer.
[3]  Store dirty bit information into array with regards to dirty bit.
[4]  Returns Array of dirty bits.


NAME :		    getFixCounts (BM_BufferPool *const bm)

[1]  Takes Buffer Pool information.
[2]  Creates an array to store number of reference toward pages in buffer.
[3]  Copy number of being referenced into the array.
[4]  Returns array of fixed count.


NAME :		    getNumReadIO (BM_BufferPool *const bm)

[1]  Gets status data of buffer pool.
[2]  Throws RC_BUFFER_NOTINITIALIZED if buffer management data is empty.
[3]  Otherwise, gets read count from management data and returns number of read count.


NAME :		    getNumWriteIO (BM_BufferPool *const bm)

[1]  Gets status data of buffer pool.
[2]  Throws RC_BUFFER_NOTINITIALIZED if buffer management data is empty.
[3]  Otherwise, gets write count from management data returns write count.


NAME writeBlockToPage (BM_BufferPool *const bm, Frame *frame)

[1]  Gets an object of BufferPool and frame.
[2]  Opens Page File and write Block to disk.
[3]  Makes page to not be dirty.
[4]  Increases write count.
[5]  Store current status into buffer pool.

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