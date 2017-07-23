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
#include "misc.h"

void steer_harhaile(HIIRI * h, MAAILMA * m, float dt) {
    // harhailee ympäriinsä
    float e1,e2,e3;
    // uuteen suuntaan menemisen nopeus
#define HFD 0.99
    //kaaren laajuus randomia suuntaa otettaessa
#define KAARID 180.0
    // laskurin raja
#define RAJAD 0.75

    // arvo uusi suunta, kun laskuri on täynnä
    h->counter+=dt;
    if (h->counter>RAJAD) {
	h->counter=0.0;
	h->dir3=h->dir2+KAARID*(rnd()-0.5)*2.0;
    }

    /* seinille tultaessa käännä hiiri ympäri */
    if (h->pos[1]<(-m->clip[1]+h->dodge_radius))
	h->dir3=90.0;

    if (h->pos[1]>(m->clip[1]-h->dodge_radius))
	h->dir3=270.0;

    if (h->pos[0]>(m->clip[0]-h->dodge_radius))
	h->dir3=180.0;

    if (h->pos[0]<(-m->clip[0]+h->dodge_radius)) {
	h->dir3=0.0;
    }

    // interpoloi nykyistä suuntaa kohti kakkossuuntaa
    e1=fabs(h->dir3-h->dir2);
    e2=fabs(h->dir3+360.0-h->dir2);
    e3=fabs(h->dir3-360.0-h->dir2);

    if (e3<e1)
	h->dir2+=360.0;
    if (e2<e1)
        h->dir2-=360.0;


    h->dir2=HFD*h->dir2+(1.0-HFD)*h->dir3;

    h->desire[0]=cos(h->dir2*2.0*M_PI/360.0)*h->spd*h->spd_factor;
    h->desire[1]=sin(h->dir2*2.0*M_PI/360.0)*h->spd*h->spd_factor;

}


float global_desire_f=DEFAULT_DESIRE_F;
float new_global_desire_f=DEFAULT_DESIRE_F;
#define GDFF 0.9

void move_hiiri(HIIRI * h, float dt) {
    int i;
    float hf=global_desire_f;
    // liikuta hiirtä nopeuden suuntaan
    for (i=0; i<3; i++) {
	h->current_desire[i]=hf*h->current_desire[i]+
	    (1.0-hf)*h->desire[i];

	h->pos[i]+=(h->current_desire[i]+h->rejection[i])*dt;
;
    }

}

void init_maailma(MAAILMA * m, float clipx, float clipy, float clipz) {
    m->is_borders=1;

    m->gcounter=0.0;

    m->checker[0]=m->new_checker[0]=0.3;
    m->checker[1]=m->new_checker[1]=0.2;
    m->checker[2]=m->new_checker[2]=0.1;

    m->hiiret=NULL;
    m->seinat=NULL;
    m->pallot=NULL;

    m->goal[0]=0.0;
    m->goal[1]=0.5;
    m->goal[2]=0.0;

    m->clip[0]=clipx;
    m->clip[1]=clipy;
    m->clip[2]=clipz;
}

HIIRI * find_preceding(HIIRI * h,MAAILMA * m) {
    HIIRI * h2,*hf;
    h2=m->hiiret;
    hf=NULL;
    while ((h2!=NULL) && (hf==NULL)) {
	if (h2->next==h) {
	    hf=h2;
	}
        h2=h2->next;
    }
    return hf;
}

PALLO * find_preceding_pallo(PALLO * h,MAAILMA * m) {
    PALLO * h2,*hf;
    h2=m->pallot;
    hf=NULL;
    while ((h2!=NULL) && (hf==NULL)) {
	if (h2->next==h) {
	    hf=h2;
	}
        h2=h2->next;
    }
    return hf;
}


void kill_all_hiiret(MAAILMA * m) {
    HIIRI * h;
    h=m->hiiret;
    while (h!=NULL) {
	if (h->action==MOVE) {
	    h->spd_factor=1.0;
	    h->action=STOP_AND_DIE;
	}
	h=h->next;
    }
}
void kill_last_added_hiiri(MAAILMA * m) {
    HIIRI * h=m->hiiret;
    //m->hiiret=h->next;
    h->spd_factor=1.0;
    h->action=STOP_AND_DIE;


}

HIIRI * last_hiiri(MAAILMA * m) {
    HIIRI * h,* last=NULL;
    h=m->hiiret;
    while (h!=NULL) {
	last=h;
        h=h->next;
    }
    return last;
}

void kill_oldest_hiiri_alive(MAAILMA * m) {
    HIIRI * h=last_hiiri(m);

    while ((h->action==STOP_AND_DIE) || (h->action==DIE)) {
	h=find_preceding(h,m);
	if (h==NULL)
            return;
    }
    h->spd_factor=1.0;
    h->action=STOP_AND_DIE;

}

void kill_all_pallot(MAAILMA * m) {
    PALLO * pt=m->pallot;

    while (pt!=NULL) {
	if (pt->mode!=DIE) {
	    pt->mode=DIE;
	    pt->die_counter=0.0;
	}
	pt=pt->next;
    }

}

void add_pallo(MAAILMA * m, int tyyppi,int n) {
    PALLO * p;
    static int pallo_id=0;
//    int n=rnd()*4.0;

    p=malloc(sizeof(PALLO));
    p->next=m->pallot;
    m->pallot=p;
    p->id=pallo_id++;

    switch (n) {
    case 0:
	p->pos0[0]=1.0;
	p->pos0[1]=0.5;
        break;
    case 1:
	p->pos0[0]=-1.0;
	p->pos0[1]=0.5;
        break;
    case 2:
	p->pos0[0]=0.5;
	p->pos0[1]=1.0;
        break;
    case 3:
	p->pos0[0]=0.5;
	p->pos0[1]=-1.0;
	break;
    }



    p->mode=MOVE;

    memcpy(p->posn,p->pos0,sizeof(float)*3);
    memcpy(p->pos,p->pos0,sizeof(float)*3);
    p->pos1[0]=0.0;
    p->pos1[1]=0.0;
    p->pos2[0]=0.0;
    p->pos2[1]=0.0;

    p->yi=0;
    p->yn=8;
    p->vierin=0.0;
    p->pos[2]=0.0;

    p->counter=0.0;
    p->tyyppi=tyyppi;

    switch (tyyppi) {
    case RANTAPALLO:
	p->pomppu_h=0.35;
	p->p_counter_raja=0.9;
	p->counter_raja=0.8;
	p->spd=0.01;
        p->r=0.1;
	break;
    case KEILAPALLO:
	p->pomppu_h=0.0;
	p->p_counter_raja=0.5;
	p->counter_raja=1.0;
	p->spd=1.0;
        p->r=0.1;
        break;
    case JALKAPALLO:
	p->pomppu_h=0.0;
	p->p_counter_raja=0.5;
	p->counter_raja=0.6;
	p->spd=1.0;
        p->r=0.1;
        break;
    case TENNARI:
	p->pomppu_h=0.25;
	p->p_counter_raja=0.8;
	p->counter_raja=0.7;
	p->spd=1.0;
        p->r=0.03;
        break;
    }
    p->counter_raja*=(0.5+rnd());

}

HIIRI * default_hiiri_stuff(MAAILMA * m,float x, float y, float z, float dir, float spd) {
    // paikan, nopeuden ja suunnan säätö
    HIIRI * h;
    static int hiiri_id=0;

    h=malloc(sizeof(HIIRI));
    h->next=m->hiiret;
    m->hiiret=h;

    h->id=hiiri_id++;

    h->pos[0]=x;
    h->pos[1]=y;
    h->pos[2]=z;

    h->desire[0]=0.0;
    h->desire[1]=h->desire[2]=0.0;
    memcpy(h->current_desire,h->desire,sizeof(float)*3);
    h->desire_f=DEFAULT_DESIRE_F;

    h->rejection[0]=h->rejection[1]=h->rejection[2]=0.0;

    h->dir=dir;
    h->dir2=dir;
    h->dir3=dir;
    h->rejection_strength=1.0;

    h->vakkyra_kulma=0.0;

    h->spd=spd;
    h->old_spd=spd;
    h->spd_factor=1.0;

    h->counter=10.0;
    h->die_counter=0.0;

    h->radius=DEFAULT_RADIUS;
    h->dodge_radius=DEFAULT_DODGE_RADIUS;

    h->color=COLOR_BLUE;

    h->first_time_no_border_dodge=1;

    h->action=MOVE;

    return h;
}

PALLO * pallo_is_inside_other_pallo(PALLO * p, MAAILMA * m) {
    PALLO * p2, * tp;
    int i;
    float pd[3],d;

    p2=m->pallot;
    tp=NULL;
    while ((p2!=NULL) && (tp==0)) {
	if (p->id!=p2->id) {
	    for (i=0; i<3; i++)
		pd[i]=p2->pos[i]-p->pos[i];
	    d=sqrt(pd[0]*pd[0]+pd[1]*pd[1]+pd[2]*pd[2])-p2->r;
	    if (d<=p->r)
		tp=p2;

	}
	p2=p2->next;
    }
    return tp;
}

HIIRI * hiiri_is_inside_other_hiiri_r(HIIRI * h, MAAILMA * m, float r) {
    // palauttaa ekan törmäävän hiiren
    HIIRI * h2, * th;
    int i;
    float pd[3],d;

    h2=m->hiiret;
    th=NULL;
    while ((h2!=NULL) && (th==0)) {
        // kunhan ei ole sama hiiri..
	if (h->id!=h2->id) {
	    for (i=0; i<3; i++)
		pd[i]=h2->pos[i]-h->pos[i];
            // hiiren keskipisteen ja toisen ulkokehän etäisyys
	    d=sqrt(pd[0]*pd[0]+pd[1]*pd[1]+pd[2]*pd[2])-h2->radius;
	    if (d<=r)
                th=h2;

	}
        h2=h2->next;
    }
    return th;
}

HIIRI * pos_is_inside_other_hiiri(float x, float y, float z, MAAILMA * m) {
    // palauttaa ekan törmäävän hiiren
    HIIRI * h2, * th;
    float r=DEFAULT_RADIUS;
    float pd[3],d;

    h2=m->hiiret;
    th=NULL;
    while ((h2!=NULL) && (th==0)) {
	pd[0]=h2->pos[0]-x;
	pd[1]=h2->pos[1]-y;
	pd[2]=h2->pos[2]-z;
	// hiiren keskipisteen ja toisen ulkokehän etäisyys
	d=sqrt(pd[0]*pd[0]+pd[1]*pd[1]+pd[2]*pd[2])-h2->radius;
	if (d<=r)
	    th=h2;

	h2=h2->next;
    }
    return th;
}

void dodge_pallo(HIIRI * h, float x,float y,float z) {
#define PVOIM 0.000000001
    float d,v,dv[3];
    int i;

    dv[0]=x-h->pos[0];
    dv[1]=y-h->pos[1];
    dv[2]=(z-h->pos[2])*0;

    d=sqrt(dv[0]*dv[0]+dv[1]*dv[1]+dv[2]+dv[2]);
    if (d!=0.0)
	v=(1.0/(d*d*d*d*d*d*d*d*d*d*d))*PVOIM;
    else
	v=1.0;

    if (v>50.0)
	v=50.0;

    v*=(0.5-z);
    for (i=0; i<3; i++)
	h->rejection[i]-=dv[i]*v;
}

void dodge_seina(HIIRI * h, SEINA * s) {
    return;
}

void check_if_still_first_time(HIIRI * h, MAAILMA * m) {

    if ((h->pos[1]<(m->clip[1]-h->radius)) &&
	(h->pos[1]>(-m->clip[1]+h->radius)) &&
	(h->pos[0]<(m->clip[0]-h->radius)) &&
	(h->pos[0]>(-m->clip[0]+h->radius)))
        h->first_time_no_border_dodge=0;

}

void dodge_borders(HIIRI * h, MAAILMA * m) {
#define BVOIM 0.00000000005
#define BR 50.0

    float d,v;

    if (h->first_time_no_border_dodge)
	return;

    /* maailman rajat */
    /* ylä */

    d=fabs(+m->clip[1]-h->pos[1]);
    if (d!=0.0)
	v=(1.0/(d*d*d*d*d*d*d*d*d*d*d))*BVOIM*0.001;
    else
	v=1.0;

    if (v>BR)
	v=BR;

    h->rejection[1]-=d*v;
    /* ala */

    d=fabs(-m->clip[1]-h->pos[1]);
    if (d!=0.0)
	v=(1.0/(d*d*d*d*d*d*d*d*d*d*d))*BVOIM*0.001;
    else
	v=1.0;

    if (v>BR)
	v=BR;

    h->rejection[1]+=d*v;

    /* vasen */
    d=fabs(-m->clip[0]-h->pos[0]);
    if (d!=0.0)
	v=(1.0/(d*d*d*d*d*d*d*d*d*d*d))*BVOIM*0.001;
    else
	v=1.0;

    if (v>BR)
	v=BR;

    h->rejection[0]+=d*v;

    /* oikea */
    d=fabs(m->clip[0]-h->pos[0]);
    if (d!=0.0)
	v=(1.0/(d*d*d*d*d*d*d*d*d*d*d))*BVOIM*0.001;
    else
	v=1.0;

    if (v>BR)
	v=BR;

    h->rejection[0]-=d*v;
}

void dodge_pallot_against_hiiret(PALLO * p, HIIRI * hiiret) {
#define CVOIM 0.00000000005

    HIIRI * h2;
    float dv[3]={0.0,0.0,0.0},d,v;
    int i;
    h2=hiiret;

    while (h2!=NULL) {
	for (i=0; i<2; i++)
	    dv[i]=h2->pos[i]-p->pos[i];

	d=sqrt(dv[0]*dv[0]+dv[1]*dv[1]);
	if (d!=0.0)
	    v=(1.0/(d*d*d*d*d*d*d*d*d*d*d))*CVOIM;
	else
	    v=1.0;

	for (i=0; i<2; i++)
	    p->rejection[i]-=dv[i]*v;
	h2=h2->next;
    }


}
void dodge_pallot_against_pallot(PALLO * p, PALLO * pallot) {
#define PPVOIM2 0.0000006
//#define PPVOIM2 0.0005

    PALLO * p2;
    float dv[3]={0.0,0.0,0.0},d,v;
    int i;

    p2=pallot;

    while (p2!=NULL) {

	if (p2->id!=p->id) {

	    for (i=0; i<2; i++)
		dv[i]=p2->pos[i]-p->pos[i];

	    d=sqrt(dv[0]*dv[0]+dv[1]*dv[1]);
	    //printf("d=%f\n",d);

	    if (d!=0.0)
		v=(1.0/(d*d*d*d*d*d*d*d*d*d*d))*PPVOIM2;
	    else
		v=1.0;

//	    if (v>10.0)
//                v=10.0;
	    for (i=0; i<2; i++)
		p->rejection[i]-=dv[i]*v;

	}
	p2=p2->next;
    }
}

void dodge_pallot(HIIRI * h, PALLO * p) {
#define PPVOIM 0.00000000005

    PALLO * p2;
    float dv[3]={0.0,0.0,0.0},d,v;
    int i;

    p2=p;

    while (p2!=NULL) {

        dv[2]=0.0;
	for (i=0; i<2; i++)
	    dv[i]=p2->pos[i]-h->pos[i];

	d=sqrt(dv[0]*dv[0]+dv[1]*dv[1]+dv[2]+dv[2]);
	if (d!=0.0)
	    v=(1.0/(d*d*d*d*d*d*d*d*d*d*d))*PPVOIM;
	else
	    v=1.0;

	for (i=0; i<3; i++)
	    h->rejection[i]-=dv[i]*v;
	p2=p2->next;
    }
}

void calc_pallo_rejection(PALLO * p, MAAILMA * m) {
    int i;
    for (i=0; i<3; i++)
	p->rejection[i]=0.0;

    dodge_pallot_against_hiiret(p,m->hiiret);
    dodge_pallot_against_pallot(p,m->pallot);

}
void calc_dodge_powers(HIIRI * h, MAAILMA * m, float dt) {
#define VOIM 0.00000000005
//#define VOIM 0.0000000001

    HIIRI * h2;
    float dv[3]={0.0,0.0,0.0},d,v;
    int i;
    h2=m->hiiret;

    h->rejection[0]=0.0;
    h->rejection[1]=0.0;
    h->rejection[2]=0.0;

    while (h2!=NULL) {
	if (h2->id!=h->id) {
            dv[2]=0.0;
	    for (i=0; i<2; i++)
		dv[i]=h2->pos[i]-h->pos[i];

	    d=sqrt(dv[0]*dv[0]+dv[1]*dv[1]+dv[2]+dv[2]);
            if (d!=0.0)
		v=(1.0/(d*d*d*d*d*d*d*d*d*d*d))*VOIM;
	    else
		v=1.0;

	    for (i=0; i<3; i++)
		h->rejection[i]-=dv[i]*v;
	}
	h2=h2->next;
    }

    //dodge_pallo(h,m->goal[0],m->goal[1],m->gz);
    dodge_pallot(h,m->pallot);

    if (m->is_borders)
	dodge_borders(h,m);


}
HIIRI * hiiri_is_inside_other_hiiri(HIIRI * h, MAAILMA * m) {
    // palauttaa ekan törmäävän hiiren
    return (hiiri_is_inside_other_hiiri_r(h,m,h->radius*1.0));
}

void calc_dir(HIIRI * h) {
    float dx=(h->desire[0]+0*h->rejection[0]);
    float dy=(h->desire[1]+0*h->rejection[1]);

    if (h->spd_factor==0.0) {
	h->dir=h->old_dir;
        return;
    }

    if (dx==0) {
        if (dy<0)
	    h->dir=270.0;
	else
	    h->dir=90.0;

    } else {
	if (dx>=0)
	    h->dir=atan(dy/dx)*90.0/(M_PI/2.0);
        else
	    h->dir=atan(dy/dx)*90.0/(M_PI/2.0)+180.0;

	while (h->dir<0)
            h->dir+=360.0;
    }

}

void calc_pallo_dir(PALLO * p) {
    float dx=(p->pos[0]-p->opos[0]);
    float dy=(p->pos[1]-p->opos[1]);

    if (dx==0) {
        if (dy<0)
	    p->dir=270.0;
	else
	    p->dir=90.0;

    } else {
	if (dx>=0)
	    p->dir=atan(dy/dx)*90.0/(M_PI/2.0);
        else
	    p->dir=atan(dy/dx)*90.0/(M_PI/2.0)+180.0;

	while (p->dir<0)
            p->dir+=360.0;
    }

}


void calc_desire_vector(HIIRI * h, float * goal) {
#define DK (1.0*h->spd*h->spd_factor)
    int i;

    h->desire[0]=h->desire[1]=h->desire[2]=0.0;

    if ((fabs(goal[0])>10.0) || (fabs(goal[1])>10.0))
	return;

    for (i=0; i<2; i++)
        h->desire[i]=(goal[i]-h->pos[i])*DK;

}

void update_hiiri(HIIRI * h, MAAILMA * m,float dt) {
    HIIRI tmp;
    HIIRI * hf;
    float k[3];
    float v[3]={0.0,0.0,0.0};
    /* jemmaa vanha paikka */

    memcpy(tmp.pos,h->pos,sizeof(float)*3);

    check_if_still_first_time(h,m);

    switch (h->action) {
    case STOP:
	if (h->spd_factor>0.0)
	    h->old_dir=h->dir;

	h->spd_factor-=dt;
	if (h->spd_factor<0.0)
            h->spd_factor=0.0;
	break;
    case STOP_AND_DIE:
	if (h->spd_factor>0.0)
	    h->old_dir=h->dir;

	h->spd_factor-=dt*3.0;
	if (h->spd_factor<0.0) {
	    h->spd_factor=0.0;
            h->die_counter=0.0;
	    h->action=DIE;
	}
	break;
    case DIE:
	h->die_counter+=dt;
	h->dir-=h->die_counter*20.0;
	h->old_dir=h->dir;
	if (h->die_counter>1.5) {
	    if (m->hiiret==h)
		m->hiiret=h->next;
            else
		(find_preceding(h,m))->next=h->next;
	}
	break;
    case MOVE:
	h->old_spd=h->spd;
	break;
    }
    /* liikutus */
    switch (h->move_state) {

    case MOVE_TOWARDS_GOAL:
	//calc_desire_vector(h,&m->goal[0]);
        if (m->pallot==NULL)
	    calc_desire_vector(h,&v[0]);
        else
	    calc_desire_vector(h,&m->pallot->pos[0]);
	calc_dodge_powers(h,m,dt);
	calc_dir(h);
	move_hiiri(h,dt);
	break;
    case MOVE_HARHAILE:
	steer_harhaile(h,m,dt);
	calc_dodge_powers(h,m,dt);
	calc_dir(h);
	move_hiiri(h,dt);
	break;
    case MOVE_TOWARDS_ANOTHER:
        hf=h->follow_hiiri;

	if (hf!=NULL) {
	    k[2]=0.0;
	    k[0]=hf->pos[0]-cos(hf->dir*2*M_PI/360.0)*hf->radius;
	    k[1]=hf->pos[1]+sin(hf->dir*2*M_PI/360.0)*hf->radius;
	} else
	    k[0]=k[1]=k[2]=0.0;

	calc_desire_vector(h,&k[0]);
        calc_dodge_powers(h,m,dt);
	calc_dir(h);
	move_hiiri(h,dt);
	break;
    default:
	break;
    }

    /* toisten hiirten tsekkaus */
    switch (h->dodge_others) {
    case DODGE_STOP:
	if (hiiri_is_inside_other_hiiri(h,m))
	    memcpy(h->pos,tmp.pos,sizeof(float)*3);
	break;

    default:
        break;
    }

}

typedef float Vect[3];

void splineTCP(float u, Vect * control, Vect * result, float tension, float continuity, float bias) {
    (*result)[0] = (2*u*u*u - 3*u*u + 1)*control[1][0]
	+ (-2*u*u*u + 3*u*u)*control[2][0]
	+ (u*u*u - 2*u*u + u)*(0.5*(1-tension)*((1+bias)*(1-continuity)*(control[1][0]-control[0][0])
						+ (1-bias)*(1+continuity)*(control[2][0]-control[1][0])))
	+ (u*u*u - u*u)*(0.5*(1-tension)*((1+bias)*(1+continuity)*(control[2][0]-control[1][0])
					  + (1-bias)*(1-continuity)*(control[3][0]-control[2][0])));

    (*result)[1]
	= (2*u*u*u - 3*u*u + 1)*control[1][1]
	+ (-2*u*u*u + 3*u*u)*control[2][1]
	+ (u*u*u - 2*u*u + u)*(0.5*(1-tension)*((1+bias)*(1-continuity)*(control[1][1]-control[0][1])
						+ (1-bias)*(1+continuity)*(control[2][1]-control[1][1])))
	+ (u*u*u - u*u)*(0.5*(1-tension)*((1+bias)*(1+continuity)*(control[2][1]-control[1][1])
					  + (1-bias)*(1-continuity)*(control[3][1]-control[2][1])));

}

void update_pallo(PALLO * p, MAAILMA * m, float dt) {
    Vect c[4];
    PALLO * ed;
    int i;
    float d;
    float dv[2];
    Vect spos;
    Vect suunta;
    float f;
    for (i=0; i<2; i++)
	dv[i]=p->pos[i]-p->opos[i];

    d=(dv[0]*dv[0]+dv[1]*dv[1]);

    if (d>0.1)
        d=0.1;
    p->vierin+=75.0*30000.0*d*dt;

    memcpy(p->opos,p->pos,sizeof(Vect));

    p->p_counter+=dt;
    if (p->p_counter>p->p_counter_raja) {
        p->p_counter=0.0;
    }
    f=p->p_counter/p->p_counter_raja;

    switch (p->mode) {
    case MOVE:
	p->pos[2]=p->pomppu_h-(f-0.5)*(f-0.5)*4.0*p->pomppu_h;
	break;
    case DIE:
        p->die_counter+=dt;
	p->pos[2]+=p->die_counter*p->die_counter;
	if (p->die_counter>1.0) {
	    ed=find_preceding_pallo(p,m);

	    if (ed==NULL)
		m->pallot=NULL;
            else
		(find_preceding_pallo(p,m))->next=p->next;

            return;
	}
	break;
    }

    p->counter+=dt;
    if (p->counter>p->counter_raja) {
	p->counter=0.0;
        p->yi++;
	memcpy(p->pos0,p->posn,sizeof(Vect));
	memcpy(p->posn,p->pos1,sizeof(Vect));
	memcpy(p->pos1,p->pos2,sizeof(Vect));
	p->pos2[0]=(m->clip[0]-p->r)*cos(p->yi*2*M_PI/p->yn)+(rnd()-0.5)*0.4;
	p->pos2[1]=(m->clip[0]-p->r)*sin(p->yi*2*M_PI/p->yn)+(rnd()-0.5)*0.4;
    }

    memcpy(c[0],p->pos0,sizeof(Vect));
    memcpy(c[1],p->posn,sizeof(Vect));
    memcpy(c[2],p->pos1,sizeof(Vect));
    memcpy(c[3],p->pos2,sizeof(Vect));
    splineTCP(p->counter/p->counter_raja,
	      &c[0],
	      &spos,
	      0.0,0.0,0.0);

    for (i=0; i<2; i++)
	suunta[i]=spos[i]-p->pos[i];

    for (i=0; i<2; i++)
	p->pos[i]+=suunta[i]*dt;

    calc_pallo_dir(p);

    calc_pallo_rejection(p,m);

    if ((fabs(p->rejection[0])>10.0) ||
	(fabs(p->rejection[1])>10.0))
	return;

    for (i=0; i<2; i++)
	p->pos[i]+=p->rejection[i]*dt;

}

void update_maailma(MAAILMA * m, float dt) {
    HIIRI * h;
    PALLO * p;
    int i;

    for (i=0; i<3; i++)
	m->checker[i]=CHF*m->checker[i]+
	    (1.0-CHF)*m->new_checker[i];

    global_desire_f=GDFF*global_desire_f+
	(1.0-GDFF)*new_global_desire_f;

    // päivitä pallot
    p=m->pallot;
    while (p!=NULL) {
	update_pallo(p,m,dt);
        p=p->next;
    }

    h=m->hiiret;
    while (h!=NULL) {
	update_hiiri(h,m,dt);
        h=h->next;
    }

}

HIIRI * add_harhaileva_hiiri(MAAILMA * m, float x, float y, float z, float dir) {
    HIIRI * h;

    if (pos_is_inside_other_hiiri(x,y,z,m))
	return NULL;

    h=default_hiiri_stuff(m,x,y,z,dir,1.0*DEFAULT_SPD*(0.5+rnd()));

    /* arvo paikka, joka ei mene toisen hiiren kanssa päällekkäin */

    h->color=COLOR_GREEN;

    h->move_state=MOVE_HARHAILE;
    h->dodge_walls=DODGE_NONE;
    h->dodge_others=DODGE_STOP;

    return h;
}

HIIRI * add_goal_hiiri(MAAILMA * m, float x, float y, float z, float dir) {
    HIIRI * h;

    if (pos_is_inside_other_hiiri(x,y,z,m))
	return NULL;

    h=default_hiiri_stuff(m,x,y,z,dir,3.0*DEFAULT_SPD*(0.5+rnd()));

    /* arvo paikka, joka ei mene toisen hiiren kanssa päällekkäin */

    h->color=COLOR_YELLOW;

    h->move_state=MOVE_TOWARDS_GOAL;
    h->dodge_walls=DODGE_NONE;
    h->dodge_others=DODGE_STOP;

    return h;
}

HIIRI * add_following_hiiri(MAAILMA * m, float x, float y, float z, float dir, HIIRI * hf) {
    HIIRI * h;

    if (pos_is_inside_other_hiiri(x,y,z,m))
	return NULL;

    h=default_hiiri_stuff(m,x,y,z,dir,2.0*DEFAULT_SPD);

    /* arvo paikka, joka ei mene toisen hiiren kanssa päällekkäin */

    h->color=COLOR_BLUE;
    h->follow_hiiri=hf;

    h->move_state=MOVE_TOWARDS_ANOTHER;
    h->dodge_walls=DODGE_NONE;
    h->dodge_others=DODGE_STOP;

    return h;
}
