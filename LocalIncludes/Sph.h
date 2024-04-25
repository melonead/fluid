#pragma once
#include <cmath>
#include <Settings.h>
#include <vector>
#include <Hashtable.h>
#include <algorithm>


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
	return pow((IRADIUS * IRADIUS - dist * dist), 3);
}

// _max: return max of two value
double _min(double a, double b)
{
	return a > b ? a : b;
}

// getPressure: return the pressure to be applied to a particle
double getPressure(double d)
{
	return PRESSUREMULTIPLIER * (d - IDEALDENS);
}

// avPressure: return the average pressure between 
// two particles
double avPressure(Particle& a, Particle& b)
{
	return ((a.pressure + b.pressure) / (2.0));
}

// getPressureGradient: the pressure gradient kernel
double getPressureGradient(double dist)
{
	return spikyGrad * (pow((IRADIUS - dist), 2.0));
}

// getViscGradient: the viscosity gradient kernel
double getViscGradient(double dist)
{
	return spikyLap * (IRADIUS - dist);
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

// computeDensities: compute densities for all the particles
void computeDensities(Particle particles[], std::vector<Particle>(&Table)[NUMCELLS], std::vector<Particle> neighbors, int N)
{
	neighbors.clear();
	for (int i = 0; i < N; i++)
	{
		Particle& p = particles[i];
		p.density = 0.0;
		double dist;
		getNeighbors(p.pos, neighbors, Table);
		for (int j = 0; j < neighbors.size(); j++)
		{	
			Particle& nb = neighbors.at(j);
			dist = getDistance(p.pos, nb.pos);
			p.density += nb.mass * poly6(dist) * poly6Const;
		}
		//std::cout << "dens = " << p.density << std::endl;
		neighbors.clear();
		p.pressure = getPressure(p.density);
	}
}

// computePressure: pressure acting between two particles
double computePressure(Particle& p, Particle &nb, double dist)
{
	if (nb.density == 0.0)
		nb.density = 0.001;
	return (nb.mass / nb.density) * avPressure(p, nb) * getPressureGradient(dist);
}

// computeViscosity: calculate viscous force between two particles
double computeViscosity(Particle& p, Particle& nb, double dist)
{
	if (nb.density == 0.0)
		nb.density = 0.001;
	return (nb.mass / nb.density) * getViscGradient(dist);
}

void interactWithMouse(Particle& p, glm::vec4 mousePos, glm::vec2 wPos, bool mouseClick[])
{
	// get distance from mouse position
	double xDist = mousePos.x - p.pos[0];
	double yDist = mousePos.y - p.pos[1];
	double dist = sqrt(xDist * xDist + yDist * yDist);
	double dir[] = { xDist / dist, yDist / dist };
	double speed = 0.55;

	// get the neighborhood particles


	//std::cout << dist << std::endl;
	double infDist = 2.0;
	// repel 
	if (dist < infDist && mouseClick[0])
	{
		p.velocity[0] += (-dir[0] * speed);
		p.velocity[1] += (-dir[1] * speed);
	}

	// attract
	if (dist < infDist && mouseClick[1])
	{
		p.velocity[0] += (dir[0] * speed);
		p.velocity[1] += (dir[1] * speed);
	}
}

// simulateParticle: complete run on a single particle's simulation
void simulateParticle(Particle& p, std::vector<Particle> &nbs, int N)
{
	double pressureForce[2] = {0.0, 0.0};
	double viscForce[2] = {0.0, 0.0};
	double dirVec[2] = { 0.0, 0.0 };
	for (int i = 0; i < N; i++)
	{
		// get distance between two particles
		Particle& nb = nbs.at(i);
		if (nb.id == p.id)
			continue;
		double dist = getDistance(p.pos, nb.pos);
		dirVec[0] = (nb.pos[0] - p.pos[0]) / dist;
		dirVec[1] = (nb.pos[1] - p.pos[1]) / dist;
		// compute pressure
		double pInf = computePressure(p, nb, dist);
		pressureForce[0] += pInf * dirVec[0];
		pressureForce[1] += pInf * dirVec[1];
		// compute viscosity
		double vInf = computeViscosity(p, nb, dist);
		viscForce[0] += (nb.velocity[0] - p.velocity[0]) * vInf * dirVec[0];
		viscForce[1] += (nb.velocity[1] - p.velocity[1]) * vInf * dirVec[1];
	}

	p.totalForce[0] = -pressureForce[0] + viscForce[0] * VISCMULTIPLIER;
	p.totalForce[1] = -pressureForce[1] + viscForce[1] * VISCMULTIPLIER;

}

// LeapFrogIntegration: integrate particle's position and velocity
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

// simulateFluid: complete simulation run on all the particles
void simulateFluid(Particle particles[], std::vector<Particle>(&Table)[NUMCELLS], 
	int N, double deltaTime, double gravity, std::vector<Particle> neighbors, 
	glm::vec2 translations[], glm::vec4 mousePos, bool mouseClick[])
{	

	for (int i = 0; i < N; i++)
	{
		glm::vec2 trans;
		Particle& p = particles[i];
		trans.x = p.pos[0];
		trans.y = p.pos[1];
		translations[i] = trans;
		getNeighbors(p.pos, neighbors, Table);
		simulateParticle(p, neighbors, neighbors.size());
		LeapFrogIntegration(p, deltaTime, gravity);
		//interactWithMouse(p, mousePos, mouseClick);
		neighbors.clear();
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



