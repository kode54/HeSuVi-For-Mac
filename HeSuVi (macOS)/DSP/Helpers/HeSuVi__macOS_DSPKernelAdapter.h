//
//  HeSuVi__macOS_DSPKernelAdapter.h
//  HeSuVi (macOS)
//
//  Created by Christopher Snowhill on 31 Jan 2022.
//

#import <AudioToolbox/AudioToolbox.h>

@class AudioUnitViewController;

NS_ASSUME_NONNULL_BEGIN

@interface HeSuVi__macOS_DSPKernelAdapter : NSObject

@property (nonatomic) AUAudioFrameCount maximumFramesToRender;
@property (nonatomic, readonly) AUAudioUnitBus *inputBus;
@property (nonatomic, readonly) AUAudioUnitBus *outputBus;
@property (nonatomic, readonly) double latency;

- (void)setParameter:(AUParameter *)parameter value:(AUValue)value;
- (AUValue)valueForParameter:(AUParameter *)parameter;

- (void)allocateRenderResources;
- (void)deallocateRenderResources;
- (AUInternalRenderBlock)internalRenderBlock;

@end

NS_ASSUME_NONNULL_END
