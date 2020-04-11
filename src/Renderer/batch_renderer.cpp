#include "batch_renderer.h"
#include "GLEW/glew.h"
#include "stb_image.h"
#include "Utils/logfile.h"

namespace t3
{

	const int BATCH_MAX_SIZE = 1000;

	typedef struct
	{

		Vertex bottom_left;		//0
		Vertex bottom_right;	//1
		Vertex top_right;		//2
		Vertex top_left;		//3

	}SpriteQuad;

	typedef struct
	{
		GLuint top_0; 
		GLuint top_1;
		GLuint top_2;

		GLuint bottom_0;
		GLuint bottom_1;
		GLuint bottom_2;

	}SpriteQuadIndices;

	SpriteQuad sprite_batch[BATCH_MAX_SIZE];
	SpriteQuadIndices sprite_batch_indices[BATCH_MAX_SIZE];
	int current_batch_length = 0;
	GLuint vao, vbo, ebo;


	GLuint loadTexture(const char* image_file_path, GLuint texture_slot)
	{
		GLuint texture;
		glGenTextures(1, &texture);
		glActiveTexture(texture_slot);
		glBindTexture(GL_TEXTURE_2D, texture);
		// set the texture wrapping/filtering options (on the currently bound texture object)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// load and generate the texture
		int width, height, nrChannels;
		unsigned char* data = stbi_load(image_file_path, &width, &height, &nrChannels, 0);
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
		glBindTexture(GL_TEXTURE_2D, 0);
		return texture;
	}


	void initialize_batch_renderer()
	{
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);


		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(sprite_batch), sprite_batch, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(0 * sizeof(float)));
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		glGenBuffers(1, &ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sprite_batch_indices), sprite_batch_indices, GL_STATIC_DRAW);

	}


	void submit_sprite(int sprite_offset, int num_sprites_width, float center_x, float center_y, float width, float height)
	{
		SpriteQuad& current_sprite = sprite_batch[current_batch_length];

		float min_x = center_x - width/2;
		float max_x = center_x + width/2;

		float min_y = center_y - height/2;
		float max_y = center_y + height/2;

		if (num_sprites_width == 0)num_sprites_width = 1;
		
		float min_u = (float)(sprite_offset % num_sprites_width) / (float)num_sprites_width;
		float max_u = (float)(sprite_offset % num_sprites_width + 1) / (float)num_sprites_width;
		float min_v = (float)(sprite_offset / num_sprites_width) / (float)num_sprites_width;
		float max_v = (float)(sprite_offset / num_sprites_width + 1) / (float)num_sprites_width;

		current_sprite.bottom_left.position = glm::vec3(min_x, min_y, 0);
		current_sprite.bottom_left.uv_coords = glm::vec2(min_u, min_v);

		current_sprite.bottom_right.position = glm::vec3(max_x, min_y, 0);
		current_sprite.bottom_right.uv_coords = glm::vec2(max_u, min_v);

		current_sprite.top_right.position = glm::vec3(max_x, max_y, 0);
		current_sprite.top_right.uv_coords = glm::vec2(max_u, max_v);

		current_sprite.top_left.position = glm::vec3(min_x, max_y, 0);
		current_sprite.top_left.uv_coords = glm::vec2(min_u, max_v);


		SpriteQuadIndices& current_sprite_indices = sprite_batch_indices[current_batch_length];
		current_sprite_indices.top_0 = current_batch_length * 4 + 0;
		current_sprite_indices.top_1 = current_batch_length * 4 + 2;
		current_sprite_indices.top_2 = current_batch_length * 4 + 3;
		current_sprite_indices.bottom_0 = current_batch_length * 4 + 0;
		current_sprite_indices.bottom_1 = current_batch_length * 4 + 1;
		current_sprite_indices.bottom_2 = current_batch_length * 4 + 2;

		current_batch_length++;
	}

	void render_batch()
	{
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(SpriteQuad) * current_batch_length, sprite_batch, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(SpriteQuadIndices) * current_batch_length, sprite_batch_indices, GL_STATIC_DRAW);

		//glDrawArrays(GL_TRIANGLES, 0, current_batch_length * sizeof(Vertex));
		glDrawElements(GL_TRIANGLES, 6 * current_batch_length, GL_UNSIGNED_INT, 0);

		current_batch_length = 0;
	}

}