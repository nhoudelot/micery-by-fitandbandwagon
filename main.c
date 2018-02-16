/*
    The main source of Micery.
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <SDL.h>

#ifdef MAC
#include <gl.h>
#include <glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

// screen
#define WIDTH 800
#define HEIGHT 600

#include "writer.h"
#include "steering.h"
#include "steer_render.h"
#include "mouse.h"
#include "musa.h"
#include "intro.h"

#include "paa.h"

#include "misc.h"

MAAILMA maa1;


int seed;
int init_steer() {
    srand(time(NULL));

    init_maailma(&maa1,1.00,1.00,1.0);
    char datapath[]="/usr/share/micery-by-fitandbandwagon/";
    if (chdir(datapath) == 0) {
    }else{
         printf("Error : Could not change directory to %s\n",datapath);
    }
    if (mouse_and_pallo_load()!=0) {
	fprintf(stderr,"Mouse load error!!!\n");
        return -1;
    }
    if (init_musa()!=0) {
        return -1;
    }

    start_musa();

    seed=rand();
    intro_init(seed);


    return 0;
}


void checker_board() {
#define C 0.2
#define I 6
    int x,y;
    static int ekakerta=1;
    static int lista;


    if (ekakerta) {
        lista=glGenLists(1);
	glNewList(lista,GL_COMPILE);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glColor3f(1.0,1.0,1.0);
	for (y=-I; y<I; y++)
	    for (x=-I; x<I; x++) {
		if ((x+y) % 2==0) {
		    glBegin(GL_POLYGON);
		    glVertex3f(x*C,y*C,-0.005);
		    glVertex3f((x+1)*C,y*C,-0.005);
		    glVertex3f((x+1)*C,(y+1)*C,-0.005);
		    glVertex3f(x*C,(y+1)*C,-0.005);
		    glEnd();
		}
	    }
	glEndList();
	ekakerta=0;
    } else {
	glCallList(lista);
    }
}


int camera_mode=1;
int vaihto=1;
float vaihtocounter=10.0;

float * follow_pos;
float * follow_dir;

float new_rot[3]={0.0,0.0,0.0};
float cf=0.995;

float tyyppiz=0.0;

float vaihtoaika=10.0;

void start_vaihto(float va) {
    vaihto=1;
    vaihtoaika=va;
    vaihtocounter=vaihtoaika;
}
void do_camera(float dt) {
#define CFR 0.999

    int i;
    static float pos[3]={0.0,0.1,-3.4};
    static float new_pos[3]={0.0,0.1,-0.4};
    static float rot[3]={0.0,0.0,0.0};

    if (vaihto) {
	cf=0.995-(0.995-0.5)*(vaihtoaika-vaihtocounter)/vaihtoaika;
	vaihtocounter-=dt;
	if (vaihtocounter<=0) {
	    vaihtocounter=0.0;
            vaihto=0;
	}
    } else
        cf=0.5;
    switch (camera_mode) {
    case 0:
        new_pos[0]=0.0;
        new_pos[1]=0.0;
        new_pos[2]=0.2;
	break;
    case 1:
        new_pos[0]=0.0;
        new_pos[1]=0.1;
        new_pos[2]=-0.4;
	break;
    case 2:
    case 3:
    case 4:
	new_pos[0]=-follow_pos[0];
	new_pos[1]=-follow_pos[1];
        new_pos[2]=tyyppiz;
	new_rot[2]=0.0;
        break;
    case 5:
    case 6:
	new_pos[0]=-follow_pos[0];
	new_pos[1]=-follow_pos[1];
        new_rot[2]=-*follow_dir;
        new_pos[2]=0.0;
        new_rot[0]=-55.0;
	break;
    }
    glRotatef(rot[0],1.0,0.0,0.0);
    glRotatef(rot[2],0.0,0.0,1.0);
    glTranslatef(pos[0],pos[1],pos[2]);

   for (i=0; i<3; i++)
	rot[i]=CFR*rot[i]+(1.0-CFR)*new_rot[i];
    for (i=0; i<3; i++)
	pos[i]=cf*pos[i]+(1.0-cf)*new_pos[i];

}

#define FIRST_RAUH_KAMERA 0
#define N_RAUH_KAMERAT 7
#define FIRST_ACTION_KAMERA 5
#define N_ACTION_KAMERAT 2

#define MAX_RAUH_HIIRET 24
#define MAX_ACTION_HIIRET 8
int skene_hiiret=0;
int skene_pallot=0;

void uusia_hiiria(int kulma, int n) {
    int i;
    for (i=0; i<n; i++) {
	switch (kulma) {
	case 0:
	    add_harhaileva_hiiri(&maa1,+maa1.clip[0]+(0.1+DEFAULT_RADIUS)*i,0.0,0.0,
				 180.0);
	    break;
	case 1:
	    add_harhaileva_hiiri(&maa1,0.0,-maa1.clip[1]-(0.1+DEFAULT_RADIUS)*i,0.0,
				 90.0);
            break;
	case 2:
	    add_harhaileva_hiiri(&maa1,-maa1.clip[0]-(0.1+DEFAULT_RADIUS)*i,0.0,0.0,
				 0.0);
            break;
	case 3:
	    add_harhaileva_hiiri(&maa1,maa1.clip[0]+(0.1+DEFAULT_RADIUS)*i,0.0,0.0,
				 180.0);
            break;

	}
    }
}

void uusia_pallomaali_hiiria(int kulma, int n) {
    int i;
    for (i=0; i<n; i++) {
	switch (kulma) {
	case 0:
	    add_goal_hiiri(&maa1,+maa1.clip[0]+(0.1+DEFAULT_RADIUS)*i,0.0,0.0,
				 180.0);
	    break;
	case 1:
	    add_goal_hiiri(&maa1,0.0,-maa1.clip[1]-(0.1+DEFAULT_RADIUS)*i,0.0,
				 90.0);
            break;
	case 2:
	    add_goal_hiiri(&maa1,-maa1.clip[0]-(0.1+DEFAULT_RADIUS)*i,0.0,0.0,
				 0.0);
            break;
	case 3:
	    add_goal_hiiri(&maa1,maa1.clip[0]+(0.1+DEFAULT_RADIUS)*i,0.0,0.0,
				 180.0);
            break;

	}
    }
}
enum {
    PITAVA=0,
    LIUKASTUS=1,
    JONOSSA=2
};

void actioni(int trig,int ekakerta) {
    int kul,i=0;

    static float ts=0.0;
    static int ac_moodi=PITAVA;
    static HIIRI * johtaja;

    switch (trig) {
    case TRIG4:
	if (ekakerta) {
            kill_all_pallot(&maa1);
	    add_pallo(&maa1,rnd()*4,rnd()*4);
	    start_vaihto(1.0);
            camera_mode=2;
	    follow_pos=&maa1.pallot->pos[0];
	    follow_dir=&ts;
	    kill_all_hiiret(&maa1);
	    skene_hiiret=0;
	    ac_moodi=rnd()*3;

	    if ((ac_moodi==PITAVA) || (ac_moodi==JONOSSA)) {
		maa1.new_checker[0]=0.3;
		maa1.new_checker[1]=0.2;
		maa1.new_checker[2]=0.1;
		new_global_desire_f=0.5;
	    }
	    else {
		maa1.new_checker[0]=0.1;
		maa1.new_checker[1]=0.2;
		maa1.new_checker[2]=0.3;
		new_global_desire_f=0.99;
	    }

	    if (ac_moodi==JONOSSA)
		johtaja=add_goal_hiiri(&maa1,+maa1.clip[0]+(0.1+DEFAULT_RADIUS)*i,0.0,0.0,
				       180.0);
            break;
	}


	if (skene_hiiret<MAX_ACTION_HIIRET) {
	    kul=rnd()*4;

            if (ac_moodi!=JONOSSA)
		uusia_pallomaali_hiiria(kul,1);
	    else {
		johtaja=add_following_hiiri(&maa1,0.0,-maa1.clip[1]-(0.1+DEFAULT_RADIUS)*i,0.0,
				 90.0,johtaja);
	    }
	    skene_hiiret+=1;
	}



        break;
    case TRIG64:
	if (ekakerta)
            break;

	uusia_hiiria(0,1);
	uusia_hiiria(1,1);
	uusia_hiiria(2,1);
	uusia_hiiria(3,1);

	camera_mode=rnd()*N_ACTION_KAMERAT+FIRST_ACTION_KAMERA;
	start_vaihto(1.0);

        tyyppiz=rnd()*0.4;
	if ((maa1.hiiret==NULL) || (maa1.pallot==NULL))
	    camera_mode=0;
	else {
	    if (camera_mode==5) {
		follow_pos=&(last_hiiri(&maa1))->pos[0];
		follow_dir=&(last_hiiri(&maa1))->dir;
	    } else {
		follow_pos=&maa1.pallot->pos[0];
		follow_dir=&ts;
	    }
	}

	break;
    case TRIG128:
	if (ekakerta)
	    break;

	add_pallo(&maa1,rnd()*4,rnd()*4);
	if (ac_moodi!=JONOSSA) {
	    for (i=0; i<2; i++)
		kill_oldest_hiiri_alive(&maa1);
	    skene_hiiret-=2;
	}

	break;

    case TRIG32:
	if (ekakerta)
	    break;

        break;
    }
}

enum {
    HIIRET=0,
    PALLOT=1};

int in_intro=1;
void intro_osa(int stillon) {
    in_intro=stillon;
}

void rauhallinen(int trig,int eka_rauhallinen) {
    int n,kul,i;
    int uus_sh;
    static int rauh_moodi=HIIRET;

    switch (trig) {
    case TRIG4:
	if (eka_rauhallinen) {
	    new_global_desire_f=0.5;
	    maa1.new_checker[0]=0.3;
	    maa1.new_checker[1]=0.2;
	    maa1.new_checker[2]=0.1;

	    kill_all_hiiret(&maa1);
            skene_pallot=0;
	    kill_all_pallot(&maa1);
	    skene_hiiret=0;
	    start_vaihto(100.0);
	    camera_mode=1;

	    rauh_moodi=rnd()*2;
            break;
	}

        break;
    case TRIG16:
	if (rauh_moodi==PALLOT) {
	    if (skene_pallot<10) {
		uusia_hiiria(rnd()*4,1);
		skene_hiiret+=1;
		add_pallo(&maa1,rnd()*4,0);
		add_pallo(&maa1,rnd()*4,1);
		add_pallo(&maa1,rnd()*4,2);
		add_pallo(&maa1,rnd()*4,3);
		skene_pallot+=4;
	    }
	}
	break;

    case TRIG32:
	if (rauh_moodi==PALLOT) {

	} else {
	    n=rnd()*4+1;
	    kul=rnd()*4;
	    uusia_hiiria(kul,n);
	    skene_hiiret+=n;

	    if (skene_hiiret>MAX_RAUH_HIIRET) {
		uus_sh=skene_hiiret*(0.4+rnd()*0.5);

		for (i=0; i<(skene_hiiret-uus_sh); i++)
		    kill_oldest_hiiri_alive(&maa1);

		skene_hiiret=uus_sh;
	    }
	}

	break;
    case TRIG128:
	if (eka_rauhallinen)
	    break;

	camera_mode=rnd()*N_RAUH_KAMERAT+FIRST_RAUH_KAMERA;
	start_vaihto(10.0);

	if (rauh_moodi==HIIRET) {
	    tyyppiz=rnd()*0.4;
	    if (maa1.hiiret==NULL)
		camera_mode=0;
	    else {
		follow_pos=&maa1.hiiret->pos[0];
		follow_dir=&maa1.hiiret->dir;
	    }
	} else {
	    tyyppiz=rnd()*0.4;
	    if (maa1.pallot==NULL)
		camera_mode=0;
	    else {
		follow_pos=&maa1.pallot->pos[0];
		follow_dir=&maa1.pallot->dir;
	    }
	}

        break;
    case TRIG256:
	break;

    case TRIG64:
	new_rot[0]=-(rnd())*16;

	break;
    }

}


void render_steer(int tick, int cameramode) {

    static int old_t=0;
    float dt;

    dt=(float)(tick-old_t)/1000.0;
    old_t=tick;

    if (in_intro) {
	intro(tick,seed);
	go_musa();
	return;
    }

    glClearColor(maa1.checker[0],maa1.checker[1],maa1.checker[2],0.0);

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    checker_board();

    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    gluPerspective(90.0, (float)WIDTH/(float)HEIGHT, 0.01, 1135.0);

    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();
    glTranslatef(0.0,0.0,-0.8);
    do_camera(dt);

    draw_maailma(&maa1,dt);

    update_maailma(&maa1,dt);
    go_musa();
}



int main(int argc, char * argv[]) {
#define RATE 75.0
    int quit=0;
    int shiftdown=0;
    int frame=0;
    int keydown;
    int start_tick;
    int cmode=0;
    int tick;
    int flags=SDL_OPENGL+SDL_FULLSCREEN,width=WIDTH,height=HEIGHT;
    SDL_Event e;
    int old_frame=-1,new_frame;

    /* Initialize SDL */
    if ( SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER|SDL_INIT_AUDIO) < 0 ) {
	fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
	return -1;
    }

    if((argc>1 && !strcmp(argv[1],"-w"))
#ifdef MAC
       || argv[0][strlen(argv[0])-1]=='w' /* micery_window */
#endif
      )
        flags-=SDL_FULLSCREEN;

#ifndef MAC /* This hasn't historically worked on Mac SDL "too well" */
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);
#endif

    if(argc==3)
    {
        width=atoi(argv[1]);
        height=atoi(argv[2]);
    }
    if(argc==4)
    {
        width=atoi(argv[2]);
        height=atoi(argv[3]);
    }

    if (SDL_SetVideoMode(width,height,32,flags)<0) {
	fprintf( stderr, "SDL Video mode set failed: %s\n",
	         SDL_GetError());
	return 0;
    }

    SDL_WM_SetCaption("Micery",0);
    SDL_ShowCursor(0);

    init_steer();

    start_tick=SDL_GetTicks();
    while (quit!=1) {

	if (quit==2) {
	    kill_all_hiiret(&maa1);
	    kill_all_pallot(&maa1);
	}
	if ((quit==2) && (maa1.hiiret==NULL))
            quit=1;

	tick=SDL_GetTicks();

	new_frame=(int)((tick/1000.0)*RATE);
	if (new_frame!=old_frame) {
	    render_steer(tick-start_tick,cmode);
            old_frame=new_frame;
	    SDL_GL_SwapBuffers();
	}
	frame++;

	while(SDL_PollEvent(&e)>0) {

	    if (e.type==SDL_QUIT)
		quit=1;
	    if (e.type==SDL_KEYUP) {
		keydown=0;
		if (e.key.keysym.sym==SDLK_LSHIFT)
                    shiftdown=0;
	    }

            /* The quit events. Mac demos should exit with mousebutton. */
#ifdef MAC
            if(e.type==SDL_MOUSEBUTTONDOWN)
                quit=1;
#endif
	    if (e.type==SDL_KEYDOWN) {
		keydown=e.key.keysym.sym;
		if (keydown==SDLK_LSHIFT)
		    shiftdown=1;

		switch (e.key.keysym.sym) {

		case SDLK_ESCAPE:
		    if (quit==2)
			quit=1;
		    else {
			quit=2;
			kill_all_hiiret(&maa1);
                        kill_all_pallot(&maa1);
		    }
		    break;
		default:
                    break;
		}
	    }
	}
    }

    SDL_Quit();

    return 0;
}
