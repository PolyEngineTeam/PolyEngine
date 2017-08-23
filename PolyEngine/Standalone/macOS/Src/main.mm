#import <Cocoa/Cocoa.h>
#import "AppDelegate.h"
#import "CppInterface.hpp"

id window;
bool running;

static void createWindow() {
    NSUInteger windowStyle = NSWindowStyleMaskMiniaturizable|NSWindowStyleMaskClosable|NSWindowStyleMaskResizable|NSWindowStyleMaskTitled;
    
    NSRect screenRect = [[NSScreen mainScreen] frame];
    NSRect viewRect = NSMakeRect(0, 0, 800, 600);
    NSRect windowRect = NSMakeRect(NSMidX(screenRect) - NSMidX(viewRect),
                                   NSMidY(screenRect) - NSMidY(viewRect),
                                   viewRect.size.width,
                                   viewRect.size.height);
    
    window = [[NSWindow alloc] initWithContentRect:windowRect
                                         styleMask:windowStyle
                                           backing:NSBackingStoreBuffered
                                             defer:NO];
    
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
    
    [window cascadeTopLeftFromPoint:NSMakePoint(200, 200)];
    [window setTitle:@"macOS Standalone"];
    [window makeKeyAndOrderFront:nil];
    
    PolyEngineLoad((void*)window, windowRect.size.width, windowRect.size.height);
}

void initApp() {
    [NSApplication sharedApplication];
    [NSApp setDelegate:[[AppDelegate alloc] init]];
    running = true;
    [NSApp finishLaunching];
}

void handleEvents() {
    @autoreleasepool {
        NSEvent* ev;
        do {
            ev = [NSApp nextEventMatchingMask: NSEventMaskAny
                                    untilDate: nil
                                       inMode: NSDefaultRunLoopMode
                                      dequeue: YES];
            if (ev) {
                // handle events here
                [NSApp sendEvent: ev];
            }
        } while (ev);
    }
}

int main(/*int argc, const char * argv[]*/)  {
    @autoreleasepool {
        initApp();
        createWindow();
        
        while (running) {
            handleEvents();
            PolyEngineUpdate();
            [window display];
        }
    }
    return (0);
}
