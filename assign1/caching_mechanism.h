#ifndef CACHING_MECHANISM_H
#define CACHING_MECHANISM_H

#include "buffer_mgr.h"

// method definitions of caching mechanisms
void fifo (BM_BufferPool *const bm, Frame *frame);
void lru (BM_PageHandle *const page, Frame *Frame);
void copyNewFrameInOld(Frame *oldframe, Frame *freshframe);

#endif
