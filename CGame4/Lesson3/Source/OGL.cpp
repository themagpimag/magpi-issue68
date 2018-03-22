#include  "OGL.h"
#include <assert.h> // assert is a way of making sure that things are what we expect and if not it will stop and tell us.
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
using namespace std;

/*  For beginners this might be a bit too much, but just include the file for now and use the OGL.h file when you need to use
	OpenGLES2.0 instructions. 
	
	In order to set up a screen, we will need to ask the video system to create a couple of buffer areas with 
	the right type of pixels and also indicate some of the hardware features we want to switch on. This is done using EGL libs.
	
	The process of setting up a graphic screen is a little technical but mainly we must create an EGLscreen
	which itself is optimised on RaspPi to use a special format our Broadcom GPU givs us for its display, all handled in 
	the bcm_host.h file so we can forget about it, but we do need to tell EGL we are using DISPMANX systems
	
	Once we have our EGL/DISMPANX surface and displays systems it means OpenGLES2.0 has something to work with and we can
	draw things using our shaders.
	For convenience we will set up a simple default shader, but normally we would have a few different ones for different
	effects.
	
	You can totally ignore the contents of this file until you feel you want to explore how to set things up, but do make 
	sure you have an #include OGL.h in any class that wants to play with OpenGL and that you double check that you create 
	an instance of OGL and call its init() method to set it all up of us.
	
	Also note this file makes use of fprintf instead of cout, its just a tidier way to format printed text.
	
*/

OGL::OGL() {};
OGL::~OGL() {};

// this is a simple definition of an attribute list to create an EGL window, there are many variations we could have
static const EGLint attribute_list[] =
{
// these 1st 4, let us define the size of our colour componants and from that the size of a pixel. 
// In this case 32bits made up of 8xR, 8xG,8xB and 8xA 
	EGL_RED_SIZE,
	8,
	EGL_GREEN_SIZE,
	8,
	EGL_BLUE_SIZE,
	8,
	EGL_ALPHA_SIZE,
	8,
	EGL_SAMPLE_BUFFERS,
	1, // if you want anti alias at a slight fps cost
	EGL_SAMPLES,
	4,  //keep these 2 lines
	// These lines set up some basic internal features that help 
	EGL_DEPTH_SIZE,
	1,
	//	These lines tell EGL how the surface is made up.
	EGL_SURFACE_TYPE,
	EGL_WINDOW_BIT,
	EGL_NONE
};	

// we will also need to tell our context what we want.
static const EGLint context_attributes[] =
{
	EGL_CONTEXT_CLIENT_VERSION,
	2,
	EGL_NONE
};


/* We've added a simple routine to create textures
 Create a texture with width and height
*/
GLuint OGL::CreateTexture2D(int width, int height,  char* TheData)
{
	// Texture handle
	GLuint textureId;
	// Set the alignment
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	// Generate a texture object
	glGenTextures(1, &textureId);
	// Bind the texture object
	glBindTexture(GL_TEXTURE_2D, textureId);
	// set it up
	glTexImage2D(GL_TEXTURE_2D,
		0,
		GL_RGBA,
		width,
		height,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		TheData);
	
	if (glGetError() != GL_NO_ERROR) printf("We got an error"); // its a good idea to test for errors.

	// Set the filtering mode
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	return textureId;
}

// This will set up the Broadcomm GPU version of an EGL display,

void OGL::init_EGL(Target_State *state, int width, int height)
{
	
		
	std::cout << "init openGLES started\n";

	
// first set up some local variables we will need to work with	
	int32_t success = 0;
	EGLBoolean result;
	EGLint num_config;

	DISPMANX_ELEMENT_HANDLE_T dispman_element;
	DISPMANX_DISPLAY_HANDLE_T dispman_display;
	DISPMANX_UPDATE_HANDLE_T dispman_update;
	VC_RECT_T dst_rect;
	VC_RECT_T src_rect;

	EGLConfig config;
	
// now lets do the work and set things up
	memset(state, 0, sizeof(Target_State)); // we need to make sure the state is clear

// get an EGL display connection
	state->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

// initialize the EGL display connection
	result = eglInitialize(state->display, NULL, NULL);
	assert(EGL_FALSE != result); // just in case, we should test it.
	
// get an appropriate EGL frame buffer configuration
	result = eglChooseConfig(state->display, attribute_list, &config, 1, &num_config);
	assert(EGL_FALSE != result);

// get an appropriate EGL frame buffer configuration
	result = eglBindAPI(EGL_OPENGL_ES_API);
	assert(EGL_FALSE != result);


// create an EGL rendering context
	state->context = eglCreateContext(state->display, config, EGL_NO_CONTEXT, context_attributes);
	assert(state->context != EGL_NO_CONTEXT);

// create an EGL window surface
	success = graphics_get_display_size(0 /* LCD */, &state->width, &state->height);
	assert(success >= 0);

// ok now that the EGL is all set up, lets give it some important info so that our display manager can make a window
	
	state->width = width;
	state->height = height;

	dst_rect.x = 160; // gives us a slight offset away from the top left corner
	dst_rect.y = 160;
	dst_rect.width = width;
	dst_rect.height = height;

	src_rect.x = 16;
	src_rect.y = 16;
	src_rect.width = width << 16;
	src_rect.height = height << 16;        
	
// now tell the hardware to make the display open to us
	dispman_display = vc_dispmanx_display_open(0 /* LCD */);
	dispman_update = vc_dispmanx_update_start(0);

	dispman_element = 	vc_dispmanx_element_add(dispman_update,
		dispman_display,
		0/*layer*/,
		&dst_rect,
		0/*src*/,
		&src_rect,
		DISPMANX_PROTECTION_NONE, 
		0 /*alpha*/,
		0/*clamp*/,
		(DISPMANX_TRANSFORM_T) 0/*transform*/);
// tell our Target_stat structure what we have
	
	state->nativewindow.element = dispman_element;
	state->nativewindow.width = state->width;
	state->nativewindow.height = state->height;
	vc_dispmanx_update_submit_sync(dispman_update);
// now that the display is ready, we have to have a surgace our OpenGLES2.0 systems will draw to, 
	
	state->surface = eglCreateWindowSurface(state->display, config, &(state->nativewindow), NULL);
	assert(state->surface != EGL_NO_SURFACE);
// connect the context to the surface
	result = eglMakeCurrent(state->display, state->surface, state->surface, state->context);
	assert(EGL_FALSE != result);
	
// if we got through all that ok, we have a display screen ready for us	
	std::cout << "init openGLES finished\n";

}



/*
 This is a fairly standard Shader loader and generator
 enter with the type of shader needed and a pointer to where the code text is
*/
GLuint OGL::LoadShader(GLenum type, const char *shaderSrc)
{	
// see if we can allcoate a new shader
	GLuint TheShader = glCreateShader(type);
	if (TheShader == 0) return FALSE; // return FALSE (0) to indicate we failed to allocate this shader.
// note...allocating a shader is not the same as generating one. It just means a space was made available
// and our next step is to put the code in there.

	// pass the source
	glShaderSource(TheShader, 1, &shaderSrc, NULL);
	
	// Compile the shader
	glCompileShader(TheShader);

	GLint  IsCompiled; // we need a value to be set if we fail or succeed.
	
// Compiling can result in errors, so we need to make sure we find out if it compiled, 
// but also its useful to report the error
	glGetShaderiv(TheShader, GL_COMPILE_STATUS, &IsCompiled);  // if compile works, TheShader will have a non 0 value 

// a compiler fail means we need to fix the error manually, so output the compiler errors to help us	
	if (!IsCompiled)
	{
		GLint RetinfoLen = 0; 
		glGetShaderiv(TheShader, GL_INFO_LOG_LENGTH, &RetinfoLen);
		if (RetinfoLen > 1)
		{ // standard output for errors
			char* infoLog = (char*) malloc(sizeof(char) * RetinfoLen);
			glGetShaderInfoLog(TheShader, RetinfoLen, NULL, infoLog);
			fprintf(stderr, "Error compiling this shader:\n%s\n", infoLog); // this is a useful formatting print system
			free(infoLog);
		}
		glDeleteShader(TheShader); // we did allocate a shader, but since we failed to compile we need to remove it.
		return FALSE; // indicate our failure to make the shader
	}
	return TheShader; // if we succeeded we will have a valuer in TheShader, return it.
}

///
// Initialize open GL and create our shaders
//
int OGL::Init()
{
// once the BCM has been initialsed and EGL/DISPMANX is up and runnng, all re really need now is some shaders to work with
// since OpenGLES can't actually draw without them, so we will set up a very simple default pair of shaders, and turn them
// into a Program Object which we will set as the default
	

	bcm_host_init(); // make sure we have set up our Broadcom
	
	init_EGL(&state, 1024, 768); // this will set up the EGL stuff, its complex so put in a seperate method
	
	// HERE we will load in our graphic image to play with and convert it into a texture	
	int Grwidth, Grheight, comp; // these are variables we will use to get info
	
// this is a strange instruction with * and & symbols which we'll discuss soon, but what it does is simple
// it loads a graphic image into memory and stores its width and height into variables Grwidth and Grheight, don't worry about comp or the 4, but they are needed
	unsigned char *data = stbi_load("../images/MagPicomp.png", &Grwidth, &Grheight, &comp, 4); // ask it to load 4 componants since its rgba

	//now its loaded we have to create a texture, which will have a "handle" ID that can be stored, we have added a nice function to make this easy		
	 textureID = CreateTexture2D(Grwidth, Grheight, (char*) data); //just pass the width and height of the graphic, and where it is located and we can make a texture
	
/*
 *
 *we're done making the texture but something to think about,  where is the data we made it from? We'll find out soon.
 *
 **/ 
	
	

// we'll need a base colour to clear the screen this sets it up here (we can change it anytime), our test graphic is very black 
// so we'll go for a magenta colour so we can see it
	glClearColor(1.0f, 0.0f, 1.0f, 1.0f);

	
// this is our shader code, contained in an array of text
// there are much better ways to do this, we will cover later
// This v2 shader code can now handle textures but its doing similar things


	
	GLbyte vShaderStr[] =
		"attribute vec4 a_position;\n"
		"uniform vec4 u_position;\n"
		"attribute vec2 a_texCoord;\n"
		"varying vec2 v_texCoord;\n"
		"void main()\n"
		"{gl_Position = a_position + u_position; \n"
		" v_texCoord=a_texCoord;"
		"}\n";

	GLbyte fShaderStr[] =
			"precision mediump float;\n"
		"varying vec2 v_texCoord;\n"
		"uniform sampler2D s_texture;\n"
		"void main()\n"
	    "{\n"
        "gl_FragColor = texture2D( s_texture, v_texCoord );\n"
        "}\n";
		
     
		
// shaders next, lets create variables to hold these	
	GLuint vertexShader, fragmentShader; // we need some variables


// Load and compile the vertex/fragment shaders
	vertexShader = LoadShader(GL_VERTEX_SHADER, (char*)vShaderStr);
	fragmentShader = LoadShader(GL_FRAGMENT_SHADER, (char*)fShaderStr);

	if (vertexShader == 0 || fragmentShader == 0) // test if they both have non 0 values
	{
		return FALSE; // one of them is false, the report will have been printed, and we cannot continue
	}
	
// Create the program object	
	programObject = glCreateProgram();
	
	if (programObject == 0) 	
	{
		cout << "Unable to create our Program Object " << endl;		
		return FALSE; // there was a failure here
	
	}
	
// now we have teh V and F shaders  attach them to the progam object
	glAttachShader(programObject, vertexShader);
	glAttachShader(programObject, fragmentShader);
	
// Link the program
	glLinkProgram(programObject);
// Check the link status
	GLint AreTheylinked;
	
	glGetProgramiv(programObject, GL_LINK_STATUS, &AreTheylinked);
	if (!AreTheylinked)
	{
		GLint RetinfoLen = 0;
// check and report any errors
		glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &RetinfoLen);
		if (RetinfoLen > 1)
		{
			GLchar* infoLog = (GLchar*)malloc(sizeof(char) * RetinfoLen);
			glGetProgramInfoLog(programObject, RetinfoLen, NULL, infoLog);
			fprintf(stderr, "Error linking program:\n%s\n", infoLog);
			free(infoLog);
		}
		glDeleteProgram(programObject);
		return FALSE;
	}

	glUseProgram(programObject); // we only plan to use 1 program object so tell openGL to use this one
	return TRUE;
}

