#pragma once
#define _USE_MATH_DEFINES // for C++  
#include <cmath> 
#include <memory.h>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <time.h>

typedef int(*domain_fun_t)(float, float);
int box_indicator(float x, float y);
int circ_indicator(float x, float y);
int countor_indicator(float x, float y);

class shps {
    //System state
public:
    typedef struct sim_state_t {
        int n; /* Number of particles */
        float max_height;
        int layers_count;     /* Count of history*/
        float size;        /* Particle size */
        float dt;       /* Time step */
        float rho0;     /* Reference density */
        float k_bulk;        /* Bulk modulus */
        float mu;       /* Viscosity */
        float gx;        /* Gravity strength */
        float gy;        /* Gravity strength */

        float mass; /* Particle mass */
        float*  rho; /* Densities */
        float*  cur_lay; /* Positions */
        float*  prev_lay; /* Positions */

        int cur_li;
        int prev_li;


        float t;
        float*  pos; /* Positions */
        float*  vh; /* Velocities (half step) */
        float*  v; /* Velocities (full step) */
        float*  a; /* Acceleration */
        float* k_move;    /*Vx = cos(t*k_move) Vy = sin(t*k_move)*/

    } sim_state_t;

    void alloc_state();
    void free_state();

    //Density computations
    void compute_density();

    //Computing forces
    void compute_accel();

    //Leapfrog integration
    void leapfrog_step(double dt);
    void leapfrog_start(double dt);
    void leapfrog_start_circle(double dt);


    //Reection boundary conditions
    void damp_reflect(int which, float barrier, float* x, float* v, float* vh);
    void reflect_bc();

    //Init
    void place_particles(domain_fun_t indicatef);
    void normalize_mass();
    void init_state();

    //Option processing
    void write_param(std::ofstream &fout);

    //File log
    void write_header(std::ofstream &fout, int n);
    void write_frame_data(std::ofstream &fout, int n, float* x, int* c, int frame);

    //Work flow
    void check_state();
    void init();
    void init_circle();

    void update_cur_layer();
    void update_all_layers();
    void update_p_s_layer(int layer);
    void move_tentakles_circle_pos();  /* Change Pos_x Pos_y 0 layer, without inc cur_pos iter and without change v,v */
    void move_tentakles_circle_v();  /* Change Vx Vy 0 layer, without inc cur_pos and without change x,y */

    float get_x(int pos, int layer);
    float get_y(int pos, int layer);

    void inc_cur_li();

public:
    sim_state_t *state;
};
