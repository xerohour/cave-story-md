#include "tables.h"
#include "resources.h"

const background_info_def background_info[BACKGROUND_COUNT] = {
	{ NULL, PAL2, 2, 0, 0 },
	{ &BG_Blue, PAL2, 0, 8, 8 }, // Unused
	{ &BG_Blue, PAL2, 0, 8, 8 }, // Mimiga Village, Grasstown
	{ &BG_Grass, PAL0, 0, 8, 8 },
	{ &BG_Fog, PAL2, 1, 16, 4 },
	{ &BG_Gard, PAL3, 0, 6, 8 }, // Sand Zone Building
	{ &BG_Gray, PAL2, 0, 8, 8 },
	{ &BG_Green, PAL3, 0, 8, 8 },
	{ &BG_Maze, PAL3, 0, 8, 8 },
	{ &BG_Maze, PAL3, 0, 8, 8 },
	{ &BG_Red, PAL2, 0, 4, 4 },
	{ &BG_Red, PAL2, 0, 4, 4 },
	{ &BG_Green, PAL3, 0, 8, 8 }, // Sand Zone
	{ &BG_Eggs, PAL2, 0, 8, 8 }, // Egg Corridor
};
