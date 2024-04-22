#ifndef SETTINGS_H
#define SETTINGS_H

#include <cmath>
const int NUMPARTICLES = 1225;


const double pi = 3.142857;
const double IRADIUS = 0.45; // 0.35f was good but unstable, try it later
const double IDEALDENS = 997.0;
const double VISCMULTIPLIER = 0.001;

//const double gravityAccel = 0.0;//-9.8;
const double PRESSUREMULTIPLIER = 1000.0;
const double poly6Const = 315.0f / (64.0f * pi * pow(IRADIUS, 9.0));
const double spikyGrad = -45.0f / (pi * pow(IRADIUS, 6.0));
const double spikyLap = 45.0f / (pi * pow(IRADIUS, 6.0));

//const static double poly6Const = 4.f / (pi * pow(IRADIUS, 8.f));
//const static double spikyGrad = -10.f / (pi * pow(IRADIUS, 5.f));
//const static double spikyLap = 40.f / (pi * pow(IRADIUS, 5.f));

// velocity damp
const double DAMP = -0.10;


// screen bounds for the
const double xbound = 20.0;
const double ybound = 10.0;
const double X_MIN_BOUND = xbound * -1.0;
const double X_MAX_BOUND = xbound;
const double Y_MIN_BOUND = ybound * -1.0;
const double Y_MAX_BOUND = ybound;

const int NUMCELLS = (xbound / IRADIUS) * (ybound / IRADIUS);

const int  yRange = (Y_MAX_BOUND - Y_MIN_BOUND) / IRADIUS;
const int  xRange = (X_MAX_BOUND - X_MIN_BOUND) / IRADIUS;

// particle representation
//typedef struct Particle Particle;
struct Particle
{
	/*Particle () {
		std::cout << "particle created" << std::endl;
	}*/

	// Copy constructor
	//Particle()
	//	: id(id), key(key), index(index), prevKey(prevKey), prevIndex(prevIndex),
	//	mass(mass), pressure(pressure), density(density)
	//{
	//	// Copy pos array
	//	std::copy(std::begin(pos), std::end(pos), std::begin(pos));
	//	std::copy(std::begin(prevPos), std::end(prevPos), std::begin(prevPos));
	//	std::copy(std::begin(velocity), std::end(velocity), std::begin(velocity));
	//	std::copy(std::begin(totalForce), std::end(totalForce), std::begin(totalForce));
	//	std::copy(std::begin(acceleration), std::end(acceleration), std::begin(acceleration));
	//	std::copy(std::begin(predictedPosition), std::end(predictedPosition), std::begin(predictedPosition));
	//	std::cout << "copy of particle" << std::endl;
	//}

	int id = NULL;
	int key = NULL;
	int index = NULL;
	int prevKey = NULL;
	int prevIndex = NULL;
	double pos[2] = { 0.0, 0.0 };
	double prevPos[2] = { 0.0, 0.0 };
	double mass = 2.5;
	double pressure = 0.0;
	double density = 0.0;
	double velocity[2] = { 0.0, 0.0 };
	double totalForce[2] = { 0.0, 0.0 };
	double acceleration[2] = { 0.0, 0.0 };
	double predictedPosition[2] = { 0.0, 0.0 };

};

#endif // !SETTINGS_H
