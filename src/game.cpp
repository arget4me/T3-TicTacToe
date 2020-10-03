#include "game.h"
#include "Renderer/shader.h"
#include "Renderer/batch_renderer.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"
#include "Utils/logfile.h"
#include "server/t3_server.h"


struct t3GameState global_game_state;


//globals
t3::ShaderProgram p;
unsigned int texture;
float position[3] = { 1280.0f / 2, 720.0f / 2, 10.0f };
float tile_size = 720.0f / 4.0f;
bool show_winner = false;
PlayerTurn winner = PlayerTurn::O_PLAYER;

t3GameState::t3GameState() :data_buffer {0}
{
	for (int byte = 0; byte < 3; byte++)
	{
		for (int bit = 0; bit < 8; bit+=2)
		{
			int bit_number = byte * 8 + bit;
			int tile_number = bit_number / get_bits_per_tile();
			char tile_value;
			
			if (tile_number < 9)
			{
				tile_value = 0x1; //empty
				//DEBUG_LOG("[" << byte << ", " << bit << "] Tile number= " << tile_number << "\n");
			}
			else if(tile_number == 9)
			{
				tile_value = 0x0; //O, player turn
				//DEBUG_LOG("[" << byte << ", " << bit << "] Bit number= " << bit_number << " | player_turn bit\n");
			}
			else
			{
				tile_value = 0x2; //padding
				//DEBUG_LOG("[" << byte << ", " << bit << "] bit number= " << bit_number << " | padding bits\n");
			}
			data_buffer[byte] |= ((tile_value &0x3) << (6 - bit));

		}
	}
}

const TileState t3GameState::get_tile_state(int tile_nr) const
{
	TileState state = TileState::PADDING_TILE;
	char tile_value = 0x2;
	if (tile_nr >= 0 && tile_nr < 9)
	{
		int byte = tile_nr / (8 / get_bits_per_tile());
		int bit = (tile_nr - byte * (8 / get_bits_per_tile()) ) * get_bits_per_tile();
		{
			tile_value = (data_buffer[byte] >> (6 - bit)) & 0x3;
		}
		if (tile_value == 0x0)
		{
			state = TileState::O_TILE;
		} else if (tile_value == 0x1)
		{
			state = TileState::EMPTY_TILE;
		}else if (tile_value == 0x3)
		{
			state = TileState::X_TILE;
		}

	}

	return state;
}

const PlayerTurn t3GameState::get_player_turn() const
{
	PlayerTurn state = PlayerTurn::O_PLAYER;
	char player_state_value;
	constexpr int player_state_offset = 9;
	int byte = player_state_offset / (8 / get_bits_per_tile());
	int bit = (player_state_offset - byte * (8 / get_bits_per_tile())) * get_bits_per_tile();
	{
		player_state_value = (data_buffer[byte] >> (6 - bit)) & 0x3;
	}
	if (player_state_value == 0x0)
	{
		state = PlayerTurn::O_PLAYER;
	}
	else if (player_state_value == 0x3)
	{
		state = PlayerTurn::X_PLAYER;
	}

	return state;
}

void t3GameState::set_player_turn(PlayerTurn state)
{
	constexpr int player_state_offset = 9;
	int byte = player_state_offset / (8 / get_bits_per_tile());
	int bit = (player_state_offset - byte * (8 / get_bits_per_tile())) * get_bits_per_tile();

	char tile_value = 0x2;
	if (state == PlayerTurn::O_PLAYER)
	{
		tile_value = 0x0;
	}
	else if (state == PlayerTurn::X_PLAYER)
	{
		tile_value = 0x3;
	}
	{
		tile_value = (tile_value & 0x3) << (6 - bit);
		char bit_mask = 0x3 << (6 - bit);
		data_buffer[byte] = (data_buffer[byte] & (~bit_mask)) | tile_value;
	}
}



const bool t3GameState::check_place_piece_on_tile(PlayerTurn player, int tile_nr) {
	if (global_game_state.get_player_turn() == player)
	{
		if (global_game_state.get_tile_state(tile_nr) == TileState::EMPTY_TILE) {
			return true;
		}
	}
	return false;
}

void t3GameState::set_tile_state(int tile_nr, TileState state)
{
	if (tile_nr >= 0 && tile_nr < 9)
	{
		char tile_value = 0x2;
		if (state == TileState::O_TILE)
		{
			tile_value = 0x0;
		}
		else if (state == TileState::EMPTY_TILE)
		{
			tile_value = 0x1;
		}
		else if (state == TileState::X_TILE)
		{
			tile_value = 0x3;
		}

		int byte = tile_nr / (8 / get_bits_per_tile());
		int bit = (tile_nr - byte * (8 / get_bits_per_tile())) * get_bits_per_tile();
		{
			tile_value = (tile_value & 0x3) << (6 - bit);
			char bit_mask = 0x3 << (6 - bit);
			data_buffer[byte] = (data_buffer[byte] & (~bit_mask)) | tile_value;
		}
	}
}

void server_receive_callback(char* data, int size) {
	//TODO:
}


void client_receive_callback(char* data, int size) {
	//TODO:
}

void initialize_game()
{
	p = t3::compileShader("data/vertex.glsl", "data/fragment.glsl");
	t3::initialize_batch_renderer();
	texture = t3::loadTexture("data/spritesheet.png", GL_TEXTURE0);
	t3::register_receive_callback(&server_receive_callback);
}

bool update_game(Mouse& mouse)
{
	if (mouse.middle_btn.get_click())
	{
		show_winner = true;
		winner = PlayerTurn::O_PLAYER;
	}
	if (mouse.middle_btn.get_click())
	{
		show_winner = true;
		winner = PlayerTurn::X_PLAYER;
	}

	if (mouse.left_btn.get_click())
	{
		float origin_x = position[0] - 1.5f * tile_size;
		float origin_y = position[1] - 1.5f * tile_size;
		float dx = (float)mouse.x - origin_x;
		float dy = (float)mouse.y - origin_y;
		if (dx >= 0.0f && dy >= 0.0f)
		{
			int tile_x = (int)(dx / tile_size);
			int tile_y = (int)(dy / tile_size);

			if (tile_x < 3 && tile_y < 3)
			{
				int tile_nr = tile_x + tile_y * 3;
				if (global_game_state.check_place_piece_on_tile(global_game_state.get_player_turn(), tile_nr)) {
					if (global_game_state.get_player_turn() == PlayerTurn::O_PLAYER)
					{
						global_game_state.set_tile_state(tile_nr, TileState::O_TILE);
						global_game_state.set_player_turn(PlayerTurn::X_PLAYER);
					}
					else if (global_game_state.get_player_turn() == PlayerTurn::X_PLAYER)
					{
						global_game_state.set_tile_state(tile_nr, TileState::X_TILE);
						global_game_state.set_player_turn(PlayerTurn::O_PLAYER);
					}
				}
			}
		}
	}
	return false;
}


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
	t3::useShaderProgram(p);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniformMatrix4fv(glGetUniformLocation(p.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection_matrix));
	
	for (int y = 0; y < 3; y++)
	{
		for (int x = 0; x < 3; x++)
		{
			t3::set_layer_z(position[2]);
			t3::submit_sprite(0, 2, position[0] + tile_size * (x - 1), position[1] + tile_size * (1 - y), tile_size, tile_size);
			t3::set_layer_z(2.0f);

			TileState state = global_game_state.get_tile_state(x + y * 3);
			if (state == TileState::X_TILE)
			{
				t3::submit_sprite(1, 2, position[0] + tile_size * (x - 1), position[1] + tile_size * (1 - y), tile_size, tile_size);
			}
			else if (state == TileState::O_TILE)
			{
				t3::submit_sprite(2, 2, position[0] + tile_size * (x - 1), position[1] + tile_size * (1 - y), tile_size, tile_size);
			}
		}
	}

	if (show_winner)
	{
		t3::set_layer_z(1.0f);
		t3::submit_sprite(3, 2, tile_size, tile_size, tile_size, tile_size);
		if (winner == PlayerTurn::O_PLAYER)
		{
			t3::submit_sprite(2, 2, tile_size * 2, tile_size, tile_size, tile_size);
		}else if (winner == PlayerTurn::X_PLAYER)
		{
			t3::submit_sprite(1, 2, tile_size * 2, tile_size, tile_size, tile_size);
		}
	}


	t3::render_batch();
}


