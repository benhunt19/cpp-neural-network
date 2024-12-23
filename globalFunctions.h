#pragma once
#include <ctime>
#include <random>
//#include "globalClasses.h"

using namespace std;


// ----- GLOBAL CONSTS ------

const string RElU_const = "RElU";
const string softMax_const = "softMax";
const int maxPixles = 255;



// ----- GLOBAL FUNCTIONS ---- 

// Uniform random numbers usnig Mersenne twistor
float uniformRandom(float min, float max) {
	random_device rd;// Create a random device to seed the random number generator
	mt19937 gen(rd()); // Mersenne Twister generator
	uniform_real_distribution<> dis(min, max);
	return dis(gen);
}


// ACTIVATION FUNCTIONS
// 1. RElU (ramp)
template <typename T>
T RElU(T x) {
	return max(T(0), x);
};

// 1b. RElU prime (heaviside step)
template <typename T>
T heaviside(T x) {
	return T(x) >= 0 ? 1 : 0;
};

// 2. softMax (exponential probability)
template <typename N>
float softMax(vector<N>& vec, int index) {
	float sum = 0;
	for (int i = 0; i < vec.size(); i++) {
		sum += exp(vec[i].z);
	}
	return exp(vec[index].z) / sum;
}