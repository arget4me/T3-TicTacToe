#ifndef BATCH_REDERER_HEADER
#define BATCH_REDERER_HEADER


#include "GLEW/glew.h"
#include "glm/glm.hpp"

namespace t3 
{

	typedef struct{
		glm::vec3 position;
		glm::vec3 color;
		glm::vec2 uv_coords;
	}Vertex;

	GLuint loadTexture(const char* image_file_path, GLuint texture_slot);

	void initialize_batch_renderer();
	
	void set_layer_z(float z_value);

	void submit_sprite(int sprite_offset, int num_sprites_width, float center_x, float center_y, float width, float height);

	void render_batch();

}



#endif 
