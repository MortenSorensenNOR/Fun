#include <vector>

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#define OLC_PGEX_TRANSFORMEDVIEW
#include "olcPGEX_TransformedView.h"

using namespace std;

class Gravity : public olc::PixelGameEngine
{
public:
	Gravity()
	{
		sAppName = "Example";
	}

private:
	olc::TransformedView tv;

	double AU = 1.495978707e11;
	double G = 6.67428e-11;
	double Scale = 150 / AU; // 1AU = 100 pixels
	float TimeStep = 3600 * 24 * 73; // 1 orbit per 5 seconds
	float sun_speed = 5.0e10;

	float lastTrajectoryUpdate = 0.5;

	struct CelestialBody
	{
		int radius;
		double mass;
		olc::vd2d pos;
		olc::vd2d vel; 
		std::vector<olc::vd2d> forces;
		olc::vd2d sum_of_forces;
		std::vector<olc::vd2d> trajectory;
		float orbitTime;

		void SumOfForces()
		{
			for (int i = 0; i < forces.size(); i++)
			{
				sum_of_forces.x += forces[i].x;
				sum_of_forces.y += forces[i].y;
			}
		}

		void UpdatePosition(const float deltaTime, const float TimeStep)
		{
			this->vel.x += this->sum_of_forces.x / this->mass * (double)(deltaTime * TimeStep);
			this->vel.y += this->sum_of_forces.y / this->mass * (double)(deltaTime * TimeStep);

			this->pos.x += this->vel.x * (double)(deltaTime * TimeStep);
			this->pos.y += this->vel.y * (double)(deltaTime * TimeStep);
		}
	};

	CelestialBody Sun;
	CelestialBody Earth;
	CelestialBody Mars;
	CelestialBody Venus;
	CelestialBody Mercury;

public:
	bool OnUserCreate() override
	{
		tv.Initialise({ ScreenWidth(), ScreenHeight() });

		Sun.radius = 30;
		Sun.mass = 1.98892e30;
		Sun.pos = { 0.0, 0.0 };
		Sun.vel = { 0.0, 0.0 };

		Earth.radius = 16;
		Earth.mass = 5.9742e24;
		Earth.pos = { -1.0 * AU, 0.0 };
		Earth.vel = { 0.0, 2.9783e4 };
		Earth.orbitTime = 2.0 * 3.1415926536 * abs(Earth.pos.x) / (Earth.vel.y * TimeStep);

		Mars.radius = 12;
		Mars.mass = 6.39e23;
		Mars.pos = { -1.524 * AU, 0.0 };
		Mars.vel = { 0.0, 2.4077e4 };
		Mars.orbitTime = 2.0 * 3.1415926536 * abs(Mars.pos.x) / (Mars.vel.y * TimeStep);

		Venus.radius = 14;
		Venus.mass = 4.8685e24;
		Venus.pos = { 0.723 * AU, 0.0 };
		Venus.vel = { 0.0, -3.502e4 };
		Venus.orbitTime = 2.0 * 3.1415926536 * Venus.pos.x / abs(Venus.vel.y * TimeStep);

		Mercury.radius = 8;
		Mercury.mass = 3.30e23;
		Mercury.pos = { 0.387 * AU, 0.0 };
		Mercury.vel = { 0.0, -4.74e4 };
		Mercury.orbitTime = 2.0 * 3.1415926536 * Mercury.pos.x / abs(Mercury.vel.y * TimeStep);

		return true;
	}

	olc::vf2d GravityForce(const double m1, const double m2, const olc::vd2d& r1, const olc::vd2d& r2)
	{
		double distance = sqrt((r2.x - r1.x) * (r2.x - r1.x) + (r2.y - r1.y) * (r2.y - r1.y));
		if (distance == 0) return { 0, 0 };
		double dUnitVectorX = (r2.x - r1.x) / distance;
		double dUnitVectorY = (r2.y - r1.y) / distance;
		double forceX = (-1.0 * G * (double)(m1 * m2) * dUnitVectorX) / (distance * distance);
		double forceY = (-1.0 * G * (double)(m1 * m2) * dUnitVectorY) / (distance * distance);
		olc::vd2d Force = { forceX, forceY };

		return Force;
	}

	void DrawBody(const olc::vd2d& pos, const int radius, const olc::Pixel color)
	{
		float x = (float)pos.x * Scale + (float)ScreenWidth() / 2.0;
		float y = (float)pos.y * Scale + (float)ScreenHeight() / 2.0;
		olc::vd2d scaledPos = { x, y };
		tv.FillCircle(scaledPos, radius, color);
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		tv.HandlePanAndZoom(0);
		lastTrajectoryUpdate += fElapsedTime;

		if (GetKey(olc::Key::W).bHeld)
			Sun.vel.y -= sun_speed * fElapsedTime;
		if (GetKey(olc::Key::S).bHeld)
			Sun.vel.y += sun_speed * fElapsedTime;
		if (GetKey(olc::Key::A).bHeld)
			Sun.vel.x -= sun_speed * fElapsedTime;
		if (GetKey(olc::Key::D).bHeld)
			Sun.vel.x += sun_speed * fElapsedTime;

		Sun.pos.x += Sun.vel.x * fElapsedTime;
		Sun.pos.y += Sun.vel.y * fElapsedTime;

		Earth.forces.push_back(GravityForce(Sun.mass, Earth.mass, Sun.pos, Earth.pos));
		Earth.forces.push_back(GravityForce(Mars.mass, Earth.mass, Mars.pos, Earth.pos));
		Earth.forces.push_back(GravityForce(Venus.mass, Earth.mass, Venus.pos, Earth.pos));
		Earth.forces.push_back(GravityForce(Mercury.mass, Earth.mass, Mercury.pos, Earth.pos));
		Earth.SumOfForces();
		Earth.UpdatePosition(fElapsedTime, TimeStep);

		Mars.forces.push_back(GravityForce(Sun.mass, Mars.mass, Sun.pos, Mars.pos));
		Mars.forces.push_back(GravityForce(Earth.mass, Mars.mass, Earth.pos, Mars.pos));
		Mars.forces.push_back(GravityForce(Venus.mass, Mars.mass, Venus.pos, Mars.pos));
		Mars.forces.push_back(GravityForce(Mercury.mass, Mars.mass, Mercury.pos, Mars.pos));
		Mars.SumOfForces();
		Mars.UpdatePosition(fElapsedTime, TimeStep);

		Venus.forces.push_back(GravityForce(Sun.mass, Venus.mass, Sun.pos, Venus.pos));
		Venus.forces.push_back(GravityForce(Earth.mass, Venus.mass, Earth.pos, Venus.pos));
		Venus.forces.push_back(GravityForce(Mars.mass, Venus.mass, Mars.pos, Venus.pos));
		Venus.forces.push_back(GravityForce(Mercury.mass, Venus.mass, Mercury.pos, Venus.pos));
		Venus.SumOfForces();
		Venus.UpdatePosition(fElapsedTime, TimeStep);

		Mercury.forces.push_back(GravityForce(Sun.mass, Mercury.mass, Sun.pos, Mercury.pos));
		Mercury.forces.push_back(GravityForce(Earth.mass, Mercury.mass, Earth.pos, Mercury.pos));
		Mercury.forces.push_back(GravityForce(Mars.mass, Mercury.mass, Mars.pos, Mercury.pos));
		Mercury.forces.push_back(GravityForce(Venus.mass, Mercury.mass, Venus.pos, Mercury.pos));
		Mercury.SumOfForces();
		Mercury.UpdatePosition(fElapsedTime, TimeStep);

		if (lastTrajectoryUpdate >= 0.1)
		{
			Earth.trajectory.push_back(Earth.pos);
			if (Earth.trajectory.size() > Earth.orbitTime / 0.1) Earth.trajectory.erase(Earth.trajectory.begin());

			Mars.trajectory.push_back(Mars.pos);
			if (Mars.trajectory.size() > Mars.orbitTime / 0.1) Mars.trajectory.erase(Mars.trajectory.begin());

			Venus.trajectory.push_back(Venus.pos);
			if (Venus.trajectory.size() > Venus.orbitTime / 0.1) Venus.trajectory.erase(Venus.trajectory.begin());

			Mercury.trajectory.push_back(Mercury.pos);
			if (Mercury.trajectory.size() > Mercury.orbitTime / 0.1) Mercury.trajectory.erase(Mercury.trajectory.begin());

			lastTrajectoryUpdate = 0;
		}
;
		// DRAW
		Clear(olc::BLACK);

		for (int t = 0; t < Earth.trajectory.size(); t++)
			DrawBody(Earth.trajectory[t], 2, olc::Pixel(52, 119, 235));
		for (int t = 0; t < Mars.trajectory.size(); t++)
			DrawBody(Mars.trajectory[t], 2, olc::Pixel(235, 64, 52));
		for (int t = 0; t < Venus.trajectory.size(); t++)
			DrawBody(Venus.trajectory[t], 2, olc::Pixel(255, 255, 200));
		for (int t = 0; t < Mercury.trajectory.size(); t++)
			DrawBody(Mercury.trajectory[t], 2, olc::Pixel(80, 78, 81));

		DrawBody(Sun.pos, Sun.radius, olc::Pixel(255, 255, 0));
		DrawBody(Earth.pos, Earth.radius, olc::Pixel(52, 119, 235));
		DrawBody(Mars.pos, Mars.radius, olc::Pixel(235, 64, 52));
		DrawBody(Venus.pos, Venus.radius, olc::Pixel(255, 255, 200));
		DrawBody(Mercury.pos, Mercury.radius, olc::Pixel(80, 78, 81));

		Earth.forces = {};
		Mars.forces = {};
		Venus.forces = {};
		Mercury.forces = {};

		Earth.sum_of_forces = { 0, 0 };
		Mars.sum_of_forces = { 0, 0 };
		Venus.sum_of_forces = { 0, 0 };
		Mercury.sum_of_forces = { 0, 0 };

		return true;
	}
};


int main()
{
	Gravity demo;
	if (demo.Construct(800, 800, 1, 1))
		demo.Start();

	return 0;
}
