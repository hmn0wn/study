#pragma once

#include "smyth_particle_hydro_system.h"




void shps::alloc_state() {
    sim_state_t* s = new sim_state_t;
    s->n = 10;
    s->max_height = 0.1f;
    s->dt = 1e-6;
    s->layers_count = 1;

    s->size = 8e-1;
    s->rho0 = 100;
    s->k_bulk = 1e3;
    s->mu = 0.1;
    s->gx = 10.5f;
    s->gy = 10.5f;

   
    
    s->rho = new float[s->n]; // rho is density and has one entry per particle
    s->pos = new float[2*s->n * s->layers_count];
    s->cur_lay = s->pos;
    s->cur_li = 0;
    s->prev_li = 0;
    s->t = 0.f;
    s->prev_lay = s->pos;
    s->vh = new float[2 * s->n];
    s->v = new float[2 * s->n];
    s->a = new float[2 * s->n];
    s->k_move = new float[s->n];

    state = s;
}
void shps::free_state()
{
    sim_state_t* s = state;
    delete[] s->rho;
    delete[] s->pos;
    delete[] s->vh;
    delete[] s->v;
    delete[] s->a;
    delete[] s->k_move;
}



//Density computations
void shps::compute_density()
{
    sim_state_t* s = state;

    int n = s->n;
    float*  rho = s->rho;
    //const float*  x = s->prev_pos;
    const float*  x = s->pos + 2 * s->n * s->prev_li;

    float h = s->size;
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
void shps::compute_accel()
{
    // Unpack basic parameters
    sim_state_t* s = state;

    const float h = s->size;
    const float rho0 = s->rho0;
    const float k = s->k_bulk;
    const float mu = s->mu;
    const float gx = s->gx;
    const float gy = s->gy;

    const float mass = s->mass;
    const float h2 = h*h;
    // Unpack system state
    const float*  rho = s->rho;
    //const float*  x = state->prev_pos;
    const float*  x = s->pos + 2 * s->n * s->prev_li;


    const float*  v = s->v;
    float*  a = s->a;
    int n = s->n;
    // Compute density and color
    compute_density();
    // Start with gravity and surface forces

    for (int i = 0; i < n; ++i) {
        a[2 * i + 0] = gx * (std::rand() > RAND_MAX / 2 ? 1.f : 1.f);
        a[2 * i + 1] = gy * (std::rand() > RAND_MAX / 2 ? 1.f : 1.f);
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
void shps::leapfrog_step(double dt)
{
    sim_state_t* s = state;
    const float*  a = s->a;
    float*  vh = s->vh;
    float*  v = s->v;
    

    int n = s->n;
    for (int i = 0; i < 2 * n; ++i) vh[i] += a[i] * dt;
    for (int i = 0; i < 2 * n; ++i) v[i] = vh[i] + a[i] * dt / 2;
    //for (int i = 0; i < 2 * n; ++i) s->cur_pos[i] = s->prev_pos[i] + vh[i] * dt;
    for (int i = 0; i < 2 * n; ++i) (s->pos + 2 * s->n * s->cur_li)[i] = (s->pos + 2 * s->n * s->prev_li)[i] + vh[i] * dt;

    reflect_bc();
    s->t += s->dt;
}

void shps::leapfrog_start(double dt)
{
    sim_state_t* s = state;

    const float*  a = s->a;
    float*  vh = s->vh;
    float*  v = s->v;
    int n = s->n;
    for (int i = 0; i < 2 * n; ++i) vh[i] = v[i] + a[i] * dt / 2;
    for (int i = 0; i < 2 * n; ++i) v[i] += a[i] * dt;
    //for (int i = 0; i < 2 * n; ++i) s->cur_pos[i] = s->prev_pos[i] + vh[i] * dt;
    for (int i = 0; i < 2 * n; ++i) (s->pos + 2 * s->n * s->cur_li)[i] = (s->pos + 2 * s->n * s->prev_li)[i] + vh[i] * dt;

    reflect_bc();
}

void shps::leapfrog_start_circle(double dt)
{
    sim_state_t* s = state;

    const float*  a = s->a;
    float*  vh = s->vh;
    float*  v = s->v;
    int n = s->n;
    for (int i = 0; i < 2 * n; ++i) vh[i] = v[i] + a[i] * dt / 2;
    for (int i = 0; i < 2 * n; ++i) v[i] += a[i] * dt;
    //for (int i = 0; i < 2 * n; ++i) s->cur_pos[i] = s->prev_pos[i] + vh[i] * dt;
    for (int i = 0; i < 2 * n; ++i) (s->pos + 2 * s->n * s->cur_li)[i] = (s->pos + 2 * s->n * s->prev_li)[i] + vh[i] * dt;

    reflect_bc();
}

//Reection boundary conditions
void shps::damp_reflect(int which, float barrier,
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

void shps::reflect_bc()
{

    sim_state_t* s = state;

    // Boundaries of the computational domain
    const float XMIN = 0.0;
    const float XMAX = 1.0;
    const float YMIN = 0.0;
    const float YMAX = 1.0;
    float*  vh = s->vh;
    float*  v = s->v;
    //float*  x = s->cur_pos;
    float*  x = (s->pos + 2 * s->n * s->cur_li);

    int n = s->n;
    for (int i = 0; i < n; ++i, x += 2, v += 2, vh += 2) {
        if (x[0] < XMIN) damp_reflect(0, XMIN, x, v, vh);
        if (x[0] > XMAX) damp_reflect(0, XMAX, x, v, vh);
        if (x[1] < YMIN) damp_reflect(1, YMIN, x, v, vh);
        if (x[1] > YMAX) damp_reflect(1, YMAX, x, v, vh);
    }
}

//Init
int box_indicator(float x, float y)
{
    return (x < 1.f) && (y < 1.f) && (x > 0.f) && (y > 0.f);
}
int circ_indicator(float x, float y)
{
    float dx = (x - 0.5);
    float dy = (y - 0.3);
    float r2 = dx*dx + dy*dy;
    return (r2 < 0.25*0.25);
}
int countor_indicator(float x, float y)
{
    float dx = (x - 0.5f);
    float dy = (y - 0.5f);
    float r2 = dx*dx + dy*dy;
    return (abs(r2 - 0.8f*0.8f) < 0.01f);
}

void get_circle_coord(float &x, float &y, float &r)
{
    r = std::rand() / (float)RAND_MAX * 0.5f + 0.02f;
    x = std::rand() / (float)RAND_MAX * (2*r) + (1.f - 2*r)/2.f;
    y = sqrtf(r*r - (x - 0.5f)*(x - 0.5f)) + 0.5f;

    y -= (0.4f ? (std::rand() / (float)RAND_MAX < 0.5f) : 0.f);
}

void shps::place_particles(domain_fun_t indicatef)
{
    /*float h = s->size;
    float hh = h / 1.3;
    // Count mesh points that fall in indicated region.
    int count = 0;

    for (float x = 0; x < 1; x += hh)
        for (float y = 0; y < 1; y += hh)
            count += indicatef(x, y);
    // Populate the particle data structure*/

    alloc_state();
    sim_state_t* s = state;

    int count = 0;
    float r_min = 0.3f;
    float r_max = 0.4f;
    float r_step = 0.05f;
    float r = r_min;

    while(count < s->n)
    {
        //float x = std::rand() / (float)RAND_MAX;
        //float y = std::rand() / (float)RAND_MAX;

        float x = 0.f, y = 0.f, r = 0.f;
        get_circle_coord(x, y, r);
        /*r += r_step;
        if (r > r_max)
        {
            r = r_min;
        }*/
        //if (indicatef(x, y)) 
        {
            //s->cur_pos[2 * p + 0] = x;
            //s->cur_pos[2 * p + 1] = y;
            (s->pos + 2 * s->n * s->cur_li)[2 * count + 0] = x;
            (s->pos + 2 * s->n * s->cur_li)[2 * count + 1] = y;

            s->v[2 * count + 0] = 0;
            s->v[2 * count + 1] = 0;
            s->a[2 * count + 0] = 0;
            s->a[2 * count + 1] = 0;

            s->k_move[count] = (std::rand() / (float)RAND_MAX * 2.f + 1.f)*200.f;

            ++count;

        }
        
    }
}

void shps::normalize_mass()
{
    sim_state_t* s = state;
    s->mass = 1;
    compute_density();
    float rho0 = s->rho0;
    float rho2s = 0;
    float rhos = 0;
    for (int i = 0; i < s->n; ++i) {
        rho2s += (s->rho[i])*(s->rho[i]);
        rhos += s->rho[i];
    }
    s->mass *= (rho0*rhos / rho2s);
}

void shps::init_state()
{
    
    //place_particles(box_indicator);
    place_particles(countor_indicator);
    
    //sim_state_t* s = place_particles(param, box_indicator);
    normalize_mass();
}

void shps::write_param(std::ofstream &fout)
{
    sim_state_t* s = state;

    fout << "time step : " << s->dt << std::endl;
    fout << "particle size : " << s->size << std::endl;
    fout << "reference density : " << s->rho0 << std::endl;
    fout << "bulk modulus : " << s->k_bulk << std::endl;
    fout << "dynamic viscosity : " << s->mu << std::endl;
    fout << "gravitational strength : " << s->gx << std::endl;
    fout << "____________________________________________________________________" << std::endl;
    fout << "____________________________________________________________________" << std::endl << std::endl;
}

//File log
void shps::write_header(std::ofstream &fout, int n)
{
    fout << "n = " << n << std::endl;
    fout << "____________________________________________________________________" << std::endl;
    fout << "____________________________________________________________________" << std::endl << std::endl;
}


void shps::write_frame_data(std::ofstream &fout, int n, float* x, int* c, int frame)
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


void shps::check_state()
{
    sim_state_t* s = state;

    for (int i = 0; i < s->n; ++i) {
        //float xi = s->cur_pos[2 * i + 0];
        //float yi = s->cur_pos[2 * i + 1];

        float xi = (s->pos + 2 * s->n * s->cur_li)[2 * i + 0];
        float yi = (s->pos + 2 * s->n * s->cur_li)[2 * i + 1];
        

        assert(xi >= 0 || xi <= 1);
        assert(yi >= 0 || yi <= 1);
    }
}

void shps::init()
{
    srand(time(NULL));

    init_state();

    sim_state_t* s = state;

    float* start = s->cur_lay;
    compute_accel();
    leapfrog_start_circle(s->dt);
    check_state();

    for (int i = 0; i < s->layers_count; ++i)
    {
        update_cur_layer();
    }
    float* end = s->cur_lay;

}

void shps::init_circle()
{
    init_state();

    sim_state_t* s = state;

    float* start = s->cur_lay;
    compute_accel();
    leapfrog_start(s->dt);
    check_state();

    for (int i = 0; i < s->layers_count; ++i)
    {
        update_cur_layer();
    }
    float* end = s->cur_lay;

}

void shps::update_cur_layer()
{
    sim_state_t* s = state;

    compute_accel();
    leapfrog_step(s->dt);
    check_state();
    //s->prev_pos = s->cur_pos;
    //s->cur_pos += s->n *2;

    /*if (s->cur_pos == (s->pos + s->n * (s->hcount)* 2))
    {
        s->cur_pos = s->pos;
    }*/

    inc_cur_li();

    
}

void shps::update_p_s_layer(int layer)
{
    sim_state_t* s = state;
    int cur_layer = s->cur_li;
    int prev_layer = s->prev_li;

    s->cur_li = layer;
    s->prev_li = (layer - 1 + s->layers_count) % s->layers_count;

    compute_accel();
    leapfrog_step(s->dt);
    check_state();

    //s->prev_pos = s->cur_pos;
    //s->cur_pos += s->n * 2;

    /*if (s->cur_pos == (s->pos + s->n * (s->hcount) * 2))
    {
        s->cur_pos = s->pos;
    }*/

    inc_cur_li();
    s->cur_li = cur_layer;
    s->prev_li = prev_layer;
 }

void shps::update_all_layers()
{
    sim_state_t* s = state;
    //move_tentakles_circle_v();
    //inc_cur_li();
    for (int i = 0; i < s->layers_count; ++i)
    {
        update_p_s_layer(i);
    }
}

void shps::move_tentakles_circle_pos()
{
    sim_state_t* s = state;

    //float*  x = state->cur_pos;
    float*  x = s->pos;
    float   dt = s->dt;
    
    float*  v = s->v;
    float*  a = s->a;
    float*  k = s->k_move;

    int n = s->n;

    for (int i = 0; i < n; ++i) {
        /*float v_x_prev = v[2 * i + 0];
        float v_y_prev = v[2 * i + 1];

        v[2 * i + 0] = v_x_prev * cos(dt*k[i]) - v_y_prev * sin(dt * k[i]);
        v[2 * i + 1] = v_y_prev * cos(dt*k[i]) + v_x_prev * sin(dt * k[i]);

        x[2 * i + 0] += v[2 * i + 0] * dt*100.f;
        x[2 * i + 1] += v[2 * i + 1] * dt*100.f;*/

        float x_prev = x[2 * i + 0] - 0.5f;
        float y_prev = x[2 * i + 1] - 0.5f;

        x[2 * i + 0] = x_prev * cos(dt*k[i]) - y_prev * sin(dt * k[i]) + 0.5f;
        x[2 * i + 1] = y_prev * cos(dt*k[i]) + x_prev * sin(dt * k[i]) + 0.5f;

    }

    //state->prev_pos = state->cur_pos;
    //state->cur_pos += state->n * 2;
        
}

void shps::move_tentakles_circle_v()
{
    sim_state_t* s = state;

    //float*  x = state->cur_pos;
    float*  x = s->pos;
    float   dt = s->dt;
    float   t = s->t;
    float*  v = s->v;
    float*  a = s->a;
    float*  k = s->k_move;

    int n = s->n;

    for (int i = 0; i < n; ++i) {
        float v_x_prev = v[2 * i + 0];
        float v_y_prev = v[2 * i + 1];

        v[2 * i + 0] = cos(t * k[i]);
        v[2 * i + 1] = sin(t * k[i]);

        x[2 * i + 0] += v[2 * i + 0] * dt*100.f;
        x[2 * i + 1] += v[2 * i + 1] * dt*100.f;

                
    }

    //state->prev_pos = state->cur_pos;
    //state->cur_pos += state->n * 2;
}



float shps::get_x(int pos, int layer)
{
    sim_state_t* s = state;

    int offset = (s->prev_lay - s->pos);

    int len = s->n * s->layers_count*2;
    int ij = (s->n * layer*2 + 2 * pos);
    if (ij + offset >= len)
    {
        ij -= len;
    }

    //return state->prev_pos[ij + 0];
    return (s->pos + 2 * s->n * layer + 2 * pos)[0];
    
}

float shps::get_y(int pos, int layer)
{
    sim_state_t* s = state;

    int offset = s->prev_lay - s->pos;
    int len = s->n * s->layers_count*2;
    int ij = (s->n * layer*2 + 2 * pos);
    if (ij + offset >= len)
    {
        ij -= len;
    }
    //return state->prev_pos[ij + 1];
    return (s->pos + 2 * s->n * layer + 2 * pos)[1];


}

void shps::inc_cur_li()
{
    sim_state_t* s = state;

    //state->prev_pos = state->cur_pos;
    //state->cur_pos += state->n * 2;
    s->prev_li = s->cur_li;
    s->cur_li = (s->cur_li + 1) % s->layers_count;
}