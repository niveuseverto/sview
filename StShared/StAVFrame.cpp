/**
 * Copyright © 2013-2015 Kirill Gavrilov <kirill@sview.ru>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file license-boost.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt
 */

#include <StAV/StAVFrame.h>

#include <StAV/StAVImage.h>

StAVFrame::StAVFrame()
#if(LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(55, 45, 101))
: Frame(av_frame_alloc())
#else
: Frame(avcodec_alloc_frame())
#endif
{
    reset();
}

StAVFrame::~StAVFrame() {
#if(LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(55, 45, 101))
    av_frame_free(&Frame);
#else
    av_free(Frame);
#endif
}

void StAVFrame::reset() {
#if(LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(55, 45, 101))
    av_frame_unref(Frame);
#else
    avcodec_get_frame_defaults(Frame);
#endif
}

void StAVFrame::getImageInfo(const AVCodecContext* theCodecCtx,
                             int&           theSizeX,
                             int&           theSizeY,
                             AVPixelFormat& thePixFmt) const {
#if(LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(53, 5, 0))
    (void )theCodecCtx;
    theSizeX  = Frame->width;
    theSizeY  = Frame->height;
    thePixFmt = (AVPixelFormat )Frame->format;
#else
    theSizeX  = theCodecCtx->width;
    theSizeY  = theCodecCtx->height;
    thePixFmt = theCodecCtx->pix_fmt;
#endif
}

StAVFrameCounter::StAVFrameCounter()
: myFrame(NULL),
  myIsProxy(false) {
#if(LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(55, 45, 101))
    myFrame = av_frame_alloc();
#endif
}

StAVFrameCounter::~StAVFrameCounter() {
#if(LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(55, 45, 101))
    av_frame_free(&myFrame);
#endif
}

void StAVFrameCounter::createReference(StHandle<StBufferCounter>& theOther) const {
#if(LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(55, 45, 101))
    StHandle<StAVFrameCounter> anAvRef = StHandle<StAVFrameCounter>::downcast(theOther);
    if(anAvRef.isNull()) {
        anAvRef = new StAVFrameCounter();
        theOther = anAvRef;
    }

    av_frame_unref(anAvRef->myFrame);
    if(myIsProxy) {
        // just steal the reference
        av_frame_move_ref(anAvRef->myFrame, myFrame);
    } else {
        av_frame_ref(anAvRef->myFrame, myFrame);
    }
#else
    theOther.nullify();
#endif
}

void StAVFrameCounter::releaseReference() {
#if(LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(55, 45, 101))
    av_frame_unref(myFrame);
#endif
}

void StAVFrameCounter::moveReferenceFrom(AVFrame* theFrame) {
    myIsProxy = true;
#if(LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(55, 45, 101))
    av_frame_unref(myFrame);
    av_frame_move_ref(myFrame, theFrame);
#else
    (void )theFrame;
#endif
}
