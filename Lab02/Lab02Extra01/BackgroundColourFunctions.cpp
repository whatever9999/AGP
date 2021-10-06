/*
*  Dominique Russell 06-10-2021
*  This file contains functions to change the colour of a viewport's background
*/

#include "BackgroundColourFunctions.h"
// Include for pauses between colour changes
#include <thread>
// Include for math functions
#include <cmath> 

void BackgroundColourFunctions::ShiftOverTime(float colour[4])
{
	// Loop through each element of the g_clear_colour array
	for (int i = 0; i < sizeof(colour) / sizeof(colour[0]); i++)
	{
		// Add a random amount from 0.0f to 1.0f to the new value for this element
		float new_value = colour[i];
		float add_random = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		new_value += add_random;

		// If we're over 1 loop back through the 0.0f to 1.0f range
		if (new_value > 1.0f)
		{
			new_value = new_value -= 1.0f;
		}

		// Set the new colour
		colour[i] = new_value;
	}

	// Wait before changing colour again
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

void BackgroundColourFunctions::MouseClick(float colour[4], POINT mouse_pos, float screen_size[2])
{
	// Use mouse pos to determine value
	double alpha_strength = sqrt(pow(mouse_pos.x, 2.0) + pow(mouse_pos.y, 2.0));
	double max_screen_strength = sqrt(pow(screen_size[0], 2.0) + pow(screen_size[1], 2.0));

	// Normalise for a value between 0.0f and 1.0f
	double normalised_alpha = alpha_strength / max_screen_strength;

	// Loop through each element of the g_clear_colour array
	for (int i = 0; i < sizeof(colour) / sizeof(colour[0]); i++)
	{
		//colour[0] = normalised_alpha;
		colour[1] = normalised_alpha;
		colour[2] = normalised_alpha;
		//colour[3] = normalised_alpha;

		colour[0] = 0.0f;
		//colour[1] = 0.0f;
		//colour[2] = 0.0f;
		colour[3] = 1.0f;
	}
}