
#ifndef GL_RENDER_H
#define GL_RENDER_H

#define RENDER_NORMAL 0
#define RENDER_CARTOON 1
#define RENDER_WIRE 2

#include "wavefront.h"

void render_scene(SCENE *s,int type);
void render_object(SCENE *s,OBJ *o,int type);

#endif
