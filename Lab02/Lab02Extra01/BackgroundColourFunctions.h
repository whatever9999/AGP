/*
*  Dominique Russell 06-10-2021
*  This file contains functions to change the colour of a viewport's background
*/

#pragma once

#include <windows.h>

class BackgroundColourFunctions
{
	public:
		void ShiftOverTime(float colour[4]);
		void MouseClick(float colour[4], POINT mouse_pos, float screen_size[2]);
};