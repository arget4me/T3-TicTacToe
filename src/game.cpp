#include "game.h"
#include "Renderer/shader.h"
#include "Renderer/batch_renderer.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"


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
	//
}

bool update_game()
{

	return false;
}

float position[3] = { 1280.0f/2, 720.0f/2, 10.0f };
float tile_size = 720.0f / 4.0f;
void render_game()
{
	
	ImGui_ImplGlfwGL3_NewFrame();
	ImGui::Text("Debug Panel:");
	ImGui::Separator();
	ImGui::DragFloat3("Position", position);
	ImGui::DragFloat("Tilesize", &tile_size);

	ImGui::Render();

	glm::mat4 projection_matrix(1.0f);
	projection_matrix = glm::ortho(0.0f, 1280.0f, 0.0f, 720.0f, -0.1f, -100.0f);
	//projection_matrix = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -0.1f, -100.0f);
	t3::useShaderProgram(p);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniformMatrix4fv(glGetUniformLocation(p.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection_matrix));

	
	//t3::set_layer_z(0.5f);
	t3::set_layer_z(position[2]);
	for(int y = 0; y < 3; y++)
	{
		for (int x = 0; x < 3; x++)
		{
			//t3::submit_sprite(0, 2, 0.0f + tile_size * (x - 1), 0.0f + tile_size * (1 - y), tile_size, tile_size);
			t3::submit_sprite(0, 2, position[0] + tile_size * (x - 1), position[1] + tile_size * (1 - y), tile_size, tile_size);
		}
	}
	t3::set_layer_z(1.0f);
	t3::submit_sprite(2, 2, position[0] + tile_size * (0 - 1), position[1] + tile_size * (1 - 0), tile_size, tile_size);
	t3::submit_sprite(1, 2, position[0] + tile_size * (1 - 1), position[1] + tile_size * (1 - 1), tile_size, tile_size);



	t3::render_batch();
}


