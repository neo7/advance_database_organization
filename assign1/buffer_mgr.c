#include "buffer_mgr.h"
#include "storage_mgr.h"
#include <stdio.h>
#include <stdlib.h>


RC
initBufferPool(BM_BufferPool *const bm, const char *const pageFileName,
		  const int numPages, ReplacementStrategy strategy,
		  void *stratData)
{

	if (pageFileName == NULL || numPages<=0)
	{
		return RC_FILE_NOT_FOUND;
	}
	stats = MAKE_STATS();
	bm->pageFile = (char *)pageFileName;
	bm->numPages = numPages;
	bm->strategy = strategy;
	Frame *frames = malloc (sizeof(*frames)*numPages);

	int pageCounter = numPages;
	int frameLoopCounter = 0;
	while (pageCounter > 0)
	{
		frames[frameLoopCounter].pagedata = NULL;
		frames[frameLoopCounter].pagenum = -1;
		frames[frameLoopCounter].dirtybit = DIRTY_UNFLAG;
		frames[frameLoopCounter].fixedcount = 0;
		frameLoopCounter++;
		pageCounter--;
	}
	bm->mgmtData = frames;

	stats->hitcount = 0;
	stats->readcount = 0;
	stats->writecount = 0;
	bm->statData = stats;
	return RC_OK;
}
RC
shutdownBufferPool(BM_BufferPool *const bm)
{
	Frame * frames = (Frame *)bm->mgmtData;
	forceFlushPool(bm);
	int i;
	bool isPageInBuffer = false;
	for (i = 0; i<bm->numPages; i++)
	{
		if(frames[i].fixedcount != 0)
		{
			isPageInBuffer = true;
		}
	}
    if (	bm->mgmtData == NULL){

        return RC_BUFFER_NOTINITIALIZED;
    }
	if (isPageInBuffer)
	{
		return RC_PAGE_IN_BUFFER_ERROR;
	}
	else
	{
		return RC_OK;
	}
}
RC
forceFlushPool(BM_BufferPool *const bm){
    if (	bm->mgmtData == NULL){

        return RC_BUFFER_NOTINITIALIZED;
    }

 Frame * frames = (Frame *) bm->mgmtData;
 Stats * stat = (Stats*) bm->statData;
 int i;
 for (i = 0; i<bm->numPages; i++)
 {
	 if (frames[i].dirtybit == DIRTY_FLAG && frames[i].fixedcount == 0)
	 {
		 writeBlockToPage(bm, &frames[i]);
	 }
 }
 return RC_OK;
}


// Buffer Manager Interface Access Pages
RC
markDirty (BM_BufferPool *const bm, BM_PageHandle *const page)
{
    if (	bm->mgmtData == NULL){

        return RC_BUFFER_NOTINITIALIZED;
    }

	Frame *frames = ((Frame *)(bm->mgmtData));
	int i;
	bool foundPageNum = false;
	for (i = 0; i<bm->numPages; i++)
	{
		if (frames[i].pagenum == page->pageNum)
		{
            if (frames[i].dirtybit == DIRTY_FLAG) {
                return RC_MARK_DIRTY_ERROR;
            }

			frames[i].dirtybit = DIRTY_FLAG;
			foundPageNum = true;
			break;
		}
	}
	if (foundPageNum == true)
	{
		return RC_OK;

	} else {
			return RC_MARK_DIRTY_ERROR;
	}
}

RC
unpinPage (BM_BufferPool *const bm, BM_PageHandle *const page){

    if (	bm->mgmtData == NULL){

        return RC_BUFFER_NOTINITIALIZED;
    }

	Frame *frames = ((Frame *)(bm->mgmtData));
	int i;
	bool foundPageNum = false;
	for (i = 0; i<bm->numPages; i++)
	{
		if (frames[i].pagenum == page->pageNum)
		{
			frames[i].fixedcount = frames[i].fixedcount - 1;
			foundPageNum = true;
			break;
		}
	}
	if (foundPageNum == true)
	{
		return RC_OK;

	} else {
			return RC_UNPIN_PAGE_ERROR;
	}
}

RC
forcePage (BM_BufferPool *const bm, BM_PageHandle *const page)
{
    int check;
    if (	bm->mgmtData == NULL){

    return RC_BUFFER_NOTINITIALIZED;
    }

    if (page->pageNum == -1 || page->pageNum > bm->numPages) {
        return RC_PAGE_ERROR;
    }

	Frame * frames = (Frame *) bm->mgmtData;
	int i;
	for (i= 0; i<bm->numPages; i++)
	{
		if (frames[i].pagenum == page->pageNum)
		{
			check = writeBlockToPage(bm, &frames[i]);
		}
	}
    if(check == 0){
        return RC_OK;
    }else{
        return RC_FORCE_POOL_ERROR;
    }
}

RC
pinPage (BM_BufferPool *const bm, BM_PageHandle *const page,
	    const PageNumber pageNum){
return 0;
      }


PageNumber
*getFrameContents (BM_BufferPool *const bm)
{
	PageNumber * pagenumberarray = malloc(sizeof(PageNumber)*bm->numPages);
	Frame * frames = (Frame *) bm->mgmtData;
	int i;
	for (i = 0; i<bm->numPages; i++)
	{
		pagenumberarray[i] = frames[i].pagenum;
	}
	return pagenumberarray;
}

bool
*getDirtyFlags (BM_BufferPool *const bm)
{
    if (	bm->mgmtData == NULL){

        return false;
    }

	bool * dirtyflagarray = malloc(sizeof(bool)*bm->numPages);
	Frame * frames = (Frame *)bm->mgmtData;
	int i;
	for (i=0; i<bm->numPages; i++)
	{
		if(frames[i].dirtybit == DIRTY_FLAG)
		{
			dirtyflagarray[i] = true;
		} else
		{
			dirtyflagarray[i] = false;
		}
	}
	return dirtyflagarray;
}

int
*getFixCounts (BM_BufferPool *const bm){


	int * fixedcountarray = malloc(sizeof(int)*bm->numPages);
	Frame * frames = (Frame *)bm->mgmtData;
	int i;
	for (i = 0; i<bm->numPages; i++)
	{
		fixedcountarray[i] = frames[i].fixedcount;
	}
	return fixedcountarray;
}

int
getNumReadIO (BM_BufferPool *const bm){

    if (	bm->mgmtData == NULL){

        return RC_BUFFER_NOTINITIALIZED;
    }

 if (bm->statData != NULL)
 {
	 Stats * stats = (Stats *) bm->statData;
	 return stats->readcount;
 } else
 {
	 return 0;
 }
}

int
getNumWriteIO (BM_BufferPool *const bm)
{
    if (	bm->mgmtData == NULL){

        return RC_BUFFER_NOTINITIALIZED;
    }

 if (bm->statData != NULL)
 {
	 Stats * stats = (Stats *) bm->statData;
	 return stats->writecount;
 } else
 {
	 return 0;
 }
}

int writeBlockToPage(BM_BufferPool *const bm, Frame *frame)
{
	SM_FileHandle fh;
	Stats * stats = (Stats *) bm->statData;
	openPageFile(bm->pageFile, &fh);
	writeBlock(frame->pagenum, &fh, frame->pagedata);
	frame->dirtybit = DIRTY_UNFLAG;
	stats->writecount++;
	bm->statData = stats;
	return RC_OK;
}
