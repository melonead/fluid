#pragma once
#include <cmath>
#include <Settings.h>
#include <vector>


// print: print function
void print(std::string message)
{
	std::cout << message << std::endl;
}

// getDistance: get distance between two locations
double getDistance(double pos1[2], double pos2[2])
{	
	double x_dist = (pos2[0] - pos1[0]);
	double y_dist = (pos2[1] - pos1[1]);

	return sqrt(x_dist * x_dist + y_dist * y_dist);
}

// poly6a: poly6 kernel
double poly6(double dist)
{	
	if (dist < IRADIUS)
		return pow((IRADIUS * IRADIUS - dist * dist), 3);
	return 0.0f;
}

// _max: return max of two value
double _min(double a, double b)
{
	return a < b ? a : b;
}

// getPressure: return the pressure to be applied to a particle
double getPressure(double d)
{
	return _min(PRESSUREMULTIPLIER * (d - IDEALDENS), 0);
}

// computeDensity: computes the density of a particle
void computeDensity(Particle &p, std::vector<Particle> nbs, int N)
{
	p.density = 0.0;
	double dist = 0.0;
	for (int i = 0; i < N; i++)
	{
		//std::cout << nbs[i].pos[0] << std::endl;
		dist = getDistance(p.pos, nbs[i].pos);
		if (dist < IRADIUS)
		{
			p.density += nbs[i].mass * poly6(dist) * poly6Const;
		}
	}
	p.pressure = getPressure(p.density);
}


// avPressure: return the average pressure between 
// two particles
double avPressure(Particle &a, Particle &b)
{
	return ((a.pressure + b.pressure) / (2.0));
}

// getPressureGradient: the pressure gradient kernel
double getPressureGradient(double dist)
{
	if (dist < IRADIUS)
		return spikyGrad * (pow((IRADIUS - dist), 2.0));
	return 0.0;
}

// getViscGradient: the viscosity gradient kernel
double getViscGradient(double dist)
{
	if (dist < IRADIUS)
		return spikyLap * (IRADIUS - dist);
	return 0.0;
}

// computeForces: compute forces acting on a particle

void computeForces(Particle &p, std::vector<Particle> &nbs, int N)
{	
	p.totalForce[0] = 0.0;
	p.totalForce[1] = 0.0;

	double pr[2] = { 0.0, 0.0 };
	double vis[2] = { 0.0, 0.0 };
	double dist;
	double dir[2];
	double vec[2] = { 0.0, 0.0 };
	for (int i = 0; i < N; i++)
	{
		Particle& nb = nbs.at(i);


		if (nb.id == p.id)
			continue;
		dist = getDistance(p.pos, nb.pos);
		vec[0] = nb.pos[0] - p.pos[0]; 
		vec[1] = nb.pos[1] - p.pos[1];

		dir[0] = vec[0] / dist;
		dir[1] = vec[1] / dist;


		if (nb.density <= 0.0)
			nb.density = 0.001;

		pr[0] += (nb.mass / nb.density) * avPressure(p, nb) * getPressureGradient(dist) * dir[0];
		pr[1] += (nb.mass / nb.density) * avPressure(p, nb) * getPressureGradient(dist) * dir[1];


		vis[0] += (nb.velocity[0] - p.velocity[0]) * (nb.mass / nb.density) * getViscGradient(dist) * dir[0];
		vis[1] += (nb.velocity[1] - p.velocity[1]) * (nb.mass / nb.density) * getViscGradient(dist) * dir[1];

	}

	p.totalForce[0] += pr[0] * -1.0;
	p.totalForce[1] += pr[1] * -1.0;

	p.totalForce[0] += vis[0] * VISCMULTIPLIER;
	p.totalForce[1] += vis[1] * VISCMULTIPLIER;

}



void LeapFrogIntegration(Particle& p, double deltaTime, double gravityAccel)
{
	
	double ax = p.acceleration[0] + ((p.totalForce[0] / p.density) * deltaTime);
	p.velocity[0] += ((p.acceleration[0] + ax) * deltaTime) / 2.0f;
	p.pos[0] += (p.velocity[0] * deltaTime + (p.acceleration[0] * pow(deltaTime, 2)) / 2.0);
	p.predictedPosition[0] = p.pos[0] + p.velocity[0] * deltaTime;

	double eps = 0.1;
	if ((p.pos[0] - eps) < X_MIN_BOUND)
	{
		p.velocity[0] *= DAMP;
		p.pos[0] = X_MIN_BOUND + eps;

	}
	if ((p.pos[0] + eps) > X_MAX_BOUND) {
		p.velocity[0] *= DAMP;
		p.pos[0] = X_MAX_BOUND - eps;
	}

	double ay = p.acceleration[1] + ((p.totalForce[1] / p.density) * deltaTime);
	p.velocity[1] += ((p.acceleration[1] + ay) * deltaTime) / 2.0f;
	p.velocity[1] += gravityAccel * deltaTime;
	p.pos[1] += (p.velocity[1] * deltaTime + (p.acceleration[1] * pow(deltaTime, 2.0)) / 2.0);
	p.predictedPosition[1] = p.pos[1] + p.velocity[1] * deltaTime;

	if ((p.pos[1] - eps) < Y_MIN_BOUND)
	{
		p.velocity[1] *= DAMP;
		p.pos[1] = Y_MIN_BOUND + eps;

	}
	if ((p.pos[1] + eps) > Y_MAX_BOUND) {
		p.velocity[1] *= DAMP;
		p.pos[1] = Y_MAX_BOUND - eps;
	}
}

void VerletIntergration(Particle& p, double deltaTime, double gravityAccel)
{

	p.acceleration[0] = (p.totalForce[0] / p.density) * deltaTime;
	p.acceleration[1] = ((p.totalForce[1] / p.density) + gravityAccel) * deltaTime;

	double eps = 0.08;

	p.pos[0] = p.pos[0] + (p.pos[0] - p.prevPos[0]) + p.acceleration[0] * deltaTime * deltaTime;
	p.prevPos[0] = p.pos[0];
	if (p.pos[0] < X_MIN_BOUND)
	{
		p.pos[0] = X_MIN_BOUND + eps;

	}
	if (p.pos[0] > X_MAX_BOUND) {
		p.pos[0] = X_MAX_BOUND - eps;
	}


	p.pos[1] = p.pos[1] + (p.pos[1] - p.prevPos[1]) + p.acceleration[1] * deltaTime * deltaTime;
	p.prevPos[1] = p.pos[1];
	if (p.pos[1] < Y_MIN_BOUND)
	{
		p.pos[1] = Y_MIN_BOUND + eps;
	}
	if (p.pos[1] > Y_MAX_BOUND) {
		p.pos[1] = Y_MAX_BOUND - eps;
	}
}



