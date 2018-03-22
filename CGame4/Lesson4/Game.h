#pragma once
#include "OGL.h"

class Game
{
public:
	Game(); //standard constructor
	~Game(); //standard destructor

		//list the functions we want to have (called methods in C++)	
	void Update();
	
	OGL	OGLES;
	
	//this is just an example of how we can now create an example of our new type, we could also initialise
	//its contents here but its best to do that a different way. 
		simpleVec2	TestVector2;  
	
	
};

