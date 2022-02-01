//Plugins! HOORAY!

@protocol CogSource <NSObject>
+ (NSArray *)schemes; //http, file, etc

- (NSURL *)url;
- (NSString *)mimeType;

- (BOOL)open:(NSURL *)url;
- (BOOL)seekable;
- (BOOL)seek:(long)position whence:(int)whence;
- (long)tell;
- (long)read:(void *)buffer amount:(long)amount; //reads UP TO amount, returns amount read.
- (void)close;
- (void)dealloc;
@end

@protocol CogDecoder <NSObject>
@required
+ (NSArray *)mimeTypes;
+ (NSArray *)fileTypes; //mp3, ogg, etc
+ (NSArray *)fileTypeAssociations; // array of NSArray of NSString, where first item in array is the type name, the second is the icon name, and the rest are the extensions
+ (float)priority; // should be 0.0 ... 1.0, higher means you get selected first, should default to 1.0 unless you know a reason why any of your extensions may behave badly, ie. greedily taking over some file type extension without performing any header validation on it

//For KVO
//- (void)setProperties:(NSDictionary *)p;
- (NSDictionary *)properties;

- (int)readAudio:(void *)buffer frames:(UInt32)frames;

- (BOOL)open:(id<CogSource>)source;
- (long)seek:(long)frame;
- (void)close;

@optional
- (void)dealloc;

- (BOOL)setTrack:(NSURL *)track;

//These are in NSObject, so as long as you are a subclass of that, you are ok.
- (void)addObserver:(NSObject *)observer forKeyPath:(NSString *)keyPath options:(NSKeyValueObservingOptions)options context:(void *)context;
- (void)removeObserver:(NSObject *)observer forKeyPath:(NSString *)keyPath;
@end

@protocol CogPluginController <NSObject>
+ (id<CogPluginController>)sharedPluginController;

- (NSDictionary *)sources;

- (NSDictionary *)decodersByExtension;
- (NSDictionary *)decodersByMimeType;

- (id<CogSource>) audioSourceForURL:(NSURL *)url;
- (id<CogDecoder>) audioDecoderForSource:(id<CogSource>)source skipCue:(BOOL)skip;
@end



