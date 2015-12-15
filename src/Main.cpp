#include "Common.h"
#include "Graphics.h"
#include "Vertices.h"
#include "Shader.h"
#include "Texture.h"
#include "Mesh.h"
#include "FBXLoader.h"
#include "FileSystem.h"
#include "GameObject.h"

//mouse movement
int mouseX, mouseY;
float vAngle = 0;
float hAngle = 3.14f;
float mSpeed = 0.01f;
float speed = 2;
vec3 rightVector;

//matrices
mat4 viewMatrix;
mat4 projMatrix;

mat4 MVPMatrix;

vector<shared_ptr<GameObject> > gameObjects;
GLuint currentShaderProgam = 0;

vec4 ambientLightColour = vec4(1.0f, 1.0f, 1.0f, 1.0f);
vec4 diffuseLightColour = vec4(1.0f, 1.0f, 1.0f, 1.0f);
vec4 specularLightColour = vec4(1.0f, 1.0f, 1.0f, 1.0f);
float specularPower = 25.0f;

vec3 lightDirection = vec3(0.0f, 0.0f, 1.0f);
vec3 cameraPosition = vec3(0.0f, 10.0f, 50.0f);
vec3 cameraLookAt = vec3(0.0f, 0.0f, 0.0f);

//for Framebuffer
GLuint FBOTexture;
GLuint FBODepthBuffer;
GLuint frameBufferObject;
GLuint fullScreenVAO;
GLuint fullScreenVBO;
GLuint fullScreenShaderProgram;
const int FRAME_BUFFER_WIDTH = 640;
const int FRAME_BUFFER_HEIGHT = 480;

//timing
unsigned int lastTicks, currentTicks;
float elapsedTime;
float totalTime;

vec2 screenResolution = vec2(FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);

void mouseMovement()
{
	SDL_GetMouseState(&mouseX, &mouseY);
	hAngle += mSpeed * float(FRAME_BUFFER_WIDTH / 2 - mouseX);
	if (degrees(hAngle) > 360.0f)
	{
		hAngle = radians(0.0f);
	}
	else if (degrees(hAngle) < 0.0f)
	{
		hAngle = radians(360.0f);
	}
	vAngle += mSpeed * float(FRAME_BUFFER_HEIGHT / 2 - mouseY);
	if (degrees(vAngle) >= 90.0f)
	{
		vAngle = radians(-89.9f);
	}
	cameraLookAt = vec3(cos(vAngle)*sin(hAngle), sin(vAngle), cos(vAngle)*cos(hAngle));
	rightVector = vec3(sin(hAngle - 3.14 / 2), 0, cos(hAngle - 3.14 / 2));
}

void createFramebuffer()
{
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &FBOTexture);
	glBindTexture(GL_TEXTURE_2D, FBOTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0, GL_RGBA,
		GL_UNSIGNED_BYTE, NULL);


	glGenRenderbuffers(1, &FBODepthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, FBODepthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glGenFramebuffers(1, &frameBufferObject);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferObject);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, FBOTexture, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, FBODepthBuffer);

	GLenum status;
	if ((status = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE) {
		cout << "Issue with Framebuffers" << endl;
	}
	float vertices[] = {
		-1, -1,
		1, -1,
		-1, 1,
		1, 1,

	};

	glGenVertexArrays(1, &fullScreenVAO);
	glBindVertexArray(fullScreenVAO);

	glGenBuffers(1, &fullScreenVBO);
	glBindBuffer(GL_ARRAY_BUFFER, fullScreenVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT ,GL_FALSE, 0, 0);

	GLuint vertexShaderProgram = 0;
	string vsPath = ASSET_PATH + SHADER_PATH + "/simplePostProcessVS.glsl";
	vertexShaderProgram = loadShaderFromFile(vsPath, VERTEX_SHADER);
	checkForCompilerErrors(vertexShaderProgram);

	GLuint fragmentShaderProgram = 0;
	string fsPath = ASSET_PATH + SHADER_PATH + "/simplePostProcessFS.glsl";
	fragmentShaderProgram = loadShaderFromFile(fsPath, FRAGMENT_SHADER);
	checkForCompilerErrors(fragmentShaderProgram);

	fullScreenShaderProgram = glCreateProgram();
	glAttachShader(fullScreenShaderProgram, vertexShaderProgram);
	glAttachShader(fullScreenShaderProgram, fragmentShaderProgram);

	//Link attributes
	glBindAttribLocation(fullScreenShaderProgram, 0, "vertexPosition");

	glLinkProgram(fullScreenShaderProgram);
	checkForLinkErrors(fullScreenShaderProgram);
	//now we can delete the VS & FS Programs
	glDeleteShader(vertexShaderProgram);
	glDeleteShader(fragmentShaderProgram);
}

void initScene()
{
	currentTicks = SDL_GetTicks();
	totalTime = 0.0f;
	createFramebuffer();
	string modelPath = ASSET_PATH + MODEL_PATH + "/utah-teapot.fbx";
	auto currentGameObject = loadFBXFromFile(modelPath);

	string vsPath = ASSET_PATH + SHADER_PATH + "/specularVS.glsl";
	string fsPath = ASSET_PATH + SHADER_PATH + "/specularFS.glsl";
	currentGameObject->loadShader(vsPath, fsPath);
	currentGameObject->setScale(vec3(0.3f, 0.3f, 0.3f));

	gameObjects.push_back(currentGameObject);

	modelPath = ASSET_PATH + MODEL_PATH + "/armoredrecon.fbx";
	currentGameObject = loadFBXFromFile(modelPath);
	currentGameObject->loadShader(vsPath, fsPath);
	gameObjects.push_back(currentGameObject);
	SDL_ShowCursor(0);
}

void cleanUpFrambuffer()
{
	glDeleteProgram(fullScreenShaderProgram);
	glDeleteBuffers(1, &fullScreenVBO);
	glDeleteVertexArrays(1, &fullScreenVAO);
	glDeleteFramebuffers(1, &frameBufferObject);
	glDeleteRenderbuffers(1, &FBODepthBuffer);
	glDeleteTextures(1, &FBOTexture);
}

void cleanUp()
{
	cleanUpFrambuffer();
	gameObjects.clear();
}

void update()
{
	lastTicks = currentTicks;
	currentTicks = SDL_GetTicks();
	elapsedTime = (currentTicks - lastTicks) / 1000.0f;
	totalTime += elapsedTime;

	projMatrix = perspective(45.0f, 640.0f / 480.0f, 0.1f, 100.0f);

	viewMatrix = lookAt(cameraPosition, cameraLookAt + cameraPosition, vec3(0.0f, 1.0f, 0.0f));

	for (auto iter = gameObjects.begin(); iter != gameObjects.end(); iter++)
	{
		(*iter)->update();
	}
}

void renderGameObject(shared_ptr<GameObject> gameObject)
{
	MVPMatrix = projMatrix*viewMatrix*gameObject->getModelMatrix();

	if (gameObject->getShaderProgram() > 0){
		currentShaderProgam = gameObject->getShaderProgram();
		glUseProgram(currentShaderProgam);
	}

	GLint MVPLocation = glGetUniformLocation(currentShaderProgam, "MVP");

	GLint ambientLightColourLocation = glGetUniformLocation(currentShaderProgam, "ambientLightColour");
	GLint ambientMaterialColourLocation = glGetUniformLocation(currentShaderProgam, "ambientMaterialColour");

	GLint diffuseLightColourLocation = glGetUniformLocation(currentShaderProgam, "diffuseLightColour");
	GLint diffuseLightMaterialLocation = glGetUniformLocation(currentShaderProgam, "diffuseMaterialColour");
	GLint lightDirectionLocation = glGetUniformLocation(currentShaderProgam, "lightDirection");

	GLint specularLightColourLocation = glGetUniformLocation(currentShaderProgam, "specularLightColour");
	GLint specularLightMaterialLocation = glGetUniformLocation(currentShaderProgam, "specularMaterialColour");
	GLint specularPowerLocation = glGetUniformLocation(currentShaderProgam, "specularPower");
	GLint cameraPositionLocation = glGetUniformLocation(currentShaderProgam, "cameraPosition");

	GLint modelLocation = glGetUniformLocation(currentShaderProgam, "Model");

	glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, value_ptr(MVPMatrix));
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, value_ptr(gameObject->getModelMatrix()));

	glUniform4fv(ambientLightColourLocation, 1, value_ptr(ambientLightColour));
	glUniform4fv(ambientMaterialColourLocation, 1, value_ptr(gameObject->getAmbientMaterial()));

	glUniform4fv(diffuseLightColourLocation, 1, value_ptr(diffuseLightColour));
	glUniform4fv(diffuseLightMaterialLocation, 1, value_ptr(gameObject->getDiffuseMaterial()));
	glUniform3fv(lightDirectionLocation, 1, value_ptr(lightDirection));

	glUniform4fv(specularLightColourLocation, 1, value_ptr(specularLightColour));
	glUniform4fv(specularLightMaterialLocation, 1, value_ptr(gameObject->getSpecularMaterial()));
	glUniform1f(specularPowerLocation, gameObject->getSpecularPower());
	glUniform3fv(cameraPositionLocation, 1, value_ptr(cameraPosition));


	glBindVertexArray(gameObject->getVertexArrayObject());

	glDrawElements(GL_TRIANGLES, gameObject->getNumberOfIndices(), GL_UNSIGNED_INT, 0);

	for (int i = 0; i < gameObject->getNumberOfChildren(); i++)
	{
		renderGameObject(gameObject->getChild(i));
	}
}

void renderScene()
{
	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferObject);
	//Set the clear colour(background)
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	//clear the colour and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (auto iter = gameObjects.begin(); iter != gameObjects.end(); iter++)
	{
		renderGameObject((*iter));
	}
}

void renderPostQuad()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//Set the clear colour(background)
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	//clear the colour and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(fullScreenShaderProgram);

	GLint textureLocation = glGetUniformLocation(fullScreenShaderProgram, "texture0");
	GLint timeLocation = glGetUniformLocation(fullScreenShaderProgram, "time");
	GLint resolutionLocation = glGetUniformLocation(fullScreenShaderProgram, "resolution");

	glUniform1f(timeLocation, totalTime);
	glUniform2fv(resolutionLocation, 1, value_ptr(screenResolution));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, FBOTexture);
	glUniform1i(textureLocation, 0);

	glBindVertexArray(fullScreenVAO);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

}

void render()
{
	renderScene();
	renderPostQuad();
}


int main(int argc, char * arg[])
{
	ChangeWorkingDirectory();
	//Controls the game loop
	bool run = true;

	// init everyting - SDL, if it is nonzero we have a problem
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		std::cout << "ERROR SDL_Init " << SDL_GetError() << std::endl;

		return -1;
	}
	//
	int	imageInitFlags = IMG_INIT_JPG | IMG_INIT_PNG;
	int	returnInitFlags = IMG_Init(imageInitFlags);
	if (((returnInitFlags)&	(imageInitFlags)) != imageInitFlags)	{

		cout << "ERROR	SDL_Image	Init	" << IMG_GetError() << endl;
	}

	if (TTF_Init() == -1)	{
		std::cout << "ERROR	TTF_Init:	" << TTF_GetError();
	}

	//Request opengl 3.1 context, Core Context
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

	//Create a window
	SDL_Window * window = SDL_CreateWindow(
		"SDL",             // window title
		SDL_WINDOWPOS_CENTERED,     // x position, centered
		SDL_WINDOWPOS_CENTERED,     // y position, centered
		640,                        // width, in pixels
		480,                        // height, in pixels
		SDL_WINDOW_OPENGL           // flags
		);

	// Create an OpenGL context associated with the window.
	SDL_GLContext glcontext = SDL_GL_CreateContext(window);

	//Call our InitOpenGL Function
	initOpenGL();
	//Set our viewport
	setViewport(640, 480);

	initScene();
	//Value to hold the event generated by SDL
	SDL_Event event;
	//Game Loop
	while (run)
	{
		//While we still have events in the queue
		while (SDL_PollEvent(&event)) {
			//Get event type
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				//set our boolean which controls the loop to false
				run = false;
			}
			if (event.type == SDL_KEYDOWN){
				switch (event.key.keysym.sym)
				{
				case SDLK_a:
					cameraPosition -= rightVector * speed;
					cout << "left" << endl;
					break;
				case SDLK_d:
					cameraPosition += rightVector * speed;
					cout << "right" << endl;
					break;
				case SDLK_w:
					cameraPosition += cameraLookAt * speed;
					cout << "in" << endl;
					break;
				case SDLK_s:
					cameraPosition -= cameraLookAt * speed;
					cout << "out" << endl;
					break;
				case SDLK_SPACE:
					break;
				case SDLK_ESCAPE:
					cout << "exit" << endl;
					run = false;
					break;
				default:
					break;
				}
			}
		}
		//init Scene
		mouseMovement();
		SDL_WarpMouseInWindow(window, FRAME_BUFFER_WIDTH / 2, FRAME_BUFFER_HEIGHT / 2);
		update();
		//render
		render();
		//Call swap so that our GL back buffer is displayed
		SDL_GL_SwapWindow(window);

	}

	// clean up, reverse order!!!
	cleanUp();
	SDL_GL_DeleteContext(glcontext);
	SDL_DestroyWindow(window);
	IMG_Quit();
	TTF_Quit();
	SDL_Quit();
	return 0;
}