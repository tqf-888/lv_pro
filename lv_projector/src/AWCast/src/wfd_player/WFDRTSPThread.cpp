#define LOG_TAG "WFDRTSPThread"

#include "WFDRTSPThread.h"

#include "cdx_log.h"

//using namespace android;

WFDRTSPThread::WFDRTSPThread(WFDSink* sink, int enableHDCP, char* rtspURL, FeedDataCB1 feedDataCB, RTSPStatusHandler1 rtspStatusHandler, void* wfdManager)
    : mSink(sink),
      mEnableHDCP(enableHDCP),
      mRTSPURL(rtspURL),
      mFeedDataCB(feedDataCB),
      mRTSPStatusHandler(rtspStatusHandler),
      mWFDManager(wfdManager) {
    CDX_LOGD("WFDRTSPThread");
}

bool WFDRTSPThread::threadLoop() {
    int ret = startWFDSink(mSink, mEnableHDCP, mRTSPURL, mFeedDataCB, mRTSPStatusHandler, mWFDManager);
    CDX_LOGD("end WFDRTSPThread, ret %d", ret);

    return false;
}
