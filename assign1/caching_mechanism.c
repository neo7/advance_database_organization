#include "caching_mechanism.h"
#include "buffer_mgr.h"

void
fifo (BM_BufferPool *const bm, Frame *freshframe)
{
    Frame *frames = (Frame *)bm->mgmtData;
    Stats *stat = (Stats *)bm->statData;
    int eviction_index = stat->lastposition%bm->numPages;
    stat->lastposition = stat->lastposition + 1;
    int i;
    for (i=0; i<bm->numPages; i++)
    {
      if(frames[eviction_index].fixedcount==0)
      {
          if(frames[eviction_index].dirtybit == DIRTY_FLAG)
          {
            writeBlockToPage(bm, &frames[eviction_index]);
          }
          copyNewFrameInOld(&frames[eviction_index], freshframe);
          decreaseRankingForPages(bm, eviction_index);
          bm->statData = stat;
          break;
      }
      else
      {
        eviction_index = eviction_index + 1;
        printf("%s\n", "evic");
        if(eviction_index%bm->numPages == 0)
        {
          eviction_index = 0;
        }
      }
    }
}

void
lru (BM_BufferPool *const bm, Frame *freshframe)
{
  Frame *frames = (Frame *)bm->mgmtData;
  Stats *stat = (Stats *)bm->statData;
  int i;
  int eviction_index = 0;
  int minimum_rank = INT_MAX;
  //search for min rank.
  for (i = 0; i<bm->numPages; i++)
  {
    if (frames[i].ranking < minimum_rank && frames[i].fixedcount == 0)
    {
      minimum_rank = frames[i].ranking;
      eviction_index = i;
    }
  }

  if(frames[eviction_index].dirtybit == DIRTY_FLAG)
  {
    writeBlockToPage(bm, &frames[eviction_index]);
  }
  copyNewFrameInOld(&frames[eviction_index], freshframe);
  decreaseRankingForPages(bm, eviction_index);

}

void copyNewFrameInOld(Frame *oldframe, Frame *freshframe)
{
  oldframe->pagedata = freshframe->pagedata;
  oldframe->pagenum = freshframe->pagenum;
  oldframe->dirtybit = freshframe->dirtybit;
  oldframe->fixedcount = freshframe->fixedcount;

}

//get index from pin page.
void
decreaseRankingForPages(BM_BufferPool *const bm, int frameindex)
{
  Frame *frames = (Frame *) bm->mgmtData;
  int i;
  for (i = 0; i<bm->numPages; i++)
  {
    if (i != frameindex || frames[i].pagenum != -1)
    {
      frames[i].ranking = frames[i].ranking - 1;
    }
    else if (i == frameindex)
    {
      frames[i].ranking = INT_MAX;
    }
  }

}
