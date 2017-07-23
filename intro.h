#ifndef INTRO_H
#define INTRO_H

/* The lower 8 bits of 'seed' are used (each of them). tid=milliseconds
   starting from zero. */
void intro(double tid,int seed);
void intro_init(int seed);

#endif
