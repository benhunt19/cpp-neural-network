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

// 2. softMax (exponential probability)
template <typename N>
float softMax(vector<N>& vec, int& index) {
	float numerator = exp(vec[index].tmp_value);
	float denominator = 0;
	for (int i = 0; i < vec.size(); i++) {
		denominator += exp(vec[i].tmp_value);
	}
	return numerator / denominator;
}