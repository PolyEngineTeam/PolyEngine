//
//  AppDelegate.m
//  Standalone
//
//  Created by Michal Majczak on 21.08.2017.
//  Copyright © 2017 Michal Majczak. All rights reserved.
//

#import "AppDelegate.h"

@interface AppDelegate ()

@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // Insert code here to initialize your application
    (void)aNotification; //unused
}


- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
    (void)aNotification; //unused
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender {
    (void)sender; //unused
    return YES;
}

@end
