#ifndef GAME_HEADER
#define GAME_HEADER
#include "Input/Mouse.h"

#define PLAYER_X_BIT_ID 0x1
#define PLAYER_O_BIT_ID 0x0


enum class PlayerTurn
{
	O_PLAYER,
	X_PLAYER,
};

enum class TileState
{
	O_TILE,
	X_TILE,
	EMPTY_TILE,
	PADDING_TILE,
};

struct t3GameState
{
	//2bits per tile, 9 tiles, row by row, top to bottom, 2 * 9 = 18 bits => 3bytes of data (24bits of data).
	//bits 1-18 tile data	00: O | 11: X	 |	01: empty | (10: padding, this on for robustness.) 
	//bit 19-20 player turn,	00: O | 11: X
	//bits 21-24 unused for now. 10: padding
	char data_buffer[3]; //((data_buffer[0] >> 7) & 0x1) is bit 1, ((data_buffer[0] >> 6) & 0x1) is bit 2.

	t3GameState();

	inline const int get_bits_per_tile() const
	{
		return 2;
	}

	inline const int get_num_tiles() const
	{
		return 9;
	}

	const TileState get_tile_state(int tile_nr) const;
	const PlayerTurn get_player_turn() const;


	const bool check_place_piece_on_tile(PlayerTurn player, int tile);

	void set_tile_state(int tile_nr, TileState state);
	void set_player_turn(PlayerTurn state);

};

void server_receive_callback(char* data, int size);
void client_receive_callback(char* data, int size);
void initialize_game();
bool update_game(Mouse& mouse);
void render_game();


#endif