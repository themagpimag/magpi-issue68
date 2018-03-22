#include <iostream> // we need this to output to console this file contains cout
#include "SimpleObj.h"

using namespace std;

//we are going to draw a triangle so we need a list of offsets we will hold in array called TriVertices


static const GLfloat TriVertices[] =
{  // Square vertices with no texture
	1, 1,0, -1,1,0, 1,-1,0, /*now 2nd tri*/ -1,1,0, -1,-1,0,	1, -1,0,
};


// we are going to reflect the size of our textures, so we need a square that has size 1x1
// since the distance from the centre to the corners is 0.5x and 0.5y, two tringles will use offsets of 0.5f
static const GLfloat TriVerticesWithTex[] =
{
	 // square vertices with texture coordinates added, so we have 3 floats of vertex,
	// then 2 floats of texture coords (called uv coords), this is called interlacing data
	0.5f,
	0.5f,
	0,
	1.0f,
	0.0f,
	  // texture coordinate
	- 0.5f,
	0.5f,
	0,
	0.0f,
	0.0f,
	  // texture coordinate
	0.5f,
	-0.5f,
	0,
	1.0f,
	1.0f,
	 // texture coordinate

//2nd tri
	- 0.5f,
	0.5f,
	0,
	0.0f,
	0.0f,
	  // texture coordinate
	-0.5f,
	-0.5f,
	0,
	0.0f,
	1.0f,
	 // texture coordinate
	0.5f,
	-0.5f,
	0,
	1.0f,
	1.0f // texture coordinate
};

SimpleObj::SimpleObj()
{
	Xspeed = 0.0f; // lets make our construc
	Yspeed = 0.0f;
	Scale = 16.0; // this tells us how many pixels to make our texture square, we are using 16x16 so 16 give us a one to one ratio, but we can scale up or down
}


SimpleObj::~SimpleObj() {} // no code yet in the destructor

/************************************************************************
Update has to do some work, in this case we'll print out some info on our position and
move our object in pixel space, and do a few tests to look interesting
************************************************************************/

void SimpleObj::Update()
{		// this is a tile, so it does not really need any code, its set up when it was made

		SimpleObj::Draw();   // just draw at the xy coordinates


}


/************************************************************************
The Draw system needs a bit of setting up
The shader will draw what we tell it but we have to send it some
************************************************************************/
void SimpleObj::Draw()
{
// its important we tell our draw system what Program Object (PO) we are using, intially its the same one as in the OGL::OpenGLES instance but that may change
// we set each instance up with a copy of the OGL ProgramObject
	glUseProgram(ProgramObject);

// each instance has its own texture now so lets "bind it" which means we tell the GPU that this is the only texture it should care about for now
	glBindTexture(GL_TEXTURE_2D, ThisTilesTextureID);

// we have to get the correct handles from the GPU so we can tell what attributes and uniforms we are trying to set up.

	// Get the attribute locations, we'll store these in values we can call handles.
	GLint positionLoc = glGetAttribLocation(ProgramObject, "a_position");
	GLint texCoordLoc = glGetAttribLocation(ProgramObject, "a_texCoord");

	// if we want to position it somewhere we need to ask for the handle for the position uniform and other unfirms
	GLint positionUniformLoc = glGetUniformLocation(ProgramObject, "u_position");	 // << you can see we ask for the handle of the variable in our shader called u_position
	GLint SamplerLoc = glGetUniformLocation(ProgramObject, "s_texture");
	GLint ScreenCoordLoc = glGetUniformLocation(ProgramObject, "u_Screensize");
	GLint ScaleLoc= glGetUniformLocation(ProgramObject, "u_Scale");




/***********************************************************************
This is still a rather hacky way to send position data, but its getting
better as we pass more reponsibility to the shader
We could improve on this quite a bit if our position data itself was
a simpleVec2, but we are going to do a better system later so lets not expand
too much on this idea beyond the fact we can create new types
************************************************************************/

// Lets use a couple of simpleVec2's, its still not an elegant way to do this but it demonstrates some ideas we can use elsewhere

	simpleVec2 testingVec = { Xpos, Ypos };
	simpleVec2 ScreenData = { 512, 384 };	// we only need half the screen size which is currently a fixed amount


// 2nd, we can now transfer this to the u_position value in the shader which we have a handle for in positionUniformLoc
	glUniform2fv(positionUniformLoc, 1, &testingVec.xValue); //<< notice the format, we are passing 2 float values (2fV) starting with the 1st which is the xValue
// now transfer our screen sizes as well
	glUniform2fv(ScreenCoordLoc, 1, &ScreenData.xValue);   // pass our half screen sizes by pointing to address of the 1st value in the struct
// we also need to transfer scale, this is a simpler format, 1 float, just tell it the value to send
	glUniform1f(ScaleLoc, Scale);
	glUniform1i(SamplerLoc, 0); // send an int to the Sampler to indicate texture 0


// So we've told the shader to use our Xpos and Ypos values as the centre point, now tell it to use the array of vertices in TriVertices
	glVertexAttribPointer	(
		positionLoc,
		3,  // we send 3 values at a time (x,y,z offsets)
		GL_FLOAT,  // they are floats
		GL_FALSE,  // then are not normalised(explained later)
		5*sizeof(GLfloat), //0 means no step needed, just go to the next set, here we have 5 valyes 3 vertices and 2 uv coords.
		TriVerticesWithTex //TriVertices
		);

	// Load the texture coordinate
	glVertexAttribPointer(texCoordLoc,
		2,
		GL_FLOAT,
		GL_FALSE,
		5 * sizeof(GLfloat),
		&TriVerticesWithTex[3]);

// we set up the attributes with start, type, size and stride, so they are ready to go.
	glEnableVertexAttribArray(positionLoc); // when we enable it, the shader can now use it
	glEnableVertexAttribArray(texCoordLoc); // when we enable it, the shader can now use it

	glActiveTexture(GL_TEXTURE0);
// now its been set up, tell it to draw 6 vertices which make up a square
	glDrawArrays(GL_TRIANGLES, 0, 6);

// we could have this after each GL function but having one here will tell us if there was an error
	if (glGetError() != GL_NO_ERROR) printf("OpenGLES2.0 reports an error!");

}
