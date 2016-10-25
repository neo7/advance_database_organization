#include "buffer_mgr.h"


RC
initBufferPool(BM_BufferPool *const bm, const char *const pageFileName,
		  const int numPages, ReplacementStrategy strategy,
		  void *stratData)
{
return 0;
}
RC
shutdownBufferPool(BM_BufferPool *const bm){
return 0;
}
RC
forceFlushPool(BM_BufferPool *const bm){
return 0;
}

// Buffer Manager Interface Access Pages
RC
markDirty (BM_BufferPool *const bm, BM_PageHandle *const page){
return 0;
}

RC
unpinPage (BM_BufferPool *const bm, BM_PageHandle *const page){
return 0;
}

RC
forcePage (BM_BufferPool *const bm, BM_PageHandle *const page){
return 0;
}

RC
pinPage (BM_BufferPool *const bm, BM_PageHandle *const page,
	    const PageNumber pageNum){
return 0;
      }

// Statistics Interface
PageNumber
*getFrameContents (BM_BufferPool *const bm){
return 0;
}

bool
*getDirtyFlags (BM_BufferPool *const bm){
return 0;
}

int
*getFixCounts (BM_BufferPool *const bm){
return 0;
}

int
getNumReadIO (BM_BufferPool *const bm){
return 0;
}

int
getNumWriteIO (BM_BufferPool *const bm){
return 0;
}
