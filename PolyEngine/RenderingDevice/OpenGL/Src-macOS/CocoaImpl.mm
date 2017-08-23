#import "CocoaImpl.h"

@implementation CocoaImpl

void CreateDeviceImpl(void* window_ptr, unsigned width, unsigned height)
{
    id window = (id)window_ptr;

    NSOpenGLPixelFormatAttribute attrs[] =
    {
        NSOpenGLPFADoubleBuffer,
        NSOpenGLPFADepthSize, 24,
        // Must specify the 3.2 Core Profile to use OpenGL 3.2
        NSOpenGLPFAOpenGLProfile,
        NSOpenGLProfileVersion3_2Core,
        0
    };

    NSOpenGLPixelFormat *pf = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];

    if (!pf)
    {
        NSLog(@"No OpenGL pixel format");
    }

    [window setContentView:[[NSOpenGLView alloc] initWithFrame:NSMakeRect(0, 0, width, height) pixelFormat:pf]];
}

void DestroyDeviceImpl(void* window)
{
    (void)window;
}

void EndFrameImpl(void* window_ptr)
{
    id window = (id)window_ptr;
    [window setNeedsDisplay:YES];
}

@end