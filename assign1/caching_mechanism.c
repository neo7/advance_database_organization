#include "caching_mechanism.h"
#include "buffer_mgr.h"

void
fifo (BM_BufferPool *const bm, Frame *freshframe)
{
    Frame *frames = (Frame *)bm->mgmtData;
    Stats *stat = (Stats *)bm->statData;
    //printf("%d\n", stat->lastposition);
    int eviction_index = stat->lastposition%bm->numPages;
    stat->lastposition = stat->lastposition + 1;
    int i;
    for (i=0; ; i++)
    {
      if(frames[eviction_index].fixedcount==0)
      {   if(frames[eviction_index].dirtybit == DIRTY_FLAG)
          {
            writeBlockToPage(bm, &frames[eviction_index]);
          }
          //copyNewFrameInOld(&frames[eviction_index], freshframe);
          frames[eviction_index].pagedata = freshframe->pagedata;
          frames[eviction_index].pagenum = freshframe->pagenum;
          frames[eviction_index].dirtybit = freshframe->dirtybit;
          frames[eviction_index].fixedcount = freshframe->fixedcount;
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
  frames[eviction_index].pagedata = freshframe->pagedata;
  frames[eviction_index].pagenum = freshframe->pagenum;
  frames[eviction_index].dirtybit = freshframe->dirtybit;
  frames[eviction_index].fixedcount = freshframe->fixedcount;

  decreaseRankingForPages(bm, eviction_index);

}


//get index from pin page.
void
decreaseRankingForPages(BM_BufferPool *const bm, int frameindex)
{
  printf("frameidx %d\n", frameindex);
  Frame *frames = (Frame *) bm->mgmtData;
  int i;
  for (i = 0; i<bm->numPages; i++)
  {
    if (i != frameindex || frames[i].pagenum != -1)
    {

      frames[i].ranking = frames[i].ranking - 1;
      printf("ranking other frames %d\n", frames[i].ranking);
    }
    if (i == frameindex)
    {
      frames[i].ranking = INT_MAX;
      printf("replaced frame rank %d\n", frames[i].ranking);
    }
  }

}
