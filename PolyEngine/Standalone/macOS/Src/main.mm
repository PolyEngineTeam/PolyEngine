#import <Cocoa/Cocoa.h>
#import "AppDelegate.h"
#import "CppInterface.hpp"
#import "Window.h"

id window;
bool running;

static NSRect createWindow() {
    NSUInteger windowStyle = NSWindowStyleMaskMiniaturizable|NSWindowStyleMaskClosable|NSWindowStyleMaskResizable|NSWindowStyleMaskTitled;
    
    NSRect screenRect = [[NSScreen mainScreen] frame];
    NSRect viewRect = NSMakeRect(0, 0, 800, 600);
    NSRect windowRect = NSMakeRect(NSMidX(screenRect) - NSMidX(viewRect),
                                   NSMidY(screenRect) - NSMidY(viewRect),
                                   viewRect.size.width,
                                   viewRect.size.height);
    
    window = [[Window alloc] initWithContentRect:windowRect
                                         styleMask:windowStyle
                                           backing:NSBackingStoreBuffered
                                             defer:NO];
    
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
    
    [window cascadeTopLeftFromPoint:NSMakePoint(200, 200)];
    [window setTitle:@"macOS Standalone"];
    [window makeKeyAndOrderFront:nil];
    
    return windowRect;
}

void initApp() {
    [NSApplication sharedApplication];
    [NSApp setDelegate:[[AppDelegate alloc] init]];
    running = true;
    //[NSApp finishLaunching];
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
                if([ev type] == NSEventType::NSEventTypeKeyUp)
                {
                    unsigned code = [ev keyCode];
                    PolyEngineKeyUp(code);
                }
                else if([ev type] == NSEventType::NSEventTypeKeyDown)
                {
                    unsigned code = [ev keyCode];
                    PolyEngineKeyDown(code);
                }
                else if([ev type] == NSEventType::NSEventTypeMouseMoved)
                {
                    NSPoint point = [ev locationInWindow];
                    PolyEngineUpdateMousePos(point.x, point.y);
                }
                else if([ev type] == NSEventType::NSEventTypeLeftMouseUp)
                {
                    PolyEngineKeyUp(0xfe);
                }
                else if([ev type] == NSEventType::NSEventTypeLeftMouseDown)
                {
                    PolyEngineKeyDown(0xfe);
                }
                else if([ev type] == NSEventType::NSEventTypeRightMouseUp)
                {
                    PolyEngineKeyUp(0xfd);
                }
                else if([ev type] == NSEventType::NSEventTypeRightMouseDown)
                {
                    PolyEngineKeyDown(0xfd);
                }
                else if([ev type] == NSEventType::NSEventTypeOtherMouseUp)
                {
                    PolyEngineKeyUp(0xfc);
                }
                else if([ev type] == NSEventType::NSEventTypeOtherMouseDown)
                {
                    PolyEngineKeyDown(0xfc);
                }
                
                // FIXME no mouse wheel events
                
                // handle events here
                [NSApp sendEvent: ev];
            }
        } while (ev);
    }
}

int main(/*int argc, const char * argv[]*/)  {
    @autoreleasepool {
        initApp();
        NSRect windowRect = createWindow();

        PolyEngineLoad((void*)window, windowRect.size.width, windowRect.size.height);

        while (running && [window isVisible]) {
            handleEvents();
            PolyEngineUpdate();
            [window display];
        }

        PolyEngineRelease();
    }
    return (0);
}
