#import <Cocoa/Cocoa.h>

@interface WindowController : NSWindowController <NSWindowDelegate>

- (void)windowDidResize:(NSNotification *)aNotification;

@end
