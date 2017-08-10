#include "algo.hpp"

double my_rand() {

    // obtain a seed from the system clock:
    unsigned now = std::chrono::high_resolution_clock::now().time_since_epoch().count();

    unsigned pi = rand();

    uint_fast64_t seed = now*pi;

    // mt19937 is a standard mersenne_twister_engine
    std::mt19937 generator (seed);
    
    return double(generator()%RAND_MAX);
}

void Vector_Normalization(double *vector, int dim) {
    double norm = 0.0;
    for(int i=0;i<dim;i++) {
        norm += vector[i]*vector[i];
    }
    norm = sqrt(norm);
    double factor = norm/1.0;
    for(int i=0;i<dim;i++) {
        vector[i]/=factor;
    }
    return;
}

void Print_Vector(double *vector, int size) {
    for(int i=0;i<size;i++) {
        std::cout<<vector[i]<<" ";
    }
    std::cout<<std::endl;
    return;
}

void Print_Vector(int *vector, int size) {
    for(int i=0;i<size;i++) {
        std::cout<<vector[i]<<" ";
    }
    std::cout<<std::endl;
    return;
}

void Averaging_Data(double *data, int data_size) {
    //std::cout<<"Data to be adjusted"<<std::endl;
    //Print_Vector(data,data_size);
    double max = data[0];
    double min = data[0];
    for(int i=1;i<data_size;i++) {
        if(data[i]>max) {
            max = data[i];
        }
        if(data[i]<min) {
            min = data[i];
        }
    }
    double gap = max-min;
    for(int i=0;i<data_size;i++) {
        // map to [-1,1]
        data[i] = (2*(data[i]-min))/gap-1;
    }

    // Sometimes we might have broken data, it is neccesary they are not nan
    if(!gap) {
        for(int i=0;i<data_size;i++) {
            data[i] = 0.5;
        }
    }
    // std::cout<<"Data after adjusted"<<std::endl;
    // Print_Vector(data,data_size);
    return;
}