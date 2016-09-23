#include<stdio.h>
#include<stdlib.h>
#include "storage_mgr.h"



void
initStorageManager (void)
{

}

RC
createPageFile (char *fileName)
{
  FILE *fstream;
  fstream = fopen(fileName, "w+" );
  char *filepointer = (char *) calloc(PAGE_SIZE, sizeof(char));
  size_t sizeOfFile = fwrite(filepointer, sizeof(char), PAGE_SIZE, fstream);
  fclose(fstream);
  free(fstream);
  return RC_OK;
}

RC
openPageFile (char *fileName, SM_FileHandle *fHandle){
  FILE *fstream;
  fstream = fopen(fileName, "r+");
  if (fstream == NULL)
  {
    return RC_FILE_NOT_FOUND;
  }
  else
  {
    fseek(fstream, 0L, SEEK_END);
    long file_sz = ftell(fstream);

    fHandle->fileName = fileName;
    fHandle->totalNumPages = (int)file_sz/PAGE_SIZE;
    fHandle->curPagePos = 0;
    fHandle->mgmtInfo = fstream;
  }

  return RC_OK;
}

RC
closePageFile (SM_FileHandle *fHandle)
{
  FILE *fstream = fHandle->mgmtInfo;
  if (fstream == NULL || (fstream == NULL && fHandle->fileName == NULL))
  {
    return RC_FILE_HANDLE_NOT_INIT;
  }
  else
  {
    fclose(fstream);
  }
  return RC_OK;
}

RC
destroyPageFile (char *fileName)
{
  int error_code = remove(fileName);
  if (error_code != 0)
  {
    return RC_FILE_NOT_DELETED;
  }
  return RC_OK;
}

/* reading blocks from disc */
RC
readBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage)
{
  FILE *fstream = fHandle->mgmtInfo;
  if (fstream == NULL)
  {
    return RC_FILE_NOT_FOUND;
  }
  if (fHandle->totalNumPages < pageNum)
  {
    return RC_READ_NON_EXISTING_PAGE;
  }
  size_t block_file_size = fread(memPage, PAGE_SIZE, 1, fstream);
  if(block_file_size == 0)
  {
    return RC_BLOCK_READ_ERROR;
  }
  fHandle->curPagePos = pageNum;
  return RC_OK;
}

int
getBlockPos (SM_FileHandle *fHandle)
{
  if (fHandle == NULL){
    return RC_FILE_HANDLE_NOT_INIT;
  }
  return fHandle->curPagePos;
}

RC
readFirstBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
return RC_OK;
}

RC
readPreviousBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
return RC_OK;
}

RC
readCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
return RC_OK;
}

RC
readNextBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
return RC_OK;
}

RC
readLastBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
return RC_OK;
}

/* writing blocks to a page file */
RC
writeBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage)
{
return RC_OK;
}

RC
writeCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
return RC_OK;
}

RC
appendEmptyBlock (SM_FileHandle *fHandle)
{
return RC_OK;
}

RC
ensureCapacity (int numberOfPages, SM_FileHandle *fHandle)
{
return RC_OK;
}
