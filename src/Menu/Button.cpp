#include "Button.h"



void t3::update_button(ImageButton& btn, Mouse& mouse)
{
	float mx = (float)mouse.x;
	float my = (float)mouse.y;
	float btnoffset = btn.width / 2.0f;
	if (mx >= btn.x - btnoffset && mx < btn.x + btnoffset)
	{
		if (my >= btn.y - btnoffset && my < btn.y + btnoffset)
		{
			if (mouse.left_btn.get_click())
			{
				btn.button_callback(mx, my);
			}
		}
	}
}

void t3::draw_button(ImageButton& btn)
{
	t3::submit_sprite(btn.sprite.sprite_offset, btn.sprite.num_sprites_width, btn.x, btn.y, btn.width, btn.height);
}