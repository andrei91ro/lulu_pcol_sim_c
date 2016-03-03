#include "lulu.h"
#include "instance.h"
#include "debug.h"
#include "stdlib.h" //for rand(), srand()

int main(int argc, char **argv) {
    Pcolony_t pcol;
    uint8_t step_nr = 0;

    srand(8312);

    lulu_init(&pcol);
    while (1) {
        sim_step_result_t result = SIM_STEP_RESULT_FINISHED;

        printi(("Running simulation step %d", step_nr));

        result = pcolony_runSimulationStep(&pcol);
        if (result == SIM_STEP_RESULT_NO_MORE_EXECUTABLES) {
            printi(("Simulation finished sucesfully"));
            return 0;
        }
        else if (result == SIM_STEP_RESULT_ERROR) {
            printe(("Error encountered"));
            return 1;
        }

        step_nr++;
    }

    lulu_destroy(&pcol);

    return 0;
}
