#pragma once

#include "smyth_particle_hydro_system.h"




sim_state_t* alloc_state(int n) {
    sim_state_t* s = new sim_state_t;
    int size = 3 * n;  // I think 2*n not n*n!
    s->n = n;
    s->rho = new float[n]; // rho is density and has one entry per particle
    s->x = new float[size];
    s->vh = new float[size];
    s->v = new float[size];
    s->a = new float[size];
    return s;
}
void free_state(sim_state_t* s)
{
    delete[] s->rho;
    delete[] s->x;
    delete[] s->vh;
    delete[] s->v;
    delete[] s->a;

}



//Density computations
void compute_density(sim_state_t* s, sim_param_t* params)
{
    int n = s->n;
    float* __restrict rho = s->rho;
    const float* __restrict x = s->x;
    float h = params->h;
    float h2 = h*h;
    float h8 = (h2*h2)*(h2*h2);
    float C = 4 * s->mass / M_PI / h8;
    memset(rho, 0, n * sizeof(float));
    for (int i = 0; i < n; ++i) {
        rho[i] += 4 * s->mass / M_PI / h2;
        for (int j = i + 1; j < n; ++j) {
            float dx = x[2 * i + 0] - x[2 * j + 0];
            float dy = x[2 * i + 1] - x[2 * j + 1];
            float r2 = dx*dx + dy*dy;
            float z = h2 - r2;
            if (z > 0) {
                float rho_ij = C*z*z*z;
                rho[i] += rho_ij;
                rho[j] += rho_ij;
            }
        }
    }
}

//Computing forces
void compute_accel(sim_state_t* state, sim_param_t* params)
{
    // Unpack basic parameters
    const float h = params->h;
    const float rho0 = params->rho0;
    const float k = params->k;
    const float mu = params->mu;
    const float g = params->g;
    const float mass = state->mass;
    const float h2 = h*h;
    // Unpack system state
    const float* __restrict rho = state->rho;
    const float* __restrict x = state->x;
    const float* __restrict v = state->v;
    float* __restrict a = state->a;
    int n = state->n;
    // Compute density and color
    compute_density(state, params);
    // Start with gravity and surface forces
    for (int i = 0; i < n; ++i) {
        a[2 * i + 0] = 0;
        a[2 * i + 1] = -g;
    }
    // Constants for interaction term
    float C0 = mass / M_PI / ((h2)*(h2));
    float Cp = 15 * k;
    float Cv = -40 * mu;
    // Now compute interaction forces
    for (int i = 0; i < n; ++i) {
        const float rhoi = rho[i];
        for (int j = i + 1; j < n; ++j) {
            float dx = x[2 * i + 0] - x[2 * j + 0];
            float dy = x[2 * i + 1] - x[2 * j + 1];
            float r2 = dx*dx + dy*dy;
            if (r2 < h2) {
                const float rhoj = rho[j];
                float q = sqrt(r2) / h;
                float u = 1 - q;
                float w0 = C0 * u / rhoi / rhoj;
                float wp = w0 * Cp * (rhoi + rhoj - 2 * rho0) * u / q;
                float wv = w0 * Cv;
                float dvx = v[2 * i + 0] - v[2 * j + 0];
                float dvy = v[2 * i + 1] - v[2 * j + 1];
                a[2 * i + 0] += (wp*dx + wv*dvx);
                a[2 * i + 1] += (wp*dy + wv*dvy);
                a[2 * j + 0] -= (wp*dx + wv*dvx);
                a[2 * j + 1] -= (wp*dy + wv*dvy);
            }
        }
    }
}


//Leapfrog integration
void leapfrog_step(sim_state_t* s, double dt)
{
    const float* __restrict a = s->a;
    float* __restrict vh = s->vh;
    float* __restrict v = s->v;
    float* __restrict x = s->x;
    int n = s->n;
    for (int i = 0; i < 2 * n; ++i) vh[i] += a[i] * dt;
    for (int i = 0; i < 2 * n; ++i) v[i] = vh[i] + a[i] * dt / 2;
    for (int i = 0; i < 2 * n; ++i) x[i] += vh[i] * dt;
    reflect_bc(s);
}

void leapfrog_start(sim_state_t* s, double dt)
{
    const float* __restrict a = s->a;
    float* __restrict vh = s->vh;
    float* __restrict v = s->v;
    float* __restrict x = s->x;
    int n = s->n;
    for (int i = 0; i < 2 * n; ++i) vh[i] = v[i] + a[i] * dt / 2;
    for (int i = 0; i < 2 * n; ++i) v[i] += a[i] * dt;
    for (int i = 0; i < 2 * n; ++i) x[i] += vh[i] * dt;
    reflect_bc(s);
}

//Reection boundary conditions
static void damp_reflect(int which, float barrier,
    float* x, float* v, float* vh)
{
    // Coefficient of resitiution
    const float DAMP = 0.75;
    // Ignore degenerate cases
    if (v[which] == 0)
        return;
    // Scale back the distance traveled based on time from collision
    float tbounce = (x[which] - barrier) / v[which];
    x[0] -= v[0] * (1 - DAMP)*tbounce;
    x[1] -= v[1] * (1 - DAMP)*tbounce;
    // Reflect the position and velocity
    x[which] = 2 * barrier - x[which];
    v[which] = -v[which];
    vh[which] = -vh[which];
    // Damp the velocities
    v[0] *= DAMP; vh[0] *= DAMP;
    v[1] *= DAMP; vh[1] *= DAMP;
}

static void reflect_bc(sim_state_t* s)
{
    // Boundaries of the computational domain
    const float XMIN = 0.0;
    const float XMAX = 1.0;
    const float YMIN = 0.0;
    const float YMAX = 1.0;
    float* __restrict vh = s->vh;
    float* __restrict v = s->v;
    float* __restrict x = s->x;
    int n = s->n;
    for (int i = 0; i < n; ++i, x += 2, v += 2, vh += 2) {
        if (x[0] < XMIN) damp_reflect(0, XMIN, x, v, vh);
        if (x[0] > XMAX) damp_reflect(0, XMAX, x, v, vh);
        if (x[1] < YMIN) damp_reflect(1, YMIN, x, v, vh);
        if (x[1] > YMAX) damp_reflect(1, YMAX, x, v, vh);
    }
}

//Init
typedef int(*domain_fun_t)(float, float);
int box_indicator(float x, float y)
{
    return (x < 0.5) && (y < 0.5);
}
int circ_indicator(float x, float y)
{
    float dx = (x - 0.5);
    float dy = (y - 0.3);
    float r2 = dx*dx + dy*dy;
    return (r2 < 0.25*0.25);
}

sim_state_t* place_particles(sim_param_t* param, domain_fun_t indicatef)
{
    float h = param->h;
    float hh = h / 1.3;
    // Count mesh points that fall in indicated region.
    int count = 0;
    for (float x = 0; x < 1; x += hh)
        for (float y = 0; y < 1; y += hh)
            count += indicatef(x, y);
    // Populate the particle data structure
    sim_state_t* s = alloc_state(count);
    int p = 0;
    for (float x = 0; x < 1; x += hh) {
        for (float y = 0; y < 1; y += hh) {
            if (indicatef(x, y)) {
                s->x[2 * p + 0] = x;
                s->x[2 * p + 1] = y;
                s->v[2 * p + 0] = 0;
                s->v[2 * p + 1] = 0;
                ++p;
            }
        }
    }
    return s;
}



void normalize_mass(sim_state_t* s, sim_param_t* param)
{
    s->mass = 1;
    compute_density(s, param);
    float rho0 = param->rho0;
    float rho2s = 0;
    float rhos = 0;
    for (int i = 0; i < s->n; ++i) {
        rho2s += (s->rho[i])*(s->rho[i]);
        rhos += s->rho[i];
    }
    s->mass *= (rho0*rhos / rho2s);
}
sim_state_t* init_particles_state(sim_param_t* param)
{
    sim_state_t* s = place_particles(param, box_indicator);
    normalize_mass(s, param);
    return s;
}




//Option processing
void default_params(sim_param_t* params)
{
    params->fname = "run.txt";
    params->nframes = 3;
    params->npframe = 2;
    params->dt = 5e-2;
    params->h = 5e-1;
    params->rho0 = 1000;
    params->k = 1e3;
    params->mu = 0.1;
    params->g = 9.8;
}

void write_param(std::ofstream &fout, sim_param_t* params)
{
    fout << "output file name : " << params->fname << std::endl;
    fout << "number of frames : " << params->nframes << std::endl;
    fout << "steps per frame : " << params->npframe << std::endl;
    fout << "time step : " << params->dt << std::endl;
    fout << "particle size : " << params->h << std::endl;
    fout << "reference density : " << params->rho0 << std::endl;
    fout << "bulk modulus : " << params->k << std::endl;
    fout << "dynamic viscosity : " << params->mu << std::endl;
    fout << "gravitational strength : " << params->g << std::endl;
    fout << "____________________________________________________________________" << std::endl;
    fout << "____________________________________________________________________" << std::endl << std::endl;
}



//File log
void write_header(std::ofstream &fout, int n)
{
    fout << "n = " << n << std::endl;
    fout << "____________________________________________________________________" << std::endl;
    fout << "____________________________________________________________________" << std::endl << std::endl;
}


void write_frame_data(std::ofstream &fout, int n, float* x, int* c, int frame)
{
    fout << "frame : " << frame << std::endl;
    fout << "____________________________________________________________________" << std::endl;
    fout << "____________________________________________________________________" << std::endl << std::endl;

    for (int i = 0; i < n; ++i) {
        fout << "\t\txi = " << *(x++) << std::endl;
        fout << "\t\tyi = " << *(x++) << std::endl;
        fout << "\t\tci = " << (c ? *(c++) : 0.0f) << std::endl;
        fout << "____________________________________________________________________" << std::endl << std::endl;


    }
}