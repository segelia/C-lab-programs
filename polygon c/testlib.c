#include "primlib.h"
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#define CENTER_X gfx_screenWidth()/2
#define CENTER_Y gfx_screenHeight()/2
#define NUMBER_OF_SIDES 5
#define ANGLE_OF_CHANGE (2*M_PI/NUMBER_OF_SIDES)
#define SPEED_OF_ROTATION (2*M_PI/180)

int main(int argc, char* argv[])
{
    if (gfx_init()) {
        exit(3);
    }
    int initial_radius = 100;
    double x_coordinates[NUMBER_OF_SIDES];
    double y_coordinates[NUMBER_OF_SIDES];
    double angle = 0;
    int increasing = true;

    while (1) {
        gfx_filledRect(0,0,gfx_screenWidth()-1,gfx_screenHeight()-1,BLACK);

        for (int iterate_fill_coord=0; iterate_fill_coord<NUMBER_OF_SIDES; iterate_fill_coord++) {
            x_coordinates[iterate_fill_coord]= CENTER_X + initial_radius*cos(angle+ANGLE_OF_CHANGE*iterate_fill_coord);
            y_coordinates[iterate_fill_coord] = CENTER_Y+initial_radius*sin(angle+ANGLE_OF_CHANGE*iterate_fill_coord);
        }

        for (int iterate_draw=0; iterate_draw<NUMBER_OF_SIDES; iterate_draw++) {
            int next_index = (iterate_draw + 1) % NUMBER_OF_SIDES;
            gfx_line(x_coordinates[iterate_draw],y_coordinates[iterate_draw],x_coordinates[next_index],y_coordinates[next_index],GREEN);
        }

        if(increasing)
        {
            initial_radius += 2;
        }
        else
        {
            initial_radius -= 2;
        }

        while(initial_radius > gfx_screenHeight()/2-20)
        {
            increasing = false;
            initial_radius -= 2;
        }

        while(initial_radius < 20)
        {
            increasing = true;
            initial_radius += 2;
        }

        gfx_pollkey();
        gfx_updateScreen();
        SDL_Delay(10);

        angle += SPEED_OF_ROTATION;

        while(angle>2*M_PI)
        {
            angle -= 2*M_PI;
        }
    }

    return 0;
}
