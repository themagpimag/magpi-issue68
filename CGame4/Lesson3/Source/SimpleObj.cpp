#include <iostream> // we need this to output to console this file contains cout
#include "SimpleObj.h"

using namespace std;

// disclaimer, the MagPi logo is actually very large, so we've scaled down the size of the vertices
// to make it a managable size on screen, normally we would want our default value to be 1.0f or in the case of
// a proper quad 0.5 since the centre reference point, would be 0.5 units from each corner, making corner to corner 1.0f




//we are going to draw a plain no texture quad (2 tris) we need a list of offsets we will hold in array called QuadVertices
// arrays are fully discussed in lesson 4 article
static const GLfloat QuadVertices[] =  
{  // Square vertices with no texture
	0.1, 0.1,0.0, -0.1,0.1,0.0, 0.1,-0.1,0.0, /*now 2nd tri*/ -0.1,0.1,0.0, -0.1,-0.1,0.0,	0.1, -0.1,0.0,	 
};


static const GLfloat QuadVerticesWithTex[] = 
{	// square vertices with texture coordinates added, so we have 3 floats of vertex, 
	// then 2 floats of texture coords (called uv coords), this is called interlacing data 
	0.1,
	0.1,
	0.0, 
	1.0, 0.0, // texture coordinate
	-0.1,
	0.1,
	0.0, 
	0.0, 0.0, // texture coordinate
	0.1,
	-0.1, 
	0.0,
	1.0, 1.0, // texture coordinate
	
//2nd tri	
	-0.1, 
	0.1,
	0.0,
	0.0, 0.0, // texture coordinate
	-0.1,
	-0.1,
	0.0,
	0.0, 1.0, // texture coordinate
	0.1,
	-0.1,
	0.0,	
	1.0, 1.0 // texture coordinate
};


SimpleObj::SimpleObj() 
{
	Xspeed = 4.0f; // lets make our constructor set some base values, we don't need to keep them though, they can be reset by the creator code.
	Yspeed = 11.0f;

} 

SimpleObj::~SimpleObj() {} // no code yet in the destructor

/************************************************************************ 
Update has to do some work, in this case we'll print out some info on our position and
move our object this time though we're doing it in pixel space with assumed screen
size of 1024x768
************************************************************************/

void SimpleObj::Update()
{
	
// lets move our object	by just adding the speed value and this time we're using pixel values
// strictly speaking we should not use hard numbers like 1024 or 768, its much better to have a #define somewhere
// that sets SCREENWIDTH  to 1024 and SCREENHEIGHT to 768, this could be a gobal define, or a more C++ like method that returns 
// the size we'll address that soon when we rewrite this code to be cleaner and more stable	
	
	
	Xpos += Xspeed;
	Ypos += Yspeed;
	if (Xspeed > 0)
	{
		if (Xpos > 1024) Xspeed = -Xspeed; //<<< try changing this value, to something >0 but < 1024 to see the impact it has and how much easier it is to visualise coordinates
	}
	else
	{
		if (Xpos < 0) Xspeed = -Xspeed;
	}
// here's the same concept applied to the Y coordinate	
	if (Yspeed > 0)
	{
		if (Ypos > 768) Yspeed = -Yspeed;
	}
	else
	{
		if (Ypos < 0) Yspeed = -Yspeed;
	}
	
	
	
		
// now lets draw our object
	SimpleObj::Draw(); // we don't really need to prefix this, this method is in the SimpleObj class, so it can just type Draw(); I only do it for emphasis.
}


/************************************************************************
The Draw system needs a bit of setting up
The shader will draw what we tell it but we have to send it some
************************************************************************/
void SimpleObj::Draw() 
{
// its important we tell our draw system what Program Object (PO) we are using, intially its the same one as in the OGL::OpenGLES instance but that may change	
	glUseProgram(ProgramObject); // this was set up when we created the instance of SimpleObj

	// Get the attribute locations which are the offset buffers, we'll store these in values we can call handles.
	GLint positionLoc = glGetAttribLocation(ProgramObject, "a_position");
	GLint texCoordLoc = glGetAttribLocation(ProgramObject, "a_texCoord");
	
	// if we want to position it somewhere we need to ask for the handle for the position uniform
	GLint positionUniformLoc = glGetUniformLocation(ProgramObject, "u_position");	
	GLint samplerLoc = glGetUniformLocation(ProgramObject, "s_texture");

	
	
/***********************************************************************
This is a rather hacky way to send position data, we'll refine this soon!	
In this primitive shader, our screen goes from -1 to +1 in the X and Y position
we need to pass our new Xpos and Ypos to the shader in 2 steps
************************************************************************/
	
// 1st create a dummy array	of 4 values
	float Pos[4] = { 0.0f, 0.0f, 0.0f,0.0f };

// now load our X and Y in to this, we don't care about the last 2 entries they are set to 0
// we are still sending ratios to our shader, but now we're converting them before we send them.
// we could do this in the shader, and Lesson 4 will show you a rather hacky way to do that, but
// just so we can understand what is happening we will do it here. Notice the Screen height and width
// are halved to account for the -1 to +1 range expected.
	
	Pos[0] =	((Xpos-512) / 512); // convert the x cooordinate into a clip space ratio
	Pos[1] =	((Ypos-384) / 384); // same for the y
	
// We can now transfer this to the uniform u_position value in the shader which we have a handle for in positionUniformLoc
	glUniform4fv(positionUniformLoc, 1, &Pos[0]); 
	
// So we've told the shader to use our Xpos and Ypos values as the centre point, now tell it to use the array of vertices in quadVertices
	glVertexAttribPointer	(
		positionLoc, 
		3,  // we send 3 values at a time (x,y,z offsets)
		GL_FLOAT,  // they are floats
		GL_FALSE,  // they are not normalised(explained later)
		5*sizeof(GLfloat), //0 means no step needed, just go to the next set, but here we skip 5 floats
		QuadVerticesWithTex //QuadVertices
		); 
	
	// Load the texture coordinate
	glVertexAttribPointer(texCoordLoc,
		2,
		GL_FLOAT,
		GL_FALSE,
		5 * sizeof(GLfloat),
		&QuadVerticesWithTex[3]); // << notice that & we'll talk about that and * symbols in lesson 5
	
	
	glEnableVertexAttribArray(positionLoc); // when we enable it, the shader can now use it
	glEnableVertexAttribArray(texCoordLoc); // when we enable it, the shader can now use it
	
	glActiveTexture(GL_TEXTURE0);
// now its been set up, tell it to draw 6 vertices which make up a square
	glDrawArrays(GL_TRIANGLES, 0, 6);

// we could have this after each GL function but having one here will tell us if there was an error	
	if (glGetError() != GL_NO_ERROR) printf("OpenGLES2.0 reports an error!");	
	
} 