/* interfaces from music routines to main program */

// triggers
enum {
    NONE=0,
    TRIG1,
    TRIG4,
    TRIG8,
    TRIG16,
    TRIG32,
    TRIG64,
    TRIG128,
    TRIG256
};

void intro_osa(int stillon);
void rauhallinen(int trig,int eka_rauhallinen);
void actioni(int trig,int ekakerta);
