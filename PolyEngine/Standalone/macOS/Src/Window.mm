#import "Window.h"
#import "CppInterface.hpp"

@implementation WindowController

- (void)windowDidResize:(NSNotification *)aNotification {
    (void)aNotification; //unused
    NSRect screenRect = [[[self window] contentView] bounds];
    PolyEngineResize(screenRect.size.width, screenRect.size.height);
}

@end
