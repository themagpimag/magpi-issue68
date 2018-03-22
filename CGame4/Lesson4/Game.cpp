

#include "Game.h"
#include "SimpleObj.h"

Game::Game()
{
	OGLES.Init(); // make sure we initialise the OGL instance we called OGLES
};

Game::~Game() {}; // We should close down some things here but can do it later.


void Game::Update()
{

// strictly speaking this is an init and update, but this update never ends, we will clean up later

// this time instead of
char* TileFiles[] =  // here's a good example of an array, being used to store text, which is actually stored somewhere else in memory so we point * at it!
	{
		(char*)"images/tile0.png", // take care to note that runing in the IDE, the root dir of the project is the root this loads from.
		(char*)"images/tile1.png",
		(char*)"images/tile2.png",
		(char*)"images/tile3.png",
		(char*)"images/tile4.png",
		(char*)"images/tile5.png",
		(char*)"images/tile6.png",
		(char*)"images/tile7.png",
		(char*)"images/tile8.png",
		(char*)"images/tile9.png",
		(char*)"images/tile10.png",
		(char*)"images/tile11.png",
		(char*)"images/tile12.png",
		(char*)"images/tile13.png",
		(char*)"images/tile14.png",
		(char*)"images/tile15.png",
	};
// here's another useful array
	GLint TileIDs[16]; // define an array to hold the id's we get


	// lets turn these all these graphic files into textures
	for(int i = 0 ; i < 16; i++)
	{
		int Grwidth, Grheight, comp;
		unsigned char* data = stbi_load(TileFiles[i], &Grwidth, &Grheight, &comp, 4);    // ask it to load 4 componants since its rgba
		//now its loaded we have to create a texture, which will have a "handle" ID that can be stored, we have added a nice function to make this easy
		TileIDs[i] = this->OGLES.CreateTexture2D(Grwidth, Grwidth, (char*) data);     //just pass the width and height of the graphic, and where it is located and we can make a texture
		free(data); // when we load an image, it goes into memory, making it a texture copies it to GPU memory, but we need to remove the CPU memory before to load another or it causes a leak
	}

	// now make a 2Dgrid, this is just a small test one.
		int PlayField[10][10] =
	{
		{2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
		{2, 0, 0, 0, 0, 0, 0, 0, 0, 2},
		{2, 0, 0, 0, 0, 0, 0, 0, 0, 2},
		{2, 0, 0, 0, 0, 0, 0, 0, 0, 2},
		{2, 0, 0, 0, 8, 8, 8, 8, 14, 2},
		{2, 0, 0, 0, 0, 0, 0, 0, 14, 2},
		{2, 0, 0, 0, 0, 0, 0, 0, 14, 2},
		{2, 0, 0, 0, 0, 0, 0, 0, 14, 2},
		{2, 9, 9, 9, 9, 10, 0, 0, 14, 2},
		{4, 4, 4, 4, 4, 4, 4, 4, 4, 4},
	};

	// we need to make an instance of an SimpleObj for each tile, there are 100 (10x10) lets do a nested loop to create them
	SimpleObj	MyTiles[100]; // This will create and store our SimpleObjs in this array (there are better ways which we will discover)

	int		index = 0; // we need to keep an index counter
	for(int Row = 0 ; Row < 10 ; Row++) // this is an outer loop we will use the Row variable to go down the grid
	{
		for (int Column = 0; Column < 10; Column++) // this is an inner loop and will go along the grid
		{
			MyTiles[index].m_MyName = "I am a generic tile"; // we can still
			GLint WhatTile = PlayField[Row][Column]; // what tile is it?
			MyTiles[index].ThisTilesTextureID = TileIDs[WhatTile];
			MyTiles[index].Xpos = Column * 16+8+200; // the tiles are 16 pixels apart in X and Y lets move them away from the edge 200,200 to see them
			MyTiles[index].Ypos = 768-((Row * 16)+8+200); // <<woahhh.. whats this? See below
			MyTiles[index].ProgramObject = OGLES.programObject;
			// now its all set up, just increment the index
			index++; // we will now set up the next MyTile
		}
	}

/*what was this 768-((Row*16)+8+200)
 *
 *Its just becuase our Screen system is based on having 0,0 at the bottom left of the screen
 *but the grid is read from the top down, so if we didn't flip it like this we'd draw the grid upside down
 *We'll have to account for that whenever we use a grid.
 *the +8 is because our refernce point for the tile is its centre, and we want its edge to move over half the tile width
 *the 200 is just to take it away from the edge. When we use a full size map we won't need that offset
 *
 *We could increase the size of our tiles when we
 **/

// now we will do a loop to run 1000 cyles to see the result
	for (int i = 0; i < 1000; i++)
	{
		glClear(GL_COLOR_BUFFER_BIT); // clear the screen this time so we can see our black boxes
// and now lets print our 100 tiles which should show our grid,
		for(int Tiles = 0 ; Tiles < 100 ; Tiles++)
		{
			MyTiles[Tiles].Update(); // not only can we access the data of a class we can call its methods
		}

		eglSwapBuffers(OGLES.state.display, OGLES.state.surface);

	}

// This isn't a very good bit of coding, its rather slow to update, and we are repeating a lot of graphics, but we will
// make this more effective next time.

	return;
};
