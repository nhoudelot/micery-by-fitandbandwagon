#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

#include <SDL.h>

#ifdef MAC
#include <gl.h>
#include <glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include "steering.h"
#include "mouse.h"
#include "steer_render.h"

void draw_hiiri(HIIRI * h,float dt) {
#define NV 5
    float r,g,b;
    float l;
    int i;
    l=0.0;
    for (i=0; i<3; i++)
	l+=h->desire[i]*h->desire[i];
    l=sqrt(l);

    h->vakkyra_kulma+=75.0*2.5*dt*h->spd*h->spd_factor*2.0*M_PI*l;
    switch (h->color) {
    case COLOR_BLUE:
        // seuraava
	//	r=0.96; g=0.6; b=0.43;
        r=FOLLOW_R;
	g=FOLLOW_G;
        b=FOLLOW_B;
	break;

    case COLOR_GREEN:
        // harhaileva
	//r=0.95; g=0.58; b=0.30;
        r=WANDER_R;
        g=WANDER_G;
	b=WANDER_B;
	break;
    case COLOR_YELLOW:
        // goal
	//r=0.95; g=0.80; b=0.30;
	r=GOAL_R;
	g=GOAL_G;
	b=GOAL_B;

	break;

    default:
        r=1.0; g=1.0; b=1.0;
	break;
    }
    glEnable(GL_DEPTH_TEST);
    glLineWidth(1.0);
    glPushMatrix();
    glTranslatef(h->pos[0],h->pos[1],h->pos[2]);

    glPushMatrix();
    glScalef(0.03,0.03,0.03);
    glRotatef(h->dir,0.0,0.0,1.0);
    glRotatef(90.0,0.0,0.0,1.0);
    glRotatef(90.0,1.0,0.0,0.0);
    mouse_draw(r,g,b,h->vakkyra_kulma);
    glPopMatrix();

    // varjo
    glColor3f(0.07,0.07,0.07);
    glPushMatrix();
    glRotatef(h->dir+90.0,0.0,0.0,1.0);
    glScalef(0.03,0.04,1.0);
//    glRotatef(90.0,0.0,0.0,1.0);
#define R1 12
    glBegin(GL_TRIANGLES);
    for (i=0; i<R1; i++) {
	glVertex3f(0.0,0.0,0.0);
	glVertex3f(cos(i*2*M_PI/R1),
		   sin(i*2*M_PI/R1),
                   0.0);
	glVertex3f(cos((i+1)*2*M_PI/R1),
		   sin((i+1)*2*M_PI/R1),
                   0.0);
    }
    glEnd();
    glPopMatrix();

    glPopMatrix();

}

void draw_seina(SEINA * s) {
    glColor3f(0.5,0.5,0.5);
    glBegin(GL_POLYGON);
    glVertex3f(s->pos[0],s->pos[1],s->pos[2]);
    glVertex3f(s->pos[0]+s->size[0],s->pos[1],s->pos[2]);
    glVertex3f(s->pos[0]+s->size[0],s->pos[1]+s->size[1],s->pos[2]);
    glVertex3f(s->pos[0],s->pos[1]+s->size[1],s->pos[2]);
    glEnd();

}


void draw_the_pallo(PALLO * p) {
    int i;

    glEnable(GL_DEPTH_TEST);
    glPushMatrix();
    glTranslatef(p->pos[0],p->pos[1],p->pos[2]+p->r*0.7);
    glRotatef(p->dir,0.0,0.0,1.0);
    glRotatef(p->vierin,0.0,1.0,0.0);
    glScalef(0.7,0.7,0.7);
    glScalef(p->r,p->r,p->r);
    pallo_draw(p->tyyppi);
    glPopMatrix();
    // varjo
    glEnable(GL_DEPTH_TEST);
    glColor3f(0.0,0.0,0.0);
    glPushMatrix();
    glTranslatef(p->pos[0],p->pos[1],0.0);
    glScalef(0.7,0.7,0.7);

    for (i=0; i<16; i++) {
	glBegin(GL_POLYGON);
	glVertex3f(0.0,0.0,0.0);
	glVertex3f(cos(i*2.0*M_PI/16)*p->r,
		    sin(i*2.0*M_PI/16)*p->r,
                    0.0
		   );
	glVertex3f(cos((i+1)*2.0*M_PI/16)*p->r,
		    sin((i+1)*2.0*M_PI/16)*p->r,
                    0.0
		   );
	glEnd();
    }
    glPopMatrix();


}


void draw_borders(MAAILMA * m) {
#define P 0.02
#define H 0.02

    glPushMatrix();
    glTranslatef(-m->clip[0]*0.5-DEFAULT_RADIUS*0.5,m->clip[1],0.0);
    glScalef(m->clip[0]*0.5-DEFAULT_RADIUS*0.5,P,H);
    glTranslatef(0.0,1.0,1.0);
    glRotatef(270.0,1.0,0.0,0.0);
    glRotatef(180.0,0.0,0.0,1.0);
    glEnable(GL_DEPTH_TEST);
    kube_draw();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(+m->clip[0]*0.5+DEFAULT_RADIUS*0.5,m->clip[1],0.0);
    glScalef(m->clip[0]*0.5-DEFAULT_RADIUS*0.5,P,H);
    glTranslatef(0.0,1.0,1.0);
    glRotatef(270.0,1.0,0.0,0.0);
    glRotatef(180.0,0.0,0.0,1.0);
    glEnable(GL_DEPTH_TEST);
    kube_draw();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-m->clip[0]*0.5-DEFAULT_RADIUS*0.5,-m->clip[1],0.0);
    glScalef(m->clip[0]*0.5-DEFAULT_RADIUS*0.5,P,H);
    glTranslatef(0.0,-1.0,1.0);
    glRotatef(270.0,1.0,0.0,0.0);
    glRotatef(180.0,0.0,0.0,1.0);
    glEnable(GL_DEPTH_TEST);
    kube_draw();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(+m->clip[0]*0.5+DEFAULT_RADIUS*0.5,-m->clip[1],0.0);
    glScalef(m->clip[0]*0.5-DEFAULT_RADIUS*0.5,P,H);
    glTranslatef(0.0,-1.0,1.0);
    glRotatef(270.0,1.0,0.0,0.0);
    glRotatef(180.0,0.0,0.0,1.0);
    glEnable(GL_DEPTH_TEST);
    kube_draw();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(m->clip[0],-m->clip[1]*0.5-DEFAULT_RADIUS*0.5-P*1,0.00);
    glScalef(P,m->clip[1]*0.5+P*1-DEFAULT_RADIUS*0.5,H);
    glTranslatef(1.0,0.0,1.0);
    glEnable(GL_DEPTH_TEST);
    glRotatef(270.0,1.0,0.0,0.0);
    glRotatef(180.0,0.0,1.0,0.0);
    kube_draw();
    glPopMatrix();
    glPushMatrix();
    glTranslatef(m->clip[0],+m->clip[1]*0.5+DEFAULT_RADIUS*0.5+P*1,0.00);
    glScalef(P,m->clip[1]*0.5+P*1-DEFAULT_RADIUS*0.5,H);
    glTranslatef(1.0,0.0,1.0);
    glEnable(GL_DEPTH_TEST);
    glRotatef(270.0,1.0,0.0,0.0);
    glRotatef(180.0,0.0,1.0,0.0);
    kube_draw();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-m->clip[0],-m->clip[1]*0.5-DEFAULT_RADIUS*0.5-P*1,0.00);
    glScalef(P,m->clip[1]*0.5+P*1-DEFAULT_RADIUS*0.5,H);
    glTranslatef(-1.0,0.0,1.0);
    glEnable(GL_DEPTH_TEST);
    glRotatef(270.0,1.0,0.0,0.0);
    glRotatef(180.0,0.0,1.0,0.0);
    kube_draw();
    glPopMatrix();
    glPushMatrix();
    glTranslatef(-m->clip[0],+m->clip[1]*0.5+DEFAULT_RADIUS*0.5+P*1,0.00);
    glScalef(P,m->clip[1]*0.5+P*1-DEFAULT_RADIUS*0.5,H);
    glTranslatef(-1.0,0.0,1.0);
    glEnable(GL_DEPTH_TEST);
    glRotatef(270.0,1.0,0.0,0.0);
    glRotatef(180.0,0.0,1.0,0.0);
    kube_draw();
    glPopMatrix();


/*
#define VV 0.07
    glColor3f(0.0,0.0,0.0);
    glBegin(GL_POLYGON);
    glVertex3f(-m->clip[0]+VV,-m->clip[1],0.0);
    glVertex3f(-m->clip[0]+VV,+m->clip[1],0.0);
    glVertex3f(-m->clip[0],+m->clip[1],0.0);
    glVertex3f(-m->clip[0],-m->clip[1],0.0);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex3f(-m->clip[0]+VV,+m->clip[1]-VV,0.0);
    glVertex3f(+m->clip[0],+m->clip[1]-VV,0.0);
    glVertex3f(+m->clip[0],+m->clip[1],0.0);
    glVertex3f(-m->clip[0]+VV,+m->clip[1],0.0);
    glEnd();
    */
}
void draw_maailma(MAAILMA * m,float dt) {
#define NMR 8
#define WZ 0.05
    HIIRI * h;
    SEINA * s;
    PALLO * p;

    /* piirrä maaliympyrä */
    //draw_rantapallo(m->goal[0],m->goal[1],m->gz,0.1);

    /* piirrä rajat */
    if (m->is_borders)
	draw_borders(m);

    /* piirrä pallot */
    p=m->pallot;
    while (p!=NULL) {
	draw_the_pallo(p);
        p=p->next;
    }

    /* piirrä seinät */
    s=m->seinat;
    while (s!=NULL) {
	draw_seina(s);
        s=s->next;
    }

    /* piirrä hiiret */
    h=m->hiiret;

    while (h!=NULL) {
	draw_hiiri(h,dt);
        h=h->next;
    }
}
