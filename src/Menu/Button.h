#ifndef BUTTON_HEADER
#define BUTTON_HEADER

#include "Input/Mouse.h"
#include "Renderer/batch_renderer.h"

namespace t3
{

	struct ImageButton
	{
		float x, y;
		float width, height;

		Sprite sprite;

		void(*button_callback)(float, float);
	};

	void update_button(ImageButton& btn, Mouse& mouse);

	void draw_button(ImageButton& btn);
}

#endif
