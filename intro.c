/*
    The intro part with demo name & credits
*/

#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "writer.h"
#include "gl_render.h"
#include "wavefront.h"
#include "dump.h"
#include "mouse.h"

#ifdef MAC
#include <gl.h>
#else
#include <GL/gl.h>
#endif

static SCENE s;

void intro_init(int seed)
{
    if(((seed>>3)&0x3)==0)
        load_dump(&s,"data/jalkapallo2.dump");
    else
        load_dump(&s,"data/rantapallo.dump");
    init_font("data/fontti.dump");
}

void intro(double tid,int seed)
{
    int n;
    double r,h;

    /* Set projections etc. up */
    glClearColor(1,1,1,0);
    glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-1,1, -0.75,0.75, 1,10000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    r=tid;

    glTranslatef(0,-0.5,-5);

    /* Draw the balls in front */
    for(n=0;n<3;n++)
    {
        glPushMatrix();
        h=fmod(r/500+n*11,6)-3;
        h=9-h*h-4;

        if(seed&4)
        {
            glTranslatef(-fmod(n*3+r/200.0,10)+5,h,1.5);
            glRotatef(r/10*(n+1),0,0,1);
        }
        else
        {
            glTranslatef(fmod(n*3+r/200.0,10)-5,h,1.5);
            glRotatef(-r/10*(n+1),0,0,1);
        }

        if((seed&3)==0)
            render_scene(&s,RENDER_WIRE);
        else
            if((seed&3)==1)
                render_scene(&s,RENDER_NORMAL);
            else
                render_scene(&s,RENDER_CARTOON);
        glPopMatrix();
    }

    /* Draw the writer texts */
    glPushMatrix();
    glTranslatef(11-(fmod(r,6000))/300.0+3.0*cos(fmod(r,6000)*M_PI/1200.0),0,0);
    glRotatef(sin(r/200)*10.0+10.0,1,0,0);
    if(r>=6000)
    {
        if(r<12000)
            draw_string("micery",1);
        else
            if(r<18000)
                draw_string("by",1);
            else
                if(r<24000)
                    draw_string("fit",1);
                else
                    if(r<30000)
                        draw_string("bandwagon",1);
    }
    glPopMatrix();

    /* Some mice in the background */
    for(n=0;n<7;n++)
    {
        glPushMatrix();
        if(((seed>>6)&3)==0)
        {
            glTranslatef(-fmod(n*30.0+r/300.0,42)+21,-1.0,-n-3);
            glRotatef(270,0,1,0);
        }
        else
        {
            glTranslatef(fmod(n*30.0+r/300.0,42)-21,-1.0,-n-3);
            glRotatef(90,0,1,0);
        }
    
        if((seed>>5)&1)
            mouse_draw(0.5+n/30.0,0.7,1.0,r/2.0);
        else
            mouse_draw(1.0,0.7,0.5+n/30.0,r/2.0);
        glPopMatrix();
    }
}
