//
//  main.m
//  Standalone
//
//  Created by Michal Majczak on 21.08.2017.
//  Copyright © 2017 Michal Majczak. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "AppDelegate.h"
#import "OpenGLView.h"

int main(/*int argc, const char * argv[]*/) {
    @autoreleasepool {
        NSApplication* application = [NSApplication sharedApplication];
        [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
        AppDelegate * delegate = [[AppDelegate alloc] init];
        [application setDelegate:delegate];
        
        // create window
        id window = [[NSWindow alloc] initWithContentRect:NSMakeRect(0, 0, 500, 500) styleMask:NSWindowStyleMaskTitled backing:NSBackingStoreBuffered defer:NO];
        
        // set window style
        [window setStyleMask:NSWindowStyleMaskMiniaturizable|NSWindowStyleMaskClosable|NSWindowStyleMaskResizable|NSWindowStyleMaskTitled];
        
        
        NSOpenGLPixelFormatAttribute attrs[] =
        {
            NSOpenGLPFADoubleBuffer,
            NSOpenGLPFADepthSize, 24,
            // Must specify the 3.2 Core Profile to use OpenGL 3.2
            NSOpenGLPFAOpenGLProfile,
            NSOpenGLProfileVersionLegacy,
            0
        };
        
        NSOpenGLPixelFormat *pf = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];
        
        if (!pf)
        {
            NSLog(@"No OpenGL pixel format");
        }

        
        [window setContentView:[[OpenGLView alloc] initWithFrame:NSMakeRect(0, 0, 500, 500) pixelFormat:pf]];
        
        [window cascadeTopLeftFromPoint:NSMakePoint(200, 200)];
        [window setTitle:@"macOS Standalone"];
        [window makeKeyAndOrderFront:nil];
        
        [application activateIgnoringOtherApps:YES];
        [application run];
    }
    
    return 0;
}
