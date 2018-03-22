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
	
};

