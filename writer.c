
#include <string.h>
#include "wavefront.h"
#include "dump.h"
#include "gl_render.h"
#include <SDL/SDL.h>

#ifdef MAC
#include <gl.h>
#else
#include <GL/gl.h>
#endif

static SCENE font;
void init_font(char *name)
{
    load_dump(&font,name);
}
void draw_char(char c)
{
    char tmp[]=" ";

    if(c==' ')
        return;
    tmp[0]=c;
    render_object(&font,find_object(&font,tmp),RENDER_CARTOON);
}
void draw_string(char *s,int center)
{
    int n;

    glPushMatrix();
    if(center)
        glTranslatef(-(strlen(s)*1.1/2.0),0,0);

    for(n=0;s[n];n++)
    {
        draw_char(s[n]);
        glTranslatef(1.1,0,0);
    }
    glPopMatrix();
}
