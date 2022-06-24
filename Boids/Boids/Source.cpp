#include <cstdlib>
#include <ctime>

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#define PI 3.14159265358979

using namespace std;

class Boid
{
public: 
	olc::vf2d position;
	olc::vf2d velocity;
	olc::vf2d acceleration;

	float maxForce = 15;
	float maxVel = 200;

	Boid(int width, int height) 
	{
		this->position.x = (float)rand() / (float)RAND_MAX * (float)width;
		this->position.y = (float)rand() / (float)RAND_MAX * (float)height;
		this->velocity.x = (float)rand() / (float)RAND_MAX * this->maxVel - 0.5 * this->maxVel;
		this->velocity.y = (float)rand() / (float)RAND_MAX * this->maxVel - 0.5 * this->maxVel;
	}

public:
	void steer(Boid *boids[2000])
	{
		float perceptionRadius = 100.0f;
		olc::vf2d align;
		olc::vf2d cohesion;
		olc::vf2d seperation;
		int total = 0;

		for (int i = 0; i < 2000; i++)
		{
			float boidX = boids[i]->position.x;
			float boidY = boids[i]->position.y;
			float d = sqrtf((this->position.x - boidX) * (this->position.x - boidX) + (this->position.y - boidY) * (this->position.y - boidY));
			
			if (boids[i] != this && d < perceptionRadius)
			{
				// Increment the align force
				align.x += boids[i]->velocity.x;
				align.y += boids[i]->velocity.y;

				// Increment the cohesion force
				cohesion.x += boids[i]->position.x;
				cohesion.y += boids[i]->position.y;

				// Increment the seperation force
				olc::vf2d diff;
				diff.x = this->position.x - boids[i]->position.x;
				diff.y = this->position.y - boids[i]->position.y;
				diff.x /= d * d;
				diff.y /= d * d;
				seperation.x += diff.x;
				seperation.y += diff.y;

				total++;
			}
		}

		if (total)
		{
			/* ======== Align ======== */
			// Dividing align vector by total to get the average
			align.x /= total;
			align.y /= total;

			// Setting the magnitude of the vector to the max velocity
			float length_align = sqrtf(align.x * align.x + align.y * align.y);
			align.x = align.x / length_align * this->maxVel;
			align.y = align.y / length_align * this->maxVel;

			// Subtract velocity vector from align vector
			align.x -= this->velocity.x;
			align.y -= this->velocity.y;

			// Limit the vector length to the max force allowed
			length_align = sqrtf(align.x * align.x + align.y * align.y);
			if (length_align > this->maxForce)
			{
				align.x = align.x / length_align * this->maxForce;
				align.y = align.y / length_align * this->maxForce;
			}

			/* ======== Cohesion ======== */
			// Dividing cohesion vector by total to get the average
			cohesion.x /= total;
			cohesion.y /= total;

			// Subtract position vector from cohesion vector
			cohesion.x -= this->position.x;
			cohesion.y -= this->position.y;

			// Setting the magnitude of the vector to the max velocity
			float length_cohesion = sqrtf(cohesion.x * cohesion.x + cohesion.y * cohesion.y);
			cohesion.x = cohesion.x / length_cohesion * this->maxVel;
			cohesion.y = cohesion.y / length_cohesion * this->maxVel;

			// Subtract velocity vector from cohesion vector
			cohesion.x -= this->velocity.x;
			cohesion.y -= this->velocity.y;

			// Limit the vector length to the max force allowed
			length_cohesion = sqrtf(cohesion.x * cohesion.x + cohesion.y * cohesion.y);
			if (length_cohesion > this->maxForce)
			{
				cohesion.x = cohesion.x / length_cohesion * this->maxForce;
				cohesion.y = cohesion.y / length_cohesion * this->maxForce;
			}

			/* ======== Seperation ======== */
			// Dividing seperation vector by total to get the average
			seperation.x /= total;
			seperation.y /= total;

			// Setting the magnitude of the vector to the max velocity
			float length_seperation = sqrtf(seperation.x * seperation.x + seperation.y * seperation.y);
			seperation.x = seperation.x / length_seperation * this->maxVel;
			seperation.y = seperation.y / length_seperation * this->maxVel;

			// Subtract velocity vector from seperation vector
			seperation.x -= this->velocity.x;
			seperation.y -= this->velocity.y;

			// Limit the vector length to the max force allowed
			length_seperation = sqrtf(seperation.x * seperation.x + seperation.y * seperation.y);
			if (length_seperation > this->maxForce)
			{
				seperation.x = seperation.x / length_seperation * this->maxForce;
				seperation.y = seperation.y / length_seperation * this->maxForce;
			}
		}

		// Add align force to acceleration
		this->acceleration.x += align.x;
		this->acceleration.y += align.y;

		// Add cohesion force to acceleration
		this->acceleration.x += cohesion.x;
		this->acceleration.y += cohesion.y;

		// Add seperation force to acceleration
		this->acceleration.x += seperation.x;
		this->acceleration.y += seperation.y;
	}

	void update(float deltaTime, float width, float height)
	{
		this->velocity.x += this->acceleration.x * deltaTime;
		this->velocity.y += this->acceleration.y * deltaTime;

		float length_velocity = sqrtf(this->velocity.x * this->velocity.x + this->velocity.y * this->velocity.y);
		if (length_velocity > this->maxVel)
		{
			this->velocity.x = this->velocity.x / length_velocity * this->maxVel;
			this->velocity.y = this->velocity.y / length_velocity * this->maxVel;
		}

		this->position.x += this->velocity.x * deltaTime;
		this->position.y += this->velocity.y * deltaTime;

		this->position.x = fmod(this->position.x + width, width);
		this->position.y = fmod(this->position.y + height, height);

		this->acceleration.x = 0;
		this->acceleration.y = 0;
	}
};


class BoidSim : public olc::PixelGameEngine
{
public:
	BoidSim()
	{
		sAppName = "Boid Simulation";
	}

private:
	const int boidCount = 2000;
	Boid *boids[2000];

public:
	bool OnUserCreate() override
	{
		// Called once at the start, so create things here
		for (int i = 0; i < boidCount; i++)
		{
			boids[i] = new Boid(ScreenWidth(), ScreenHeight());
		}

		return true;
	}

	void DrawBoid(olc::vf2d position, olc::vf2d velocity)
	{
		float headlen = 3;
		float vectorlen = 10;
		
		float fromx = position.x;
		float fromy = position.y;

		float velocityLength = sqrtf(velocity.x * velocity.x + velocity.y * velocity.y);
		velocity.x = velocity.x / velocityLength * vectorlen;
		velocity.y = velocity.y / velocityLength * vectorlen;
		float tox = position.x + velocity.x;
		float toy = position.y + velocity.y;

		float dx = tox - fromx;
		float dy = toy - fromy;
		float angle = atan2f(dy, dx);

		// Draw arrow
		DrawLine(olc::vi2d((int)fromx, (int)fromy), olc::vi2d((int)tox, (int)toy), olc::WHITE);
		DrawLine(olc::vi2d((int)tox, (int)toy), olc::vi2d((int)(tox - headlen * cosf(angle - PI / 6)), (int)(toy - headlen * sin(angle - PI / 6))), olc::WHITE);
		DrawLine(olc::vi2d((int)tox, (int)toy), olc::vi2d((int)(tox - headlen * cosf(angle + PI / 6)), (int)(toy - headlen * sin(angle + PI / 6))), olc::WHITE);
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		// called once per frame		
		Clear(olc::BLACK);
		
		for (int boid = 0; boid < boidCount; boid++)
		{
			boids[boid]->steer(boids);
			boids[boid]->update(fElapsedTime, (float)ScreenWidth(), (float)ScreenHeight());
			DrawBoid(boids[boid]->position, boids[boid]->velocity);
			//FillCircle(boids[boid]->position, 2, olc::WHITE);
		}

		return true;
	}
};


int main()
{
	srand((unsigned)time(0));

	BoidSim sim;
	if (sim.Construct(1280, 720, 1, 1))
		sim.Start();

	return 0;
}
