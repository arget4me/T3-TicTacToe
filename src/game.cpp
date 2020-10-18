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
#include "Menu/Button.h"
#include <thread>

struct t3GameState global_game_state;


//globals
t3::ShaderProgram p;
unsigned int texture;
float position[3] = { 1280.0f / 2, 720.0f / 2, 10.0f };
float tile_size = 720.0f / 4.0f;
bool show_winner = false;
PlayerTurn winner = PlayerTurn::O_PLAYER;
t3::ImageButton start_host_btn;
t3::ImageButton start_client_btn;
static std::thread* server_thread;

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

void receive_callback(char* data, int size) {
	//TODO: Use QUEUE??

	DEBUG_LOG("message received! Size: [" << size << "]\n");

	int state_buffer_size = global_game_state.get_buffer_size();
	if (size < state_buffer_size) state_buffer_size = size;

	//COPY data...
	for (int i = 0; i < state_buffer_size; i++)
	{
		global_game_state.data_buffer[i] = data[i];
	}
}


void btn_callback(float mx, float my)
{
	DEBUG_LOG("Start server\n");
	server_thread = new std::thread(t3::init_server);
}

void btn2_callback(float mx, float my)
{
	DEBUG_LOG("Start client\n");
	
}

void initialize_game()
{
	p = t3::compileShader("data/vertex.glsl", "data/fragment.glsl");
	t3::initialize_batch_renderer();
	texture = t3::loadTexture("data/spritesheet.png", GL_TEXTURE0);
	t3::register_receive_callback(&receive_callback);


	start_host_btn.x = position[0] * 2.0f - tile_size;
	start_host_btn.y = position[1]* 2 / 3.0f * 1.0f;
	start_host_btn.width = tile_size;
	start_host_btn.height = tile_size;
	start_host_btn.sprite = { 0, 2 };
	start_host_btn.button_callback = &btn_callback;

	
	start_client_btn.x = position[0] * 2.0f - tile_size;
	start_client_btn.y = position[1] * 2 / 3.0f * 2.0f;
	start_client_btn.width = tile_size;
	start_client_btn.height = tile_size;
	start_client_btn.sprite = { 0, 2 };
	start_client_btn.button_callback = &btn2_callback;
}

PlayerTurn get_player_from_tilestate(TileState state)
{
	if (state == TileState::O_TILE)
		return PlayerTurn::O_PLAYER;


	return PlayerTurn::X_PLAYER;
}

bool check_win_condition_row(int row, TileState state)
{
	for (int i = 0; i < 3; i++)
	{
		if (state != global_game_state.get_tile_state(i + row * 3))
			return false;
	}
	return true;
}

bool check_win_condition_col(int col, TileState state)
{
	for (int i = 0; i < 3; i++)
	{
		if (state != global_game_state.get_tile_state(col + i * 3))
			return false;
	}
	return true;
}

bool check_win_condition_diagonal(bool top_left, TileState state)
{
	int start_x = 0;
	int direction = 1;
	if (!top_left)
	{
		start_x = 2;
		direction = -1;
	}

	for (int i = 0; i < 3; i++)
	{
		if (state != global_game_state.get_tile_state((start_x + direction * i) + i * 3))
			return false;
	}
	return true;
}

void update_win_condition(int tile_nr)
{
	TileState state = global_game_state.get_tile_state(tile_nr);
	int dx = tile_nr % 3;
	int dy = tile_nr / 3;
	bool row = check_win_condition_row(dy, state);
	bool col = check_win_condition_col(dx, state);
	bool diagonal_1 = check_win_condition_diagonal(false, state);
	bool diagonal_2 = check_win_condition_diagonal(true, state);

	if (row || col || diagonal_1 || diagonal_2)
	{
		show_winner = true;
		winner = get_player_from_tilestate(state);
	}
}



bool update_game(Mouse& mouse)
{
	t3::update_button(start_host_btn, mouse);
	t3::update_button(start_client_btn, mouse);
	if (mouse.middle_btn.get_click())
	{
		global_game_state = t3GameState();
		show_winner = false;
	}

	if (!show_winner && mouse.left_btn.get_click())
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
						update_win_condition(tile_nr);
					}
					else if (global_game_state.get_player_turn() == PlayerTurn::X_PLAYER)
					{
						global_game_state.set_tile_state(tile_nr, TileState::X_TILE);
						global_game_state.set_player_turn(PlayerTurn::O_PLAYER);
						update_win_condition(tile_nr);
					}
				}
			}
		}
	}

	t3::sendData(global_game_state.data_buffer, global_game_state.get_buffer_size());

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
			t3::submit_sprite(0, 2, position[0] + tile_size * (x - 1), position[1] + tile_size * (y - 1), tile_size, tile_size);
			t3::set_layer_z(2.0f);

			TileState state = global_game_state.get_tile_state(x + y * 3);
			if (state == TileState::X_TILE)
			{
				t3::submit_sprite(1, 2, position[0] + tile_size * (x - 1), position[1] + tile_size * (y - 1), tile_size, tile_size);
			}
			else if (state == TileState::O_TILE)
			{
				t3::submit_sprite(2, 2, position[0] + tile_size * (x - 1), position[1] + tile_size * (y - 1), tile_size, tile_size);
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

	t3::set_layer_z(1.0f);
	t3::draw_button(start_host_btn);
	t3::draw_button(start_client_btn);


	t3::render_batch();
}


