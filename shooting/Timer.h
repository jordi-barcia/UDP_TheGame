#pragma once
#include <time.h>
class Timer
{
public:
    Timer() {}
    ~Timer() {}

    void init(time_t temp_duration) {

        initial_time = time(NULL);
        temp = temp_duration;
        deltaTime = time(NULL);
        lastTime = initial_time;
    }

    void update() {
        //Sacamos la diferencia de tiempo desde la ultima iteracion y la actual
        deltaTime = time(NULL) - lastTime;
        //Actualizamos el tiempo restante
        temp -= deltaTime;
        //Almacenamos el tiempo de la iteracion actual
        lastTime += deltaTime;
    }

    time_t temp;


private:
    time_t deltaTime;
    time_t lastTime;
    time_t initial_time;
};

