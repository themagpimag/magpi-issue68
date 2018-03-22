#pragma once
#include <string> // we're going to use strings so need the header
#include "OGL.h"
#include "stb_image.h"
class SimpleObj
{
public:
	

	
	SimpleObj(); //standard constructor
	~SimpleObj(); //standard destructor

//list the functions we want to have (called methods in C++)	
	void Update();
	void Draw();
// list the variables we want our instances to have (called members in C++)	
	float Xpos, Ypos;
	float Xspeed, Yspeed;
	
	float Scale;	// this is just a fun variable since our actual texture is quite large, and already shrunk but the vertex offsets
	std::string m_MyName ;
	
	GLint ProgramObject; //Objects need this 
	GLint ThisTilesTextureID;		// each tile now has its own
};