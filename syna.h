/*
    Definitions for Syna

    - Marq
*/

enum {  Nc1=0,NC1,Nd1,ND1, Ne1,Nf1,NF1,Ng1, NG1,Na1,NA1,Nb1,
        Nc2,  NC2,Nd2,ND2, Ne2,Nf2,NF2,Ng2, NG2,Na2,NA2,Nb2,
        Nc3,  NC3,Nd3,ND3, Ne3,Nf3,NF3,Ng3, NG3,Na3,NA3,Nb3,
        Nc4,  NC4,Nd4,ND4, Ne4,Nf4,NF4,Ng4, NG4,Na4,NA4,Nb4,
        Nc5,  NC5,Nd5,ND5, Ne5,Nf5,NF5,Ng5, NG5,Na5,NA5,Nb5,
        Nc6,  NC6,Nd6,ND6, Ne6,Nf6,NF6,Ng6, NG6,Na6,NA6,Nb6,
        NECHON, NECHOFF, NSTOP, NVOL, NSLIDE, NEMPTY /* Special "notes" */
     };

/* Some variables that might be useful */
extern int syna_counter,syna_row;

/* Init the whole shit. Instruments etc. Call this first of all.
   freq=main frequency */
void syna_init(int freq);

#ifndef TINY
/* Loads a tune from a file. 0=OK, -1=error */
int syna_load(char *tune);
#endif

/* Gets a tune from memory */
int syna_get(void *tune);

/* Plays a fragment to destination */
void syna_play(short *dest,int len);

/* Sets channel volume [0..1]. Channels start from 1 */
void syna_setvol(int chn,float nvol);

/* Sets BPM */
void syna_setbpm(int bpm);

/* Trigs channel with note (see enum above) */
void syna_trig(int chn,int note);

/* (Un)mutes channel 0=normal 1=mute, delay=0 - mute at once, delay=1 -
   mute when the next row starts */
void syna_mute(int chn,int nmute,int delay);

/* Transposes a channel. Negative tp transposes down, positive up. Not
   relative to the previous transposition! */
void syna_transpose(int chn,int tp);
