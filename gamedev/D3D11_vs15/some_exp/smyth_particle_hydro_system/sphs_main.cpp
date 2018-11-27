#include "smyth_particle_hydro_system.h"
#include <ctime>
clock_t tic_time;

void tic() {
    tic_time = clock();
}

clock_t toc() {
    return (double)(clock() - tic_time) / CLOCKS_PER_SEC;
}



//Main event
void check_state(sim_state_t* s)
{
    for (int i = 0; i < s->n; ++i) {
        float xi = s->x[2 * i + 0];
        float yi = s->x[2 * i + 1];
        assert(xi >= 0 || xi <= 1);
        assert(yi >= 0 || yi <= 1);
    }
}


int main(int argc, char** argv)
{
    sim_param_t params;
    default_params(&params);

    sim_state_t* state = init_particles_state(&params);
    //FILE* fp = fopen(arams.fname, "w");

    std::ofstream fout(params.fname);

    int nframes = params.nframes;
    int npframe = params.npframe;
    float dt = params.dt;
    int n = state->n;
    tic();
    write_param(fout, &params);
    write_header(fout, n);
    write_frame_data(fout, n, state->x, NULL, 0);

    compute_accel(state, &params);
    leapfrog_start(state, dt);
    check_state(state);
    for (int frame = 1; frame < nframes; ++frame) {
        for (int i = 0; i < npframe; ++i) {
            compute_accel(state, &params);
            leapfrog_step(state, dt);
            check_state(state);
        }
        write_frame_data(fout, n, state->x, NULL, frame);
    }
    std::cout << "Ran in " << toc() << " seconds." << std::endl;
    free_state(state);
}