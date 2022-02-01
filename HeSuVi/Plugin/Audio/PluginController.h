/* PluginController */

#import <Cocoa/Cocoa.h>

#import "Plugin.h"

//Singletonish
@interface PluginController : NSObject <CogPluginController>
{
	NSMutableDictionary *sources;

	NSMutableDictionary *decodersByExtension;
	NSMutableDictionary *decodersByMimeType;
    
	BOOL configured;
}

@property(retain) NSMutableDictionary *sources;

@property(retain) NSMutableDictionary *decodersByExtension;
@property(retain) NSMutableDictionary *decodersByMimeType;

@property BOOL configured;

- (void)setup;
- (void)printPluginInfo;

- (void)loadPlugins; 
- (void)loadPluginsAtPath:(NSString *)path;

- (void)setupSource:(NSString *)className;
- (void)setupDecoder:(NSString *)className;

@end
