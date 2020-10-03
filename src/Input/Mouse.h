#pragma once

struct MouseButtonState
{
	bool pressed;
	bool clicked;

	void register_input(bool button_down)
	{
		if (button_down)
		{
			pressed = true;
		}
		else
		{
			pressed = false;
			clicked = false;
		}
	}

	bool get_click()
	{
		if (pressed && !clicked)
		{
			clicked = true;
			return true;
		}
		
		return false;
	}

};

struct Mouse
{
	double x, y;
	MouseButtonState left_btn;
	MouseButtonState middle_btn;
	MouseButtonState right_btn;
};