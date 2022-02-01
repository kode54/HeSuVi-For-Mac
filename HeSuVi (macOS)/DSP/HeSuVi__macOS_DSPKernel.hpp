//
//  HeSuVi__macOS_DSPKernel.hpp
//  HeSuVi (macOS)
//
//  Created by Christopher Snowhill on 31 Jan 2022.
//

#ifndef HeSuVi__macOS_DSPKernel_hpp
#define HeSuVi__macOS_DSPKernel_hpp

#import <Accelerate/Accelerate.h>
#include "DSPKernel.hpp"
#import "HeadphoneFilter.h"
#include <vector>
#include <limits>
#include <thread>
#include <mutex>

/*
 HeSuVi__macOS_DSPKernel
 Performs simple copying of the input signal to the output.
 As a non-ObjC class, this is safe to use from render thread.
 */
class HeSuVi__macOS_DSPKernel : public DSPKernel {
public:
    
    // MARK: Member Functions

    HeSuVi__macOS_DSPKernel() {}

    ~HeSuVi__macOS_DSPKernel() {
        // Block until the DSP code finishes.
        doingDSP.lock();
        hFilter = nil;
    }

    void init(int inChannelCount, int outChannelCount, double inSampleRate) {
        inChanCount = inChannelCount;
        outChanCount = outChannelCount;
        sampleRate = inSampleRate;
        int maxChanCount = (inChanCount > 8) ? 8 : inChanCount;
        NSURL * url = [[NSBundle mainBundle] URLForResource:@"gsx" withExtension:@"wv"];
        hFilter = [[HeadphoneFilter alloc] initWithImpulseFile:url forSampleRate:sampleRate withInputChannels:maxChanCount];
    }

    void reset() {
        [hFilter reset];
    }

    bool isBypassed() {
        return bypassed;
    }

    void setBypass(bool shouldBypass) {
        bypassed = shouldBypass;
    }

    void setParameter(AUParameterAddress address, AUValue value) {
    }

    AUValue getParameter(AUParameterAddress address) {
        return 0.0f;
    }
    
    double getLatency() {
        return 0.0;
    }

    void setBuffers(AudioBufferList* inBufferList, AudioBufferList* outBufferList) {
        inBufferListPtr = inBufferList;
        outBufferListPtr = outBufferList;
    }

    void process(AUAudioFrameCount frameCount, AUAudioFrameCount bufferOffset) override {
        // Acquire a lock on the doingDSP mutex until this variable goes out of
        // scope, to prevent the destructor from destructing during DSP.
        std::lock_guard<std::mutex> guard(doingDSP);
        if (bypassed) {
            // Don't do any DSP on the signal when this AU is in bypass mode.
            const int chanCount = (inChanCount > outChanCount) ? outChanCount : inChanCount;
            for (int channel = 0; channel < chanCount; ++channel) {
                if (inBufferListPtr->mBuffers[channel].mData ==  outBufferListPtr->mBuffers[channel].mData) {
                    continue;
                }

                const float* in  = (float*)inBufferListPtr->mBuffers[channel].mData + bufferOffset;
                float* out = (float*)outBufferListPtr->mBuffers[channel].mData + bufferOffset;

                cblas_scopy(frameCount, in, 1, out, 1);
            }
            return;
        }
        
        int maxChanCount = (inChanCount > 8) ? 8 : inChanCount;

        float inBuffer[frameCount * maxChanCount];
        float outBuffer[frameCount * 2];

        for (int channel = 0; channel < maxChanCount; ++channel) {
            const float* in = (float*)inBufferListPtr->mBuffers[channel].mData + bufferOffset;

            cblas_scopy(frameCount, in, 1, inBuffer + channel, maxChanCount);
        }

        [hFilter process:inBuffer sampleCount:frameCount toBuffer:outBuffer];

        int minChanCount = (outChanCount < 2) ? outChanCount : 2;
        
        for (int channel = 0; channel < minChanCount; ++channel) {
            float* out = (float*)outBufferListPtr->mBuffers[channel].mData + bufferOffset;

            cblas_scopy(frameCount, &outBuffer[channel], 2, out, 1);
        }

        for (int channel = 2; channel < outChanCount; ++channel) {
            float* out = (float*)outBufferListPtr->mBuffers[channel].mData + bufferOffset;

            vDSP_vclr(out, 1, frameCount);
        }
    }

    // MARK: Member Variables

private:
    int inChanCount = 0;
    int outChanCount = 0;
    double sampleRate = 48000.0;
    bool bypassed = false;
    HeadphoneFilter * hFilter;
    AudioBufferList* inBufferListPtr = nullptr;
    AudioBufferList* outBufferListPtr = nullptr;
    // Avoid deallocating resources while the DSP code is running.
    std::mutex doingDSP;
};

#endif /* HeSuVi__macOS_DSPKernel_hpp */
