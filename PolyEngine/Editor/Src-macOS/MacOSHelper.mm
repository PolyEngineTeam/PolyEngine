#import <Cocoa/Cocoa.h>
#import "MacOSHelper.hpp"

void* GetNSWindowFromNSView(void* nsview)
{
    return (void*)[(NSView*) nsview window];
}