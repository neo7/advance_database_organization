#include "caching_mechanism.h"
#include "buffer_mgr.h"

void
fifo (BM_BufferPool *const bm, Frame *freshframe)
{
    Frame *frames = (Frame *)bm->mgmtData;
    Stats *stat = (Stats *)bm->statData;
    int eviction_index = stat->lastposition%bm->numPages;

    int i;
    for (int i=0; i<bm->numPages; i++)
    {
      if(frames[eviction_index].fixedcount==0)
      {
          if(frames[eviction_index].dirtybit == DIRTY_FLAG)
          {
            writeBlockToPage(bm, &frames[eviction_index]);
          }
          copyNewFrameInOld(&frames[eviction_index], freshframe);
          break;
      }
      else
      {
        eviction_index = eviction_index + 1;
        if(eviction_index%bm->numPages == 0)
        {
          eviction_index = 0;
        }
      }

    }
}

void
lru (BM_PageHandle *const page, Frame *frame)
{


}

void copyNewFrameInOld(Frame *oldframe, Frame *freshframe)
{
  oldframe->pagedata = freshframe->pagedata;
  oldframe->pagenum = freshframe->pagenum;
  oldframe->dirtybit = freshframe->dirtybit;
  oldframe->fixedcount = freshframe->fixedcount;

}
