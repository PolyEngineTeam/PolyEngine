//
//  OpenGLView.m
//  Standalone
//
//  Created by Michal Majczak on 21.08.2017.
//  Copyright © 2017 Michal Majczak. All rights reserved.
//

#import "OpenGLView.h"
#import <OpenGL/gl.h>

@implementation OpenGLView

-(void) drawRect: (NSRect) bounds
{
    glClearColor(1, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    (void)bounds; //unused
    /*glColor3f(1.0f, 0.85f, 0.35f);
    glBegin(GL_TRIANGLES);
    {
        glVertex3f(0.f, 0.6f, 0.f);
        glVertex3f(-0.2f, -0.3f, 0.f);
        glVertex3f(0.2f, -0.3f, 0.f);

    }
    glEnd();
    glFlush();*/
    
    [self setNeedsDisplay:YES];
}

@end
