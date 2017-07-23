/* steering algorithms and functions*/

#define CHF 0.95

#define DEFAULT_RADIUS 0.05
#define DEFAULT_DODGE_RADIUS (DEFAULT_RADIUS*2.0)
#define DEFAULT_SPD 0.5
#define DEFAULT_DESIRE_F 0.5

/* steeripalikka */

typedef struct hiiri_t {
    int id;

    float pos[3];
    float dir; // 0..360 astetta
    float dir2; // kakkossuunta, aliohjelmat käyttävät
    float dir3; // tätä käyttää seinien väistely
    float rejection_strength; // hylkyvoimakkuus

    float rejection[3]; // hylkyvektori
    float desire[3]; // haluvektori
    float current_desire[3];
    float desire_f;

    float vakkyra_kulma;

    float spd; // hiiren nopeus
    float radius;
    float dodge_radius;
    float counter; // laskuri aliohjelmien käyttöön

    int action;

    float old_spd;
    float old_dir;
    float spd_factor;

    float die_counter;

    int first_time_no_border_dodge;

    struct hiiri_t * follow_hiiri;

    int color;

    int move_state; // hiiren tila
    int dodge_walls; // seinäväistelymoodi
    int dodge_others; // hiirtenväistelymoodi

    struct hiiri_t * next;
} HIIRI;

/* maailman artefakti */
typedef struct seina_t {
    float pos[3];
    float size[3];
    struct seina_t * next;
} SEINA;

/* pallojen tyyppejä */
enum {
    RANTAPALLO,
    JALKAPALLO,
    TENNARI,
    KEILAPALLO
};

/* pallo */
typedef struct pallo_t {
    int id;

    float r;
    float pos0[3];
    float posn[3];
    float pos[3];
    float pos1[3];
    float pos2[3];
    float opos[3];
    //    float new_pos[3];
    float rejection[3];

    float dir;
    float vierin;
    int yi;
    int yn;

    float pomppu_h;
    float p_counter;
    float p_counter_raja;

    int mode;
    float die_counter;

    float spd;
    float counter;
    float counter_raja;
    int tyyppi;
    struct pallo_t * next;
} PALLO;

typedef struct maailma_t {
    int is_borders;
    HIIRI * hiiret;
    SEINA * seinat;
    PALLO * pallot;
    float gcounter;
    float gz;

    float checker[3];
    float new_checker[3];
    float goal[3];
    float clip[3];
} MAAILMA;


/* toimintamoodit */
enum  {
    MOVE,
    STOP,
    STOP_AND_DIE,
    DIE
};

/* värit */
enum {
    COLOR_BLUE=0,
    COLOR_YELLOW=1,
    COLOR_GREEN
};

/* eri liiketilat */
enum {
    MOVE_STOP=0,
    MOVE_HARHAILE=1,
    MOVE_TOWARDS_GOAL,
    MOVE_TOWARDS_ANOTHER,
    MOVE_STRAIGHT,
};

/* eri väistelymoodit */
enum {
    DODGE_NONE=0, // ei mitään dodgea, objektit menevät toistensa läpi
    DODGE_CLIP, // lähinnä seinillä, törmäyksessä heitetään toiseen laitaan
    DODGE_STOP, // pysähdy esteeseen
    DODGE_BUMP,
};


HIIRI * add_harhaileva_hiiri(MAAILMA * m, float x, float y, float z, float dir);
HIIRI * add_goal_hiiri(MAAILMA * m, float x, float y, float z, float dir);
HIIRI * add_following_hiiri(MAAILMA * m, float x, float y, float z, float dir, HIIRI * hf);
void init_maailma(MAAILMA * m, float clipx, float clipy, float clipz);
void kill_all_hiiret(MAAILMA * m);
void kill_all_pallot(MAAILMA * m);
void kill_oldest_hiiri_alive(MAAILMA * m);
void add_pallo(MAAILMA * m, int tyyppi,int n);
HIIRI * last_hiiri(MAAILMA * m);
void update_maailma(MAAILMA * m, float dt);

extern float global_desire_f;
extern float new_global_desire_f;


