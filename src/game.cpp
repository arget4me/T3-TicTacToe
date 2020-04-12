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
	texture = t3::loadTexture("data/spritesheet.png", GL_TEXTURE0);
}

bool update_game()
{

	return false;
}


void render_game()
{

	t3::useShaderProgram(p);
	glBindTexture(GL_TEXTURE_2D, texture);

	float tile_size = 0.6f;
	t3::set_layer_z(0.5f);
	for(int y = 0; y < 3; y++)
	{
		for (int x = 0; x < 3; x++)
		{
			t3::submit_sprite(0, 4, 0.0f + tile_size * (x - 1), 0.0f + tile_size * (1 - y), tile_size, tile_size);
		}
	}
	t3::set_layer_z(0.0f);
	t3::submit_sprite(2, 4, 0.0f + tile_size * (0 - 1), 0.0f + tile_size * (1 - 0), tile_size, tile_size);
	t3::submit_sprite(1, 4, 0.0f + tile_size * (1 - 1), 0.0f + tile_size * (1 - 1), tile_size, tile_size);



	t3::render_batch();
}


