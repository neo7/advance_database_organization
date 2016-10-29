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
NAME : 	InitBufferPool (BM_BufferPool *const bm, const char *const pageFileName, const int numPages, ReplacementStrategy strategy, void *stratData)

[1]  Takes a BM_BufferPool information as arguments.
[2]  Throws error if page file name is null or number of pages is less than 0.
[3]  Creates new status object to store real-time status information.
[3]  Initializes members of BM_BufferPool.
[4]  Creates memory space as much as Frame size by number of pages.
[5]  Initializes all frames with default values.
[6]  Sets hit, read and write count to 0.


NAME : shutdownBufferPool (BM_BufferPool *const bm)

[1]  Takes a BM_BufferPool object.
[2]  Calls forcdFlushPool to write all of the dirty frames.
[3]  Iterates all pages in buffer and check whether the pages are being referenced or not.
[3]  Set isPageInBuffer flag true if number of reference to page is not 0.
[4]  Throws error if Page is not in buffer


NAME : forceFlushPool (BM_BufferPool *const bm)

[1]  Takes an object of BM_BufferPool to write all dirty frames.
[2]  Throws RC_BUFFER_NOTINITIALIZED error when object is empty.
[3]  Gets status information from buffer object.
[4]  If page is dirty and not being referenced, write block to page.


NAME : makeDirty (BM_BufferPool *const bm, BM_PageHandle *const page)

[1]  Take buffer pool object and a page from BM_BufferPool and BM_PageHandle.
[2]  Throws RC_BUFFER_NOTINITIALIZED error if management data is empty.
[3]  If the page is already dirty, throws RC_MARK_DIRTY_ERROR error.
[4]  Otherwise, makes page dirty.
[5]  Checks whether finding a page to be dirty was successful or not.
[6]  If not, throws RC_MARK_DIRTY_ERROR.


NAME : unpinPage (BM_BufferPool *const bm, BM_PageHandle *const page)

[1]  Takes a BM_BufferPool object and a page from BM_PageHandle.
[2]  Throws RC_BUFFER_NOTINITIALIZED error if management data is empty.
[3]  Iterates buffers and decreases reference count of frames.
[4]  Makes page to be unpinned from buffer.
[5]  If unpinning was not successful, throws RC_UNPIN_PAGE_ERROR.


NAME : forcePage (BM_BufferPool *const bm, BM_PageHandle *const page)

[1]  Throws RC_BUFFER_NOTINITIALIZED if buffer management data is empty.
[2]  If number of pages is invalid or page number is bigger than number of pages.
[3]  Iterates buffer and writes all available block to page by calling 'writeBlockToPage'.
[4]  Throws error if writing was not successful.


NAME : pinPage (BM_BufferPool *const bm, BM_PageHandle *const page, const PageNumber pageNum)

[1]  Gets buffer management data and status data and starts interation.
[2]  Pin pages into frames in buffer, only if frames are initial state.
   (Opens page flie, stores page data, reads blocks, store page number,
    set reference count, last position and read count 1, stores current status and decreases ranking for pages.
[3]  Otherwise, write block to page when page is dirty. Then, set reference count, last position, hit count into 1 and decreases ranking for page.
[4]  After iterating buffer pool, makes new frame which will be inserted into buffer.
[5]  Make new frame ready to be inserted into buffer.
[6]  Caches new frame into buffer accroding to the desired strategy.


NAME : getFameContents (BM_BufferPool *const bm)

[1]  Gets Buffer Pool information.
[2]  Makes an array to store information of buffer pool.
[3]  Copy buffer pool content into the array iterating buffer pool.
[4]  Returns Array of page number.


NAME : getDirtyFlags (BM_BufferPool *const bm)

[1]  Returns false if buffer management data is empty.
[2]  Creates an array to store dirty bits of pages in buffer.
[3]  Store dirty bit information into array with regards to dirty bit.
[4]  Returns Array of dirty bits.


NAME : getFixCounts (BM_BufferPool *const bm)

[1]  Takes Buffer Pool information.
[2]  Creates an array to store number of reference toward pages in buffer.
[3]  Copy number of being referenced into the array.
[4]  Returns array of fixed count.


NAME : getNumReadIO (BM_BufferPool *const bm)

[1]  Gets status data of buffer pool.
[2]  Throws RC_BUFFER_NOTINITIALIZED if buffer management data is empty.
[3]  Otherwise, gets read count from management data and returns number of read count.


NAME : getNumWriteIO (BM_BufferPool *const bm)

[1]  Gets status data of buffer pool.
[2]  Throws RC_BUFFER_NOTINITIALIZED if buffer management data is empty.
[3]  Otherwise, gets write count from management data returns it.

-------------------------------------------------------------------------------------------------------------

Execution Instructions:
-----------------------
There is a Makefile in this assignment, make use of that to compile and run.
Step 1: unzip the file.
Step 2: go in the directory from the commandline.
Step 3: Make sure you have the standard GCC compiler installed on your system. Fourier server is a nice way to test.
Step 4: run $ make
Step 5: run $ ./test_assign2_1 to run the assignment1 test
-----------------------------------------------------------------------------------------------------------------
