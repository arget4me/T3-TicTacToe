#include "game.h"
#include "Renderer/shader.h"
#include "Renderer/batch_renderer.h"


//globals
t3::ShaderProgram p;
unsigned int texture;

GLuint vao, vbo, ebo;
float vertices[] = {
	-0.5f, +0.5f, 0.0f,		1.0f, 0.0f, 0.0f,	0.0f, 1.0f,//0 - topleft
	+0.5f, +0.5f, 0.0f,		1.0f, 0.0f, 0.0f,	1.0f, 1.0f,//1 - topright
	-0.5f, -0.5f, 0.0f,		1.0f, 0.0f, 0.0f,	0.0f, 0.0f,//2 - bottomleft
	+0.5f, -0.5f, 0.0f,		1.0f, 0.0f, 0.0f,	1.0f, 0.0f,//3 - bottomright
};

int indices[] = {
	2, 1, 0, //Triangle 0
	2, 3, 1, //Triangle 2
};




void initialize_game()
{
	p = t3::compileShader("data/vertex.glsl", "data/fragment.glsl");
	t3::initialize_batch_renderer();
	texture = t3::loadTexture("data/temp.jpg", GL_TEXTURE0);
}

bool update_game()
{

	return false;
}


void render_game()
{
	t3::useShaderProgram(p);
	glBindTexture(GL_TEXTURE_2D, texture);

	t3::submit_sprite(0, 2, 0.8f, 0.0f, 0.3f, 1);

	t3::submit_sprite(1, 2, -0.5f, 0.5f, 0.1f, 0.1f);


	t3::submit_sprite(2, 2, -0.5f, 0.7f, 0.1f, 0.1f);

	t3::submit_sprite(3, 2, -0.8f, 0.7f, 0.1f, 0.1f);


	t3::submit_sprite(0, 2, 0.0f - 0.05f, 0.0f - 0.05f, 0.1f, 0.1f);
	t3::submit_sprite(1, 2, 0.0f + 0.05f, 0.0f - 0.05f, 0.1f, 0.1f);
	t3::submit_sprite(2, 2, 0.0f - 0.05f, 0.0f + 0.05f, 0.1f, 0.1f);
	t3::submit_sprite(3, 2, 0.0f + 0.05f, 0.0f + 0.05f, 0.1f, 0.1f);

	t3::render_batch();
}


