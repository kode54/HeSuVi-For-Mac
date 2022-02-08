#import "PluginController.h"
#import "Plugin.h"

#import "Logging.h"

#import "NSFileHandle+CreateFile.h"

@implementation PluginController

@synthesize sources;

@synthesize decodersByExtension;
@synthesize decodersByMimeType;

@synthesize configured;

static PluginController *sharedPluginController = nil;

+ (id<CogPluginController>)sharedPluginController {
	@synchronized(self) {
		if(sharedPluginController == nil) {
			sharedPluginController = [[self alloc] init];
		}
	}

	return sharedPluginController;
}

- (id)init {
	self = [super init];
	if(self) {
		self.sources = [[NSMutableDictionary alloc] init];

		self.decodersByExtension = [[NSMutableDictionary alloc] init];
		self.decodersByMimeType = [[NSMutableDictionary alloc] init];

		[self setup];
	}

	return self;
}

- (void)setup {
	if(self.configured == NO) {
		self.configured = YES;

		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(bundleDidLoad:) name:NSBundleDidLoadNotification object:nil];

		[self loadPlugins];
		[self printPluginInfo];
	}
}

- (void)bundleDidLoad:(NSNotification *)notification {
	NSArray *classNames = [[notification userInfo] objectForKey:@"NSLoadedClasses"];
	for(NSString *className in classNames) {
		Class bundleClass = NSClassFromString(className);
		if([bundleClass conformsToProtocol:@protocol(CogDecoder)]) {
			[self setupDecoder:className];
		}
		if([bundleClass conformsToProtocol:@protocol(CogSource)]) {
			[self setupSource:className];
		}
	}
}

- (void)loadPluginsAtPath:(NSString *)path {
	NSArray *dirContents = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:path error:nil];

	for(NSString *pname in dirContents) {
		NSString *ppath;
		ppath = [NSString pathWithComponents:@[path, pname]];

		if([[pname pathExtension] isEqualToString:@"bundle"]) {
			NSBundle *b = [NSBundle bundleWithPath:ppath];
			[b load];
		}
	}
}

- (void)loadPlugins {
	NSString *path = [[NSBundle mainBundle] builtInPlugInsPath];
	[self loadPluginsAtPath:path];
}

- (void)setupDecoder:(NSString *)className {
	Class decoder = NSClassFromString(className);
	if(decoder && [decoder respondsToSelector:@selector(fileTypes)]) {
		for(id fileType in [decoder fileTypes]) {
			NSString *ext = [fileType lowercaseString];
			NSMutableArray *decoders;
			if(![decodersByExtension objectForKey:ext]) {
				decoders = [[NSMutableArray alloc] init];
				[decodersByExtension setObject:decoders forKey:ext];
			} else
				decoders = [decodersByExtension objectForKey:ext];
			[decoders addObject:className];
		}
	}

	if(decoder && [decoder respondsToSelector:@selector(mimeTypes)]) {
		for(id mimeType in [decoder mimeTypes]) {
			NSString *mimetype = [mimeType lowercaseString];
			NSMutableArray *decoders;
			if(![decodersByMimeType objectForKey:mimetype]) {
				decoders = [[NSMutableArray alloc] init];
				[decodersByMimeType setObject:decoders forKey:mimetype];
			} else
				decoders = [decodersByMimeType objectForKey:mimetype];
			[decoders addObject:className];
		}
	}
}

- (void)setupSource:(NSString *)className {
	Class source = NSClassFromString(className);
	if(source && [source respondsToSelector:@selector(schemes)]) {
		for(id scheme in [source schemes]) {
			[sources setObject:className forKey:scheme];
		}
	}
}

- (void)printPluginInfo {
	ALog(@"Sources: %@", self.sources);

	ALog(@"Decoders by Extension: %@", self.decodersByExtension);
	ALog(@"Decoders by Mime Type: %@", self.decodersByMimeType);
}

- (id<CogSource>)audioSourceForURL:(NSURL *)url {
	NSString *scheme = [url scheme];

	Class source = NSClassFromString([sources objectForKey:scheme]);

	return [[source alloc] init];
}

// Note: Source is assumed to already be opened.
- (id<CogDecoder>)audioDecoderForSource:(id<CogSource>)source skipCue:(BOOL)skip {
	NSString *ext = [[source url] pathExtension];
	NSArray *decoders = [decodersByExtension objectForKey:[ext lowercaseString]];
	NSString *classString;
	if(decoders) {
		classString = [decoders objectAtIndex:0];
	} else {
		decoders = [decodersByMimeType objectForKey:[[source mimeType] lowercaseString]];
		if(decoders) {
			classString = [decoders objectAtIndex:0];
		} else {
			classString = @"SilenceDecoder";
		}
	}

	Class decoder = NSClassFromString(classString);

	return [[decoder alloc] init];
}

@end
