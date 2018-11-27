#pragma once
#define _USE_MATH_DEFINES // for C++  
#include <cmath> 
#include <memory.h>
#include <assert.h>
#include <iostream>
#include <fstream>




//System parameters
typedef struct sim_param_t {
    char* fname;    /* File name */
    int nframes;    /* Number of frames */
    int npframe;    /* Steps per frame */
    float h;        /* Particle size */
    float dt;       /* Time step */
    float rho0;     /* Reference density */
    float k;        /* Bulk modulus */
    float mu;       /* Viscosity */
    float g;        /* Gravity strength */
} sim_param_t;




//System state
typedef struct sim_state_t {
    int n; /* Number of particles */
    float mass; /* Particle mass */
    float* __restrict rho; /* Densities */
    float* __restrict x; /* Positions */
    float* __restrict vh; /* Velocities (half step) */
    float* __restrict v; /* Velocities (full step) */
    float* __restrict a; /* Acceleration */
} sim_state_t;

sim_state_t* alloc_state(int n);

void free_state(sim_state_t* s);



//Density computations
void compute_density(sim_state_t* s, sim_param_t* params);


//Computing forces
void compute_accel(sim_state_t* state, sim_param_t* params);


//Leapfrog integration
void leapfrog_step(sim_state_t* s, double dt);
void leapfrog_start(sim_state_t* s, double dt);


//Reection boundary conditions
static void damp_reflect(int which, float barrier, float* x, float* v, float* vh);
static void reflect_bc(sim_state_t* s);


//Init
typedef int(*domain_fun_t)(float, float);
int box_indicator(float x, float y);
int circ_indicator(float x, float y);

sim_state_t* place_particles(sim_param_t* param, domain_fun_t indicatef);

void normalize_mass(sim_state_t* s, sim_param_t* param);
sim_state_t* init_particles_state(sim_param_t* param);


//Main event


//Option processing
void default_params(sim_param_t* params);
void write_param(std::ofstream &fout, sim_param_t* params);


//File log
void write_header(std::ofstream &fout, int n);
void write_frame_data(std::ofstream &fout, int n, float* x, int* c, int frame);
