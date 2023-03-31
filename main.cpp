
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Shader.hpp"
#include "Model3D.hpp"
#include "Camera.hpp"
#include "SkyBox.hpp"
#include <iostream>

int glWindowWidth = 1924;
int glWindowHeight = 1080;
int retina_width, retina_height;
GLFWwindow* glWindow = NULL;

const unsigned int SHADOW_WIDTH = 6024;
const unsigned int SHADOW_HEIGHT = 6024;

glm::mat4 model;
GLuint modelLoc;
glm::mat4 view;
GLuint viewLoc;
glm::mat4 projection;
GLuint projectionLoc;
glm::mat3 normalMatrix;
GLuint normalMatrixLoc;

//LUMINI:

//punctiforma 3:
glm::vec3 lightPunct1;
GLuint lightPunct1Loc;
glm::vec3 lightPunct1Color;
GLuint lightPunct1ColorLoc;

glm::vec3 lightPunct2;
GLuint lightPunct2Loc;
glm::vec3 lightPunct2Color;
GLuint lightPunct2ColorLoc;

glm::vec3 lightPunct3;
GLuint lightPunct3Loc;
glm::vec3 lightPunct3Color;
GLuint lightPunct3ColorLoc;

//directionala 2:
glm::vec3 lightDir1;
GLuint lightDirLoc1;
glm::vec3 lightColor1;
GLuint lightColorLoc1;

glm::vec3 lightDir2;
GLuint lightDirLoc2;
glm::vec3 lightColor2;
GLuint lightColorLoc2;

//switch uri pentru lumini:
int switch1 = 0, switch2 = 1, switchAuto = 0;
GLuint switchLoc1, switchLoc2, switchLoc3, switchLoc4;


gps::Camera myCamera(
	glm::vec3(-7.8324f, 2.571316f, 35.1402f),  //cameraPosition
	glm::vec3(0.0f, 0.0f, -10.0f), //cameraTarget
	glm::vec3(0.0f, 1.0f, 0.0f)); //cameraUpDirection

float cameraSpeed = 0.2f;
float pitch = 0.0f;
float yaw = -90.0f;


bool pressedKeys[1024];

std::vector<const GLchar*> faces;
gps::Model3D ground;
gps::Model3D house;
gps::Model3D mill;
gps::Model3D tree;
gps::Model3D lightCube;
gps::Model3D screenQuad;
gps::Model3D tv;
gps::Model3D tv2;
gps::Model3D wheel;
gps::Model3D drop;

gps::Shader depthMapShader;
gps::Shader myCustomShader;
gps::Shader screenQuadShader;


gps::SkyBox mySkyBox;
gps::Shader skyboxShader;

GLuint shadowMapFBO;   //framebuffer pentru umbre
GLuint depthMapTexture;   // textura atasata

//variabile:
bool showDepthMap;
bool startTour = false;
bool firstMouse = true;
bool TVoff = true;
bool Wheeloff = true;

float height = 0.0f;
float dropX[5001];
float dropY[5001];
float dropZ[5001];
float dropSpeed[5001];

float seconds = 0;
float delta = 0;
double lastTimeStamp = glfwGetTime();
int order = 0;

float movementSpeed = 200;
float lastX = glWindowWidth / 2, lastY = glWindowHeight / 2; // cursor in centru

float p, y;
int wait;
int flash=0;
int stage = 0;
int numberDrops = 5000;


GLenum glCheckError_(const char* file, int line) {
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	fprintf(stdout, "window resized to width: %d , and height: %d\n", width, height);
	//TODO	
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_M && action == GLFW_PRESS)
		showDepthMap = !showDepthMap;


	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			pressedKeys[key] = true;
		else if (action == GLFW_RELEASE)
			pressedKeys[key] = false;
	}
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	if(startTour == false)
		myCamera.rotate(pitch, yaw);
}

void processMovement()
{

	if (glfwGetKey(glWindow, GLFW_KEY_B)) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	if (glfwGetKey(glWindow, GLFW_KEY_N)) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	if (glfwGetKey(glWindow, GLFW_KEY_V)) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}

	if (glfwGetKey(glWindow, GLFW_KEY_T)) {
		startTour = true;
	}


	if (pressedKeys[GLFW_KEY_SPACE]) {
		myCamera.move(gps::MOVE_UP, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_LEFT_CONTROL]) {
		myCamera.move(gps::MOVE_DOWN, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_W]) {
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
	}
	if (pressedKeys[GLFW_KEY_S]) {
		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_A]) {
		myCamera.move(gps::MOVE_LEFT, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_D]) {
		myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_1]) {
		switch1 = 0;
		printf("light1 off \n");
	}if (pressedKeys[GLFW_KEY_2]) {
		switch1 = 1;
		printf("light1 on \n");
	}

	if (pressedKeys[GLFW_KEY_3]) {
		switch2 = 0;
		printf("light2 off \n");
	}if (pressedKeys[GLFW_KEY_4]) {
		switch2 = 1;
		printf("light2 on \n");
	}

}

bool initOpenGLWindow()
{
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "OpenGL Shader Example", NULL, NULL);
	if (!glWindow) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
	glfwSetKeyCallback(glWindow, keyboardCallback);
	glfwSetCursorPosCallback(glWindow, mouseCallback);
	//hide and capture cursor:
	glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwMakeContextCurrent(glWindow);

	glfwSwapInterval(1);

	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	return true;
}

void initOpenGLState()
{
	glClearColor(0.3, 0.3, 0.3, 1.0);
	glViewport(0, 0, retina_width, retina_height);

	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise

	glEnable(GL_FRAMEBUFFER_SRGB);
}

void initObjects() {
	tree.LoadModel("objects/tree/untitled.obj");
	house.LoadModel("objects/house/house.obj");
	mill.LoadModel("objects/mill/mill.obj");
	tv.LoadModel("objects/TV/tv.obj");
	tv2.LoadModel("objects/tv2/tv.obj");
	wheel.LoadModel("objects/whell/untitled.obj");
	drop.LoadModel("objects/rain/untitled.obj");
	ground.LoadModel("objects/ground/ground.obj");
	lightCube.LoadModel("objects/cube/cube.obj");
	screenQuad.LoadModel("objects/quad/quad.obj");
}

void initShaders() {
	depthMapShader.loadShader("shaders/depthMapShader.vert", "shaders/depthMapShader.frag");
	depthMapShader.useShaderProgram();
	myCustomShader.loadShader("shaders/shaderStart.vert", "shaders/shaderStart.frag");
	myCustomShader.useShaderProgram();
	screenQuadShader.loadShader("shaders/screenQuad.vert", "shaders/screenQuad.frag");
	screenQuadShader.useShaderProgram();
}

void initUniforms() {
	myCustomShader.useShaderProgram();

	model = glm::mat4(1.0f);
	modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	normalMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "normalMatrix");
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//LUMINI:

	//111111111111111
	lightPunct1 = glm::vec3(-27.4535f, 5.01222f, -1.43079f);//set the light direction (direction towards the light)
	lightPunct1Loc = glGetUniformLocation(myCustomShader.shaderProgram, "lightPunct1");
	glUniform3fv(lightPunct1Loc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view)) * lightPunct1));

	lightPunct1Color = glm::vec3(0.5f, 0.5f, 0.5f); //green
	lightPunct1ColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightPunct1Color");
	glUniform3fv(lightPunct1ColorLoc, 1, glm::value_ptr(lightPunct1Color));

	//2222222222222222
	lightPunct2 = glm::vec3(-29.5309f, 3.14751f, 11.321f);
	lightPunct2Loc = glGetUniformLocation(myCustomShader.shaderProgram, "lightPunct2");
	glUniform3fv(lightPunct2Loc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view)) * lightPunct2));

	lightPunct2Color = glm::vec3(0.88f, 0.34f, 0.13f); //red light
	lightPunct2ColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightPunct2Color");
	glUniform3fv(lightPunct2ColorLoc, 1, glm::value_ptr(lightPunct2Color));

	//33333333333333333
	lightPunct3 = glm::vec3(-23.1743f, 3.08349f, 12.0529f);
	lightPunct3Loc = glGetUniformLocation(myCustomShader.shaderProgram, "lightPunct3");
	glUniform3fv(lightPunct3Loc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view)) * lightPunct3));

	lightPunct3Color = glm::vec3(0.88f, 0.34f, 0.13f); //red light
	lightPunct3ColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightPunct3Color");
	glUniform3fv(lightPunct3ColorLoc, 1, glm::value_ptr(lightPunct3Color));

	//LUMINI DIRECTIONALE:

	//lumina dir 1 cu umbre de culoare alba
	lightDir1 = glm::vec3(-0.603885f, 2.02917f, 0.770348f);  // pozitia luminii (cubului) lumina directionala  // umbra
	lightDirLoc1 = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir1");
	glUniform3fv(lightDirLoc1, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view)) * lightDir1));


	lightColor1 = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	lightColorLoc1 = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor1");
	glUniform3fv(lightColorLoc1, 1, glm::value_ptr(lightColor1));

	//lumina dir 2 de culoare gri
	lightDir2 = glm::vec3(2.0f, 3.0f, 5.0f);
	lightDirLoc2 = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir2");
	glUniform3fv(lightDirLoc2, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view)) * lightDir2));

	lightColor2 = glm::vec3(0.2f, 0.2f, 0.2f); //white light
	lightColorLoc2 = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor2");
	glUniform3fv(lightColorLoc2, 1, glm::value_ptr(lightColor2));

	//get switch loc
	switchLoc1 = glGetUniformLocation(myCustomShader.shaderProgram, "switch1");
	switchLoc2 = glGetUniformLocation(myCustomShader.shaderProgram, "switch2");
	switchLoc3 = glGetUniformLocation(myCustomShader.shaderProgram, "switchAuto");
	switchLoc4 = glGetUniformLocation(myCustomShader.shaderProgram, "flash");

}

void initFBO() {
	//T.O.D.O - Create the FBO, the depth texture and attach the depth texture to the FBO
	glGenFramebuffers(1, &shadowMapFBO); // crearea frame-bufferului


	//create depth texture for FBO:
	glGenTextures(1, &depthMapTexture);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);


	// atasarea texturi la shadowmapfbo:
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	// dezactivat de-o cam data:

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void initDrops() {
	for (int i = 0; i <= numberDrops; i++) // fiecarui strop ii dam o coordonata:
	{

		dropX[i] = -52.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (24.0f - (-52.0f))));
		dropY[i] = 25.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 28.0f));
		dropZ[i] = -20.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (50.0f - (-20.0f))));

		while (dropX[i] <= -20.0f && dropX[i] >= -37.0f && dropZ[i] >= -5.0f && dropZ[i] <= 7.5f) {
			dropX[i] = -52.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (24.0f - (-52.0f))));
			dropZ[i] = -20.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (50.0f - (-20.0f))));
		}

		dropSpeed[i] = 0.1f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 0.7f));
	}
}

glm::mat4 computeLightSpaceTrMatrix() {
	//T.O.D.O. - Return the light-space transformation matrix

	// matricea view (pentru schimbarea origini in lumina:):
	glm::mat4 lightView = glm::lookAt( lightDir1, glm::vec3(0.0f,0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));// al doilea vec este poz
	//glm::mat4 lightView = glm::lookAt(lightDir, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f))
	//proiectie ortografica
	const GLfloat near_plane = -40.0f, far_plane = 30.5f;  // aici tre schimbat
	glm::mat4 lightProjection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, near_plane, far_plane);

	// produsulu lor care trebuie inmultit si cu modelul:
	glm::mat4 lightSpaceTrMatrix = lightProjection * lightView;

	return lightSpaceTrMatrix;
}

void updateDelta(double elapsedSeconds) {
	seconds = elapsedSeconds;
	delta = delta + movementSpeed * elapsedSeconds;
}

void drawObjects(gps::Shader shader, bool depthPass)
{

	shader.useShaderProgram();

	// retrimit swicthuri:
	glUniform1i(switchLoc1, switch1);
	glUniform1i(switchLoc2, switch2);
	glUniform1i(switchLoc4, flash);

	for (int i = 0; i <= numberDrops; i++) // desenam fiecare strop
	{
		dropY[i] -= dropSpeed[i];
		if (dropY[i] < 0) dropY[i] = 25.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 28.0f));

		model = glm::translate(glm::mat4(1.0f), glm::vec3(dropX[i], dropY[i], dropZ[i]));
		glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

		if (!depthPass) {
			normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
			glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
		}

		drop.Draw(shader);
	}


	//rotire si trenslatie:
	model = glm::translate(glm::mat4(1.0f), glm::vec3(2.33483f, 15.7935f, -9.39762f));
	model = glm::rotate(model, glm::radians(delta), glm::vec3(0.0f, 0.0f, 1.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

		mill.Draw(shader);
	}

	//rotire si trenslatie:
	model = glm::translate(glm::mat4(1.0f), glm::vec3(-21.8775f, 2.5135f, 26.593f));
	if(Wheeloff == false) model = glm::rotate(model, glm::radians(delta), glm::vec3(0.0f, 0.0f, 1.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

		wheel.Draw(shader);
	}


	model = glm::mat4(1.0f);
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	// do not send the normal matrix if we are rendering in the depth map
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}

	house.Draw(shader);
	tree.Draw(shader);

	if (TVoff) {
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-27.4535f, 5.01222f, -1.5f));
		glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

		// do not send the normal matrix if we are rendering in the depth map
		if (!depthPass) {
			normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
			glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
		}
		switchAuto = 0;
		tv.Draw(shader);
	}
	else {
		if (order >= 25) {
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(-27.4535f, 5.01222f, -1.53f));
			glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

			// do not send the normal matrix if we are rendering in the depth map
			if (!depthPass) {
				normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
				glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
			}
			switchAuto = 1; // light on
			tv2.Draw(shader);
		}
		if (order == 50) {
			order = 0;
			switchAuto = 0; //light off
		}
		order += 1;
	}
	glUniform1i(switchLoc3, switchAuto);

	model = glm::mat4(1.0f);
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	// do not send the normal matrix if we are rendering in the depth map
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}

	ground.Draw(shader);




}

void initSkyBox() {

	faces.push_back("textures/skybox/right.tga");
	faces.push_back("textures/skybox/left.tga");
	faces.push_back("textures/skybox/top.tga");
	faces.push_back("textures/skybox/bottom.tga");
	faces.push_back("textures/skybox/back.tga");
	faces.push_back("textures/skybox/front.tga");
	mySkyBox.Load(faces);
	skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
}

void renderScene() {

	glm::vec3 camPos = myCamera.getPosition();
	glm::vec3 distanta = camPos - glm::vec3(-27.4535f, 5.01222f, -1.5f);

	if (glm::length(distanta) < 6.0f)
		TVoff = false;
	else TVoff = true;

	 distanta = camPos - glm::vec3(-21.8775f, 2.5135f, 26.593f);
	if (glm::length(distanta) < 4.0f)
		Wheeloff = false;
	else Wheeloff = true;


	double currentTimeStamp = glfwGetTime();
	updateDelta(currentTimeStamp - lastTimeStamp);
	lastTimeStamp = currentTimeStamp;
	delta += 0.001;

	if (flash == 300) {
		flash = 0;
		//printf("flash");
	}
	flash += 1;

	if (startTour == true) {
		//printf("stage: %d \n", stage);
		if (stage == 0 && wait <50) {
			myCamera.rotate(p, y);
			myCamera.move(glm::vec3(-7.8324f, 2.571316f, 35.1402f));
			wait++;
		}
		else if (stage == 0) stage = 1;
		if (stage == 1 && y <= -184.0f) {
			myCamera.rotate(p, y);
			y += 1.0f;
			p += 0.34426f;
		}
		else if (stage == 1) stage = 2;
		if (stage == 2 && wait < 100) {
			wait++;
		}
		else if (stage == 2) stage = 3;
		if (stage == 3  &&  y <= -61.0f) {
			myCamera.rotate(p, y);
			y += 1.0f;
			p += 0.01707317f;
		}
		else if (stage == 3) stage = 4;
		if (stage == 4 && wait < 150) {
			wait++;
		}
		else if (stage == 4) stage = 5; 
		if (stage == 5 && y >= -147.0f) {
			myCamera.rotate(p, y);
			y -= 1.0f;
			p -= 0.0058139f;
		}
		else if (stage == 5) stage = 6;
		if (stage == 6 && wait < 220) {
			if (wait % 10 < 5) {
				p -= 0.1f;
				myCamera.rotate(p, y);
			}
			else {
				p += 0.1f;
				myCamera.rotate(p, y);
			}
			wait++;
			myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
		}
		else if (stage == 6) { stage = 7; }
		if (stage == 7 && y <= -105.0f) {
			myCamera.rotate(p, y);
			y += 1.0f;
			p += 0.035714f;
		}
		else if (stage == 7) stage = 8;
		if (stage == 8 && wait < 300) {
			if (wait % 10 < 5) {
				p -= 0.1f;
				myCamera.rotate(p, y);
			}
			else {
				p += 0.1f;
				myCamera.rotate(p, y);
			}
			wait++;
			myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
		}
		else if (stage == 8) stage = 9;
		if (stage == 9 && y >= -127.0f) {
			myCamera.rotate(p, y);
			y -= 1.0f;
			p -= 0.33636f;
		}
		else if (stage == 9) stage = 10;
		if (stage == 10 && wait < 340) {
			if (wait % 10 < 5) {
				p -= 0.1f;
				myCamera.rotate(p, y);
			}
			else {
				p += 0.1f;
				myCamera.rotate(p, y);
			}
			wait++;
			myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
		}
		else if (stage == 10) stage = 11;
		if (stage == 11 && y >= -172.0f) {
			myCamera.rotate(p, y);
			y -= 1.0f;
			p += 1.3711111f;
		}
		else if (stage == 11) stage = 12;
		if (stage == 12 && wait < 365) {
			if (wait % 10 < 5) {
				p -= 0.3f;
				myCamera.rotate(p, y);
			}
			else {
				p += 0.3f;
				myCamera.rotate(p, y);
			}
			wait++;
			myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
		}
		else if (stage == 12) stage = 13;
		if (stage == 13 && y <= -65.0f) {
			myCamera.rotate(p, y);
			y += 1.0f;
			p -= 0.540186f;
		}
		else if (stage == 13) stage = 14;
		if (stage == 14 && wait < 380) {
			wait++;
			myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
		}
		else if (stage == 14) {
			startTour = false; 
		}
	}
	else {
		p = -21.0f;
		y = -245.0f;
		stage = 0;
		wait = 0;
		//printf("%d \n", done);
	}


	// depth maps creation pass

	depthMapShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"),
		1,
		GL_FALSE,
		glm::value_ptr(computeLightSpaceTrMatrix()));
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	drawObjects(depthMapShader, true);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// render depth map on screen - toggled with the M key




	if (showDepthMap) {
		glViewport(0, 0, retina_width, retina_height);

		glClear(GL_COLOR_BUFFER_BIT);

		screenQuadShader.useShaderProgram();


		//bind the depth map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glUniform1i(glGetUniformLocation(screenQuadShader.shaderProgram, "depthMap"), 0);


		glDisable(GL_DEPTH_TEST);
		screenQuad.Draw(screenQuadShader);
		glEnable(GL_DEPTH_TEST);
	}
	else {


		// final scene rendering pass (with shadows)

		glViewport(0, 0, retina_width, retina_height);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		myCustomShader.useShaderProgram();

		view = myCamera.getViewMatrix();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		glUniform3fv(lightDirLoc1, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view )) * lightDir1));


		//bind the shadow map
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "shadowMap"), 3);

		glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "lightSpaceTrMatrix"),
			1,
			GL_FALSE,
			glm::value_ptr(computeLightSpaceTrMatrix()));  // aici primeste textura deumbr



		drawObjects(myCustomShader, false);  //asta e render the scene


		//draw a white cube around the light
		/*
		lightShader.useShaderProgram();

		glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

		model = lightRotation;
		model = glm::translate(model, 1.0f * lightPunct1);
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

		lightCube.Draw(lightShader);
		//
		*/




		// asta e un sky box care poate  fi sters
		//skyboxShader.useShaderProgram();
		//view = myCamera.getViewMatrix();
		//glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "view"), 1, GL_FALSE,
		//	glm::value_ptr(view));

		//projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
		//glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "projection"), 1, GL_FALSE,
		//	glm::value_ptr(projection));

		//mySkyBox.Draw(skyboxShader, view, projection);
		//
	}




	// aici il inchid
}

void cleanup() {
	glDeleteTextures(1, &depthMapTexture);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &shadowMapFBO);
	glfwDestroyWindow(glWindow);
	//close GL context and any other GLFW resources
	glfwTerminate();
}


int main(int argc, const char* argv[]) {

	if (!initOpenGLWindow()) {
		glfwTerminate();
		return 1;
	}

	initOpenGLState();
	initObjects();
	initShaders();
	initUniforms();
	initFBO();  // shadow map
	initSkyBox();
	initDrops();

	while (!glfwWindowShouldClose(glWindow)) {
		processMovement();
		renderScene();

		glfwPollEvents();
		glfwSwapBuffers(glWindow);
	}

	cleanup();

	return 0;
}
