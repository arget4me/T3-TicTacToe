#include "game.h"
#include "Renderer/shader.h"
#include "stb_image.h"
#include "Utils/logfile.h"


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

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);


	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(0 * sizeof(float)));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load and generate the texture
	int width, height, nrChannels;
	unsigned char* data = stbi_load("data/temp.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		DEBUG_LOG("Failed to load texture\n");
	}
	stbi_image_free(data);


}

bool update_game()
{

	return false;
}


void render_game()
{
	t3::useShaderProgram(p);
	glBindVertexArray(vao);
	glBindTexture(GL_TEXTURE_2D, texture);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}


