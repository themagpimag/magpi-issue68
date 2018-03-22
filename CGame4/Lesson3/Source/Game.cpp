

#include "Game.h"
#include "SimpleObj.h"

Game::Game() 
{
	OGLES.Init(); // make sure we initialise the OGL instance we called OGLES
}; 

Game::~Game() {}; // We should close down some things here but can do it later.

void Game::Update()
{
	
// start by creating some SimpleObj's and initialising some important variables
	
	SimpleObj Bobby1; // create a Simple Object with the class name Bobby1
	SimpleObj Bobby2;
//Give our Bobbies a name	
	Bobby1.m_MyName = "Bobby1"; // Give Bobby 1 his name
	Bobby2.m_MyName = "Bobby2"; // Give Bobby 2 his name
	
// they do a draw, so they need to know where the Program Object is which is in OGLES
	Bobby1.ProgramObject = OGLES.programObject;
	Bobby2.ProgramObject = OGLES.programObject; // set up the program object 
	
// now we set them up with pixel values rather than ratio's from lesson 3	
	Bobby1.Xpos = 10.75; // pixel values can be floats, we can dictate the precision we want
	Bobby2.Xpos =  1000.76;	
	Bobby1.Ypos = 700;
	Bobby2.Ypos = 100;

// now we will do a loop
	for (int i = 0; i < 1000; i++)
	{
		glClear(GL_COLOR_BUFFER_BIT); // clear the screen this time so we can see our black boxes
		Bobby1.Update(); // do Bobby1's update and draw
		Bobby2.Update(); // do Bobby2's update and draw
	
// the Bobbies are drawn, now show them on the screen
		eglSwapBuffers(OGLES.state.display, OGLES.state.surface);

	} // this is the end of the for loop, the i will be incremented and tested to see if its still <1000, if so, it repeats the loop with i now +1
	return; 
};