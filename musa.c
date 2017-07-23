#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>

#include <SDL.h>


#include "syna.h"
#include "paa.h"
#include "misc.h"


void pakki(void *udata, Uint8 *stream, int len)
{
    syna_play((short *)stream,len/4);
}

int init_musa() {
    SDL_AudioSpec   w;
    w.freq=44100;
    w.format=AUDIO_S16SYS;
    w.channels=2;
    w.samples=BLKSIZE;
    w.callback=pakki;
    w.userdata=NULL;
    SDL_OpenAudio(&w,NULL);

    syna_init(44100);
    if(syna_load("data/hiiri.txt")==-1)
    {
        printf("musa load error!\n");
        return(-1);
    }
    return 0;

}

#define NC 13
#define HARHAILU1 3
// 1 sini plays p‰‰
// 2 sini plays kili
// 3 saha plays p‰‰
// 4 saha plays kili 1
// 5 saha plays kili 2
// 6 psykedeliaujellus
// 7 ihme‰‰ni
// 8 basso
// 9 leadi
// 10 bd
//11 hh
//12 saha plays kili 3
//13 snare

float patterns[][NC]={
 //  1 2 3 4 5 6 7 8 9 0 1 2 3

    {0,0,0,0,0,1,1,0,0,0,0,0,0}, // *0 = ambient1
    {0,1,0,0,0,0,1,0,0,0,0,0,0}, // *1 = ambient+kilitys+sini
    {1,0,0,0,0,0,1,0,0,0,0,0,0}, // *2 = sini
    {1,0,1,0,0,0,1,0,0,0,0,0,0}, // *3 = sini + saha
    {0,0,0,0,0,1,1,1,0,1,1,0,1}, // *4
    {0,0,0,0,0,0,1,1,0,1,1,0,1}, // *5
    {0,1,0,0,0,0,1,1,0,1,1,1,1}, // *6
    {0,1,0,0,1,0,1,1,1,1,1,0,1}, // *7
    {0,1,1,0,0,0,1,1,1,1,1,0,1}, // *8
    {0,0,0,0,1,1,0,1,0,1,1,0,1}, // *9
    {1,0,0,0,0,1,0,0,1,0,0,0,0}, // *10 = leadi + jotain
    {1,0,0,0,0,0,0,0,1,0,0,0,0}, // *11 = ihan pelkk‰ numan-leadi
    {1,0,0,0,0,0,0,0,0,0,0,0,0}, // *12 = ambient1
    {1,1,0,0,0,0,1,0,0,0,0,0,0}, // *13 = ambient+kilitys+sini
    {1,0,1,0,0,0,0,0,0,0,0,0,0}, // *14 = sini + saha
    {1,0,0,0,0,1,1,0,0,0,0,0,0}, // *15 = sini + saha
    {0,1,0,0,1,0,0,1,0,1,1,0,1}, // *16
    {1,1,0,0,1,0,0,1,0,1,1,0,1}, // *17

    /*
    {0,1,0,0,0,0,1,1,0,1,1,1,1},
    {0,1,0,0,1,0,1,1,1,1,1,0,1},
    {0,1,1,0,0,0,1,1,1,1,1,0,1},
    {0,0,0,0,0,1,1,1,0,1,1,0,1},
    {0,0,0,0,0,0,1,0,0,1,1,0,1},
    {1,0,0,0,0,0,0,0,1,0,0,0,0},
    {0,0,0,1,0,0,1,1,1,0,1,0,0},
    */
};

int rauh_order[]=
{14,3,3,3,11,10,12,12,2,2,15,2};

int tekno_order[]=
{4,5,6,6,7,8,9,16,16,17};

int intro_order[]=
{0,1,1,13};

void start_musa() {
}

enum {
    INTRO,
    RAUHALLINEN,
    ACTIONI};

void go_musa() {
    int pat=3;

    static int times_in_this_part=4;

    static int musamoodi=INTRO;
    static int this_part_started=1;
    static int transpose=0;
    static int tra_done=0;
    int t;
    int i;
    int r4=syna_row % 4;
    static int new4=0;
    int r16=syna_row % 16;
    static int new16=0;
    int r32=syna_row % 32;
    static int new32=0;
    int r64=syna_row % 64;
    static int new64=0;
    int r128=syna_row % 128;
    static int new128=0;
    int r256=syna_row % 256;
    static int new256=0;
    static int modeflip=0;

    static int rauh_pos=-1;
    static int rauh_inc=0;
    static int tekno_pos=-1;
    static int tekno_inc=0;
    static int intro_pos=0;
    static int intro_inc=0;

    if (musamoodi==INTRO)
	intro_osa(1);

    if ((r128==127) && (!tra_done)) {
	t=rnd()*4;
	switch (t) {
	case 0:
	    transpose=0;
            break;
	case 1:
	    transpose=4;
            break;
	case 2:
	    transpose=-4;
            break;
	case 3:
	    transpose=-8;
            break;
	}
        tra_done=1;
    }

    if (r128==0)
        tra_done=0;

    if (musamoodi==INTRO)
	transpose=0;

    if ((r64==63) && (!modeflip)) {
	//printf("times left in this part: %d\n",--times_in_this_part);
        times_in_this_part--;
	if (times_in_this_part==0) {
	    this_part_started=1;

	    if (musamoodi==INTRO) {
		intro_osa(0); // inform intro has ended
		musamoodi=RAUHALLINEN;
                times_in_this_part=(1+((int)rnd()*2))*2;
	    } else {

		musamoodi=rnd()*2+1; // either RAUHALLINEN or ACTIONI
		switch (musamoodi) {
		case RAUHALLINEN:
		    times_in_this_part=(1+((int)rnd()*2))*2;
		    break;
		case ACTIONI:
		    times_in_this_part=(2+((int)rnd()*2))*2;
		    break;
		}
	    }
	}
        modeflip=1;
    }

    if (r64==0)
	modeflip=0;

    switch (musamoodi) {
    case INTRO:

	if ((r64==63) && (intro_inc==0)) {
	    intro_pos++;
            intro_inc=1;
	}
	if (r64==0)
	    intro_inc=0;

	pat=intro_order[intro_pos % (sizeof(intro_order)/sizeof(int))];

        break;
    case RAUHALLINEN:
	if ((r64==63) && (rauh_inc==0)) {
	    rauh_pos++;
            rauh_inc=1;
	}
	if (r64==0)
	    rauh_inc=0;

	pat=rauh_order[rauh_pos % (sizeof(rauh_order)/sizeof(int))];

	break;
    case ACTIONI:
	if ((r64==63) && (tekno_inc==0)) {
	    tekno_pos++;
            tekno_inc=1;
	}
	if (r64==0)
	    tekno_inc=0;

	pat=tekno_order[tekno_pos % (sizeof(tekno_order)/sizeof(int))];

	break;
    }

    if ((r4==0) && (new4==0)) {
	switch (musamoodi) {
	case RAUHALLINEN:
	    rauhallinen(TRIG4,this_part_started);
	    break;
	case ACTIONI:
            actioni(TRIG4,this_part_started);
	    break;
	}
	new4=1;
    }

    if ((r4==1) && (new4==1))
	new4=0;

    if ((r16==0) && (new16==0)) {
	switch (musamoodi) {
	case RAUHALLINEN:
	    rauhallinen(TRIG16,this_part_started);
	    break;
	case ACTIONI:
            actioni(TRIG16,this_part_started);
	    break;
	}
	new16=1;
    }

    if ((r16==1) && (new16==1))
	new16=0;

    if ((r32==0) && (new32==0)) {
	switch (musamoodi) {
	case RAUHALLINEN:
	    rauhallinen(TRIG32,this_part_started);
	    break;
	case ACTIONI:
            actioni(TRIG32,this_part_started);
	    break;
	}
        new32=1;
    }

    if ((r32==1) && (new32==1))
	new32=0;

    if ((r64==0) && (new64==0)) {
	switch (musamoodi) {
	case RAUHALLINEN:
	    rauhallinen(TRIG64,this_part_started);
	    break;
	case ACTIONI:
            actioni(TRIG64,this_part_started);
	    break;
	}

        new64=1;
    }

    if ((r64==1) && (new64==1))
	new64=0;

    if ((r128==0) && (new128==0)) {
	switch (musamoodi) {
	case RAUHALLINEN:
	    rauhallinen(TRIG128,this_part_started);
	    break;
	case ACTIONI:
            actioni(TRIG128,this_part_started);
	    break;
	}

        new128=1;
    }

    if ((r128==1) && (new128==1))
	new128=0;

    if ((r256==0) && (new256==0)) {
	switch (musamoodi) {
	case RAUHALLINEN:
	    rauhallinen(TRIG256,this_part_started);
	    break;
	case ACTIONI:
            actioni(TRIG256,this_part_started);
	    break;
	}

	new256=1;
    }

    if ((r256==1) && (new256==1))
	new256=0;

    if (r64==0)
	this_part_started=0;

    SDL_PauseAudio(0);

    for (i=0; i<NC; i++)
	if ((i!=7) && (i!=10) && (i!=11) && (i!=13))
	    syna_transpose(i,transpose);

    for (i=0; i<NC; i++)
	syna_mute(i+1,1-patterns[pat][i],1);


}
