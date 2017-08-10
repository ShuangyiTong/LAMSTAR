#include <vector>
#include <iostream>
#include <chrono>
#include <random>
#include <cmath>
#include <fstream>
#include <utility>
#include <algorithm>
#include <cfloat>
#include <thread>
#include "const-string.hpp"

double my_rand();

void Vector_Normalization(double *vector, int dim);

void Averaging_Data(double *data, int data_size);

void Print_Vector(double *vector, int size);

void Print_Vector(int *vector, int size);