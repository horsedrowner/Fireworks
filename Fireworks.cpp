#include "Libs.h"
#include "Fireworks.h"

Fireworks::Fireworks(unsigned int width, unsigned int height, bool fullscreen) : 
	width(width), height(height), fullscreen(fullscreen), maxParticles(300) {
	particles = new ParticleSystem(width, height);
}

Fireworks::~Fireworks() { 
	if (window != NULL) {
		delete window;
		window = NULL;
	}

	if (particles != NULL) {
		delete particles;
		particles = NULL;
	}
}

bool Fireworks::Load() {
	// Create window
	unsigned long style = sf::Style::Close;
	if (this->fullscreen)
		style |= sf::Style::Fullscreen;

	window = new sf::Window(sf::VideoMode(width, height), "Fireworks", style);

	// Initialize GLEW
	glewExperimental = GL_TRUE;
	GLenum init = glewInit();
	if (init != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW: %s\n", glewGetErrorString(init));
		return false;
	}
	fprintf(stdout, "Loaded GLEW %s\n", glewGetString(GLEW_VERSION));

	glEnable(GL_POINT_SMOOTH);

	return true;
}

void Fireworks::Run() {
	while (window->IsOpened()) {
		DoEvents();

		float frametime = window->GetFrameTime();
		Update(frametime);
		Render();

		window->Display();
	}
}

void Fireworks::DoEvents() {
	sf::Event windowEvent;
	while (window->GetEvent(windowEvent)) {
		switch (windowEvent.Type) {
		case sf::Event::Closed:
			window->Close();
			break;

		case sf::Event::KeyPressed:
			switch (windowEvent.Key.Code) {
			case sf::Key::Escape:
				window->Close();
				break;

			case sf::Key::Space:
				AddSpawner();
				break;
			
			case sf::Key::R:
			case sf::Key::F5:
				if (particles != NULL)
					particles->Clear();
				break;

			case sf::Key::Return:
				for (unsigned i = 0; i < 10; i++)
					AddSpawner();
			}
			break;
		}
	}
}

void Fireworks::Update(float frametime) {
	fprintf(stdout, "Frame time: %.1f ms\t%u particle(s)\r", frametime * 1000.0f, particles->Count());

	if (particles->Count() < this->maxParticles) {
		AddSpawner();
	}

	particles->Update(frametime);
}

void Fireworks::Render() {
	glViewport(0, 0, this->width, this->height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0f, this->width, 0.0f, this->height, -1.0f, 1.0f);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	particles->Render();
}


void Fireworks::SetParticleLimit(size_t maxParticles) {
	this->maxParticles = maxParticles;
}

size_t Fireworks::GetParticleLimit() const {
	return this->maxParticles;
}

void Fireworks::AddRandomParticle() {
	Vector2 pos((float)(rand() % this->width), (float)(rand() % this->height));

	static double h = (rand() % 6) * 60; // Pick one of 6 random colors
	Color color(h, 1.0f, 1.0f);			 // Particles start with a "flash"

	float size = rand() % 4 + 4.0f;		 // Give a varied size
	if (rand() % 100 == 0)				 // 1% chance of an even bigger particle
		size += 12.0f;

	Vector2 initialSpeed = Vector2(0.0f, 20.0f); // Particles have some initial speed
	Vector2 force = Vector2(0.0f, 15.0f); // Particles move downwards

	Particle *p = new Particle(pos, color, size, initialSpeed, force);
	particles->Add(p);
}

void Fireworks::AddSpawner() {
	Vector2 pos((float)(rand() % this->width), 0.0f);

	double h = (rand() % 6) * 60;
	Color color(h, 1.0f, 0.5f);

	float size = 15.0f;

	Vector2 initialSpeed = Vector2(0.0f, 0.0f);
	Vector2 force = Vector2(0.0f, -80.0f);

	SpawnerParticle *p = new SpawnerParticle(pos, color, size, initialSpeed, force);
	p->SetFuseTime(0.4f * (rand() / (float)RAND_MAX) + 0.6f);
	particles->Add(p);
}