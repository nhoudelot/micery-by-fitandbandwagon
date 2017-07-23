
#include <string.h>
#include <stdlib.h>
#include "mouse.h"
#include "gl_render.h"
#include "wavefront.h"
#include "dump.h"

#include "steering.h"

static SCENE mouse;
static SCENE jalkapallo;
static SCENE keilapallo;
static SCENE rantapallo;
static SCENE tennari;
static SCENE kube;

int mouse_and_pallo_load(void)
{
    load_dump(&jalkapallo,"data/jalkapallo2.dump");
    load_dump(&rantapallo,"data/rantapallo.dump");
    load_dump(&keilapallo,"data/keilapallo.dump");
    load_dump(&tennari,"data/tennari.dump");
    load_dump(&kube,"data/kube.dump");

    return(load_dump(&mouse,"data/hiiri.dump"));
}

void kube_draw() {
    render_scene(&kube,RENDER_CARTOON);
}

void pallo_draw(int tyyppi) {

    switch (tyyppi) {
    case RANTAPALLO:
	render_scene(&rantapallo,RENDER_CARTOON);
	break;
    case KEILAPALLO:
	render_scene(&keilapallo,RENDER_CARTOON);
	break;
    case TENNARI:
	render_scene(&tennari,RENDER_CARTOON);
        break;
    case JALKAPALLO:
       render_scene(&jalkapallo,RENDER_CARTOON);
       break;
    }

}
void mouse_draw(float r,float g,float b,double time)
{
    OBJ *o;
    PART *p;
    MTL *m;

    o=find_object(&mouse,"body");
    p=o->parts;
    while(p!=NULL)
    {
        m=p->material;
        if(!strcmp(m->name,"ruumis"))
        {
            m->diffuse[0]=r;
            m->diffuse[1]=g;
            m->diffuse[2]=b;
        }
        p=p->next;
    }

    find_object(&mouse,"key")->rot=time;
    find_object(&mouse,"tail")->rot=-time*1.5;
    render_scene(&mouse,RENDER_CARTOON);
}
