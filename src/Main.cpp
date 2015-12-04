#include "Common.h"
#include "Graphics.h"
#include "Vertices.h"
#include "Shader.h"
#include "Texture.h"
#include "Mesh.h"
#include "FBXLoader.h"
#include "FileSystem.h"
#include "GameObject.h"

//matrices
mat4 viewMatrix;
mat4 projMatrix;
mat4 MVPMatrix;

vector<shared_ptr<GameObject> > gameObjects;
GLuint currentShaderProgam = 0;

GLuint textureMap;

vec4 ambientLightColour = vec4(1.0f, 1.0f, 1.0f, 1.0f);
vec4 diffuseLightColour = vec4(1.0f, 1.0f, 1.0f, 1.0f);
vec4 specularLightColour = vec4(1.0f, 1.0f, 1.0f, 1.0f);
float specularPower = 25.0f;

vec3 lightDirection = vec3(0.0f, 0.0f, 1.0f);
vec3 cameraPosition = vec3(15.0f, 15.0f, 25.0f);

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

void createFramebuffer()
{
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &FBOTexture);
	glBindTexture(GL_TEXTURE_2D, FBOTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

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
		-1,  1,
		 1,  1,
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
	string vsPath = ASSET_PATH + SHADER_PATH + "/specularVS.glsl";
	string fsPath = ASSET_PATH + SHADER_PATH + "/specularFS.glsl";

	//Model 1
	string modelPath = ASSET_PATH + MODEL_PATH + "/X-Wing.fbx";
	string texturePath = ASSET_PATH + TEXTURE_PATH + "/X-Wing/X-Wing.png";
	auto currentGameObject = loadFBXFromFile(modelPath);
	currentGameObject->loadShader(vsPath, fsPath);
	currentGameObject->setScale(vec3(0.5f, 0.5f, 0.5f));
	currentGameObject->setPosition(vec3(0.0f, 0.0f, 500.0f));
	gameObjects.push_back(currentGameObject);

	//Model 2
	modelPath = ASSET_PATH + MODEL_PATH + "/VaderTie-Fighter.fbx";
	texturePath = ASSET_PATH + TEXTURE_PATH + "";
	textureMap = loadTextureFromFile(texturePath);
	currentGameObject = loadFBXFromFile(modelPath);
	currentGameObject->loadShader(vsPath, fsPath);
	currentGameObject->setScale(vec3(0.5f, 0.5f, 0.5f));
	currentGameObject->setPosition(vec3(0.0f, 0.0f, -500.0f));
	gameObjects.push_back(currentGameObject);
}

void cleanUpFrambuffer()
{
	glDeleteTextures(1, &textureMap);
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

	viewMatrix = lookAt(cameraPosition, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

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
	SDL_Window * window = SDL_CreateWindow("SDL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);

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
				case SDLK_LEFT:
					break;
				case SDLK_RIGHT:
					break;
				case SDLK_UP:
					break;
				case SDLK_DOWN:
					break;
				default:
					break;
				}
			}
		}
		//init Scene
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