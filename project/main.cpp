//
//  main.cpp
//  OpenGL Advances Lighting
//
//  Created by CGIS on 28/11/16.
//  Copyright � 2016 CGIS. All rights reserved.
//

#if defined (__APPLE__)
    #define GLFW_INCLUDE_GLCOREARB
    #define GL_SILENCE_DEPRECATION
#else
    #define GLEW_STATIC
    #include <GL/glew.h>
#endif

#define NR_TREES 50
#define NR_WATER_DROPS 10000
#define NR_CLOUDS 30
#define NR_CARS 3

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.hpp"
#include "Model3D.hpp"
#include "Camera.hpp"
#include "SkyBox.hpp"

#include <iostream>

int glWindowWidth = 800;
int glWindowHeight = 600;
int retina_width, retina_height;
GLFWwindow* glWindow = NULL;

const unsigned int SHADOW_WIDTH = 2048;
const unsigned int SHADOW_HEIGHT = 2048;

glm::mat4 model;
GLuint modelLoc;
glm::mat4 view;
GLuint viewLoc;
glm::mat4 projection;
GLuint projectionLoc;
glm::mat3 normalMatrix;
GLuint normalMatrixLoc;
glm::mat4 lightRotation;

glm::vec3 lightDir;
GLuint lightDirLoc;
glm::vec3 lightColor;
GLuint lightColorLoc;
bool isShadow = true;
GLuint isShadowLoc;
bool isDirLight = true;
GLuint isDirLightLoc;
bool isPointLight = true;
GLuint isPointLightLoc;
std::vector < GLuint> lightPointLoc(4);
std::vector <GLuint> isLampLoc(4);
GLuint isNightLoc;
bool isFog = false;
GLuint isFogLoc;
GLuint isThorLoc;

gps::Camera myCamera(
				glm::vec3(-65.4f, 11.1f, 46.7f),
				glm::vec3(-65.4f, 11.1f, 49.7f));
gps::Camera auxCamera(
	glm::vec3(0.0f, 0.0f, -3.0f),
	glm::vec3(0.0f, 0.0f, 0.0f));
float cameraSpeed = 2.0f;

bool pressedKeys[1024];

float angleY = 0.0f;
float water_rotation = 0.0f;
float sea_rotation = 0.0f;
float wheels_rotation = 0.0f;

GLfloat lightAngle;

gps::Model3D ground;
gps::Model3D sun;

gps::Model3D lamps;
std::vector<glm::vec3> lampPosition(4);

gps::Model3D small_water;
gps::Model3D medium_water;
gps::Model3D big_water;

gps::Model3D tree;
std::vector<glm::mat4> modelTrees(NR_TREES)
;
gps::Model3D car;
gps::Model3D wheels;

gps::Model3D sea;

gps::Model3D water_drop;
std::vector<glm::vec3> posWaterDrops(NR_WATER_DROPS);
std::vector<float> speedWaterDrops(NR_WATER_DROPS);

gps::Model3D cloud;
std::vector<glm::vec3> posClouds(NR_CLOUDS);
std::vector<glm::vec3> scaleClouds(NR_CLOUDS);
std::vector<float> rotateClouds(NR_CLOUDS);
float cloudSpeed = 0.3f;

gps::Shader myCustomShader;
gps::Shader lightShader;
gps::Shader depthMapShader;

GLuint shadowMapFBO;
GLuint depthMapTexture;

gps::SkyBox mySkyBox;
gps::SkyBox daySkyBox;
gps::SkyBox nightSkyBox;
gps::Shader skyboxShader;

bool cameraMoving = true;
int cadru = 0;
bool rain = false;
bool isNight = false;
bool isThunder = false;

struct CarAnimation {
	glm::vec3 init_car_pos;
	std::vector<glm::vec3> wheel_front_deplasament{ 2 };
	std::vector<glm::vec3> wheel_back_deplasament{ 2 };
	glm::vec3 corner_car_pos;
	glm::vec3 final_corner_pos;
	glm::vec3 current_car_pos;
	float car_speed;
	glm::vec3 direction;
	float corner_rotation;
	float unghiu_dr;
	float unghiu_st;
	bool checkFirstcorner = false;
};
std::vector<CarAnimation> myCars(NR_CARS);
void windowResizeCallback(GLFWwindow* window, int width, int height) {
	fprintf(stdout, "window resized to width: %d , and height: %d\n", width, height);
	//TODO	
}

float animation_aux = 0.0f;
void presentationAnimation()
{
	if (cameraMoving) {
		cadru = 0;
		auxCamera = myCamera;
	}
	else
	{
		cadru++;
		if (cadru <= 20) {
			myCamera = gps::Camera(glm::vec3(112.0f, 9.5f, -51.0f), glm::vec3(112.0f, 9.5f, -48.0f));
			myCamera.rotate(180.0f, 0.0f);
		}
		else if (cadru > 20 && cadru <= 125) {
			myCamera.move(gps::MOVE_FORWARD,0.5f);
		}
		else if (cadru > 125 && cadru <= 150) {
			animation_aux += 0.5f;
			myCamera.rotate(-animation_aux, 0.0f);
		}
		else if (cadru > 150 && cadru <= 170) {
			animation_aux = 0.0f;
			myCamera = gps::Camera(glm::vec3(98.2f, 80.0f, -48.0f), glm::vec3(98.2f, 80.0f, -45.0f));
			myCamera.rotate(120.0f, -40.0f);
		}
		else if (cadru > 170 && cadru <= 180) {
			myCamera = gps::Camera(glm::vec3(-55.3f, 6.9f, 52.6f), glm::vec3(-55.3f, 6.9f, 49.6f));
		}
		else if (cadru > 180 && cadru <= 220) {
			myCamera.move(gps::MOVE_FORWARD, 0.5f);
			myCamera.up();
		}
		else if (cadru > 220 && cadru <= 230) {
			myCamera.setPostion(glm::vec3(5.4f, 35.6f, 190.6f));
			myCamera.setTarget(glm::vec3(6.4f, 35.4f, 190.9f));
		}
		else if (cadru > 230 && cadru <= 350)
		{
			animation_aux += 0.5f;
			myCamera.rotate(-animation_aux, -animation_aux);
			myCamera.up();
		}
		else {
			myCamera = auxCamera;
			cameraMoving = true;
		}
	}
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			pressedKeys[key] = true;
		else if (action == GLFW_RELEASE)
			pressedKeys[key] = false;
	}

}

double yaw = 0.0f;
double pitch = 0.0f;
double lastX = 0.0f, lastY = 0.0f;;
bool firstMouse = true;
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

	float sensitivity = 0.25f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (cameraMoving) myCamera.rotate(yaw, pitch);
}

void processMovement()
{
	if (pressedKeys[GLFW_KEY_1]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}

	if (pressedKeys[GLFW_KEY_2]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	if (pressedKeys[GLFW_KEY_3]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	if (pressedKeys[GLFW_KEY_7]) {
		isShadow = !isShadow;
		myCustomShader.useShaderProgram();
		glUniform1i(isShadowLoc, isShadow);
	}
	if (pressedKeys[GLFW_KEY_8]) {
		isDirLight = !isDirLight;
		myCustomShader.useShaderProgram();
		glUniform1i(isDirLightLoc, isDirLight);
	}
	if (pressedKeys[GLFW_KEY_9]) {
		isPointLight = !isPointLight;
		myCustomShader.useShaderProgram();
		glUniform1i(isPointLightLoc, isPointLight);
	}

	if (pressedKeys[GLFW_KEY_J]) {
		lightAngle -= 1.0f;
	}

	if (pressedKeys[GLFW_KEY_L]) {
		lightAngle += 1.0f;
	}

	if (pressedKeys[GLFW_KEY_Q]) {
		cameraMoving = false;
	}

	if (cameraMoving)
	{
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

		if (pressedKeys[GLFW_KEY_UP])
		{
			myCamera.up();
		}

		if (pressedKeys[GLFW_KEY_DOWN])
		{
			myCamera.down();
		}
	}
	
	if (pressedKeys[GLFW_KEY_P])
	{
		glm::vec3 position = myCamera.getPostion();
		printf("%f %f %f\n", position.x, position.y, position.z);
		glm::vec3 target = myCamera.getTarget();
		printf("%f %f %f\n\n", target.x, target.y, target.z);
	}

	if (pressedKeys[GLFW_KEY_Z]) {
		rain = !rain;
	}
	if (pressedKeys[GLFW_KEY_X]) {
		isNight = !isNight;
	}
	if (pressedKeys[GLFW_KEY_C]) {
		isFog = !isFog;
		myCustomShader.useShaderProgram();
		glUniform1i(isFogLoc, isFog);
	}
	if (pressedKeys[GLFW_KEY_V]) {
		isThunder = !isThunder;
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

	// window scaling for HiDPI displays
	glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

	// for sRBG framebuffer
	glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);

	// for antialiasing
	glfwWindowHint(GLFW_SAMPLES, 4);

	// Get the primary monitor for full-screen
	GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);

	// Create a full-screen window
	glWindow = glfwCreateWindow(mode->width, mode->height, "OpenGL Project", primaryMonitor, NULL);

	if (!glWindow) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
	glfwSetKeyCallback(glWindow, keyboardCallback);
	glfwSetCursorPosCallback(glWindow, mouseCallback);
	glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	glfwMakeContextCurrent(glWindow);

	glfwSwapInterval(1);

#if not defined (__APPLE__)
	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();
#endif

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION);    // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	// for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	return true;
}

void initOpenGLState()
{
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glViewport(0, 0, retina_width, retina_height);

	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise

	glEnable(GL_FRAMEBUFFER_SRGB);
}

glm::mat4 generateModelTree() {
	glm::mat4 model = glm::mat4(1.0f);

	float x, y, z;
	do {
		x = static_cast<float>(std::rand() % 156 - 45);  // Intervalul -45 la 110
	} while ((x >= 30.0f && x <= 48.0f) || (x >= 15.0f && x <= 60.0f));
	y = static_cast<float>(std::rand() % 26) / 5.0f;  // Intervalul 0 la 5
	do {
		z = static_cast<float>(std::rand() % 156 - 25);  // Intervalul -25 la 130
	} while ((z >= 45.0f && z <= 63.0f) || ((x >= 15.0f && x <= 60.0f) && (y >= 30.0f && y <= 75.0f)));
	model = glm::translate(model, glm::vec3(x, 23.5f - y, z));

	float angle = static_cast<float>(std::rand() % 360);
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));

	return model;
}
void initTrees()
{
	tree.LoadModel("objects/tree/tree.obj");
	for (int i = 0; i < NR_TREES; i++) {

		modelTrees[i] = generateModelTree();
	}
}
glm::vec3 generatePozWaterDrops() {
	glm::vec3 aux;
	aux.x = static_cast<float>(std::rand() % 156 - 45);  // Intervalul -45 la 110
	aux.y = 200.0f + static_cast<float>(std::rand() % 26) / 5.0f;  // Intervalul 0 la 5
	aux.z = static_cast<float>(std::rand() % 156 - 25);  // Intervalul -25 la 130

	return aux;
}
float generatspeedWaterDrops()
{
	return (float)rand() / RAND_MAX * 12.0f + 2.0f;
}
void initWaterDrops()
{
	water_drop.LoadModel("objects/water_drop/water_drop.obj");
	for (int i = 0; i < NR_WATER_DROPS; i++) {

		posWaterDrops[i] = generatePozWaterDrops();
		speedWaterDrops[i] = generatspeedWaterDrops();
	}
}
float generateRandomValue(int lower, int upper, int multiple) {
	int num = (rand() % ((upper - lower) / multiple + 1)) * multiple + lower;
	return (float)num;
}
glm::vec3 generateCloudPost()
{
	float x = generateRandomValue(-400, 200, 20);
	float y = generateRandomValue(480, 530, 20);
	float z = generateRandomValue(-400, 200, 20);
	return glm::vec3(x, y, z);
}
glm::vec3 generateCloudScale()
{
	float x = (float)rand() / RAND_MAX * 0.5f + 0.1f;;
	return glm::vec3(x, x, x);
}
float generateCloudRotate()
{
	return (float)rand() / RAND_MAX * 30.0f - 15.0f;;
}
void initClouds()
{
	cloud.LoadModel("objects/cloud/cloud.obj");
	for (int i = 0; i < NR_CLOUDS; i++)
	{
		posClouds[i] = generateCloudPost();
		scaleClouds[i] = generateCloudScale();
		rotateClouds[i] = generateCloudRotate();
	}
}
CarAnimation initCarData(float speed,glm::vec3 init_pos, glm::vec3 corner_pos, glm::vec3 final_pos)
{
	CarAnimation car;
	car.init_car_pos = init_pos;
	car.current_car_pos = init_pos;

	car.corner_car_pos = corner_pos;
	car.final_corner_pos = final_pos;

	car.wheel_front_deplasament = { glm::vec3(7.0f, -6.1f, 14.4f), glm::vec3(-6.7f, -6.1f, 14.4f) };
	car.wheel_back_deplasament = { glm::vec3(7.0f, -6.1f, -10.9f),  glm::vec3(-6.7f, -6.1f, -10.9f) };

	car.direction = glm::vec3(0, 0, 1.0f);
	car.car_speed = speed;
	car.direction = glm::vec3(0.0, 0.0f, 1.0);
	car.corner_rotation = 0.0f;
	car.unghiu_dr = -90.0f;
	car.unghiu_st = 90.0f;
	car.checkFirstcorner = false;

	return car;
}
void initCar()
{
	myCars[0] = initCarData(10.5f,
		glm::vec3(212.4f, 10.1f, -475.8f),
		glm::vec3(212.4f, 10.1f, 221.7f),
		glm::vec3(-530.4f, 10.1f, 210.7f));
	myCars[1] = initCarData(10.5f,
		glm::vec3(168.1f, 11.1f, -379.0f),
		glm::vec3(168.1f, 11.1, 179.424f),
		glm::vec3(-469.7f, 11.1, 179.424f));
	myCars[2] = initCarData(8.5f,
		glm::vec3(200.1f, 11.1f, -534.4f),
		glm::vec3(200.1f, 11.1f, 213.7f),
		glm::vec3(-532.91f, 11.1f, 213.7));
}
void initObjects() {

	initTrees();
	initWaterDrops();
	initClouds();

	initCar();
	car.LoadModel("objects/car/car/car.obj");
	wheels.LoadModel("objects/car/wheel/wheel.obj");

	lamps.LoadModel("objects/lamp/lamp.obj");
	lampPosition = { glm::vec3(21.8f, 10.6f, 37.3f), glm::vec3(51.7f, 10.6f, 37.3f),glm::vec3(51.7f, 10.6f, 67.26f),glm::vec3(21.8f, 10.6f, 67.26f) };

	ground.LoadModel("objects/ground/ground.obj");

	sun.LoadModel("objects/sun/sun.obj");

	small_water.LoadModel("objects/water/small_water/small_water.obj");
	medium_water.LoadModel("objects/water/medium_water/medium_water.obj");
	big_water.LoadModel("objects/water/big_water/big_water.obj");

	sea.LoadModel("objects/sea/sea.obj");
}

void initShaders() {
	skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
	skyboxShader.useShaderProgram();
	depthMapShader.loadShader("shaders/depthMapShader.vert", "shaders/depthMapShader.frag");
	depthMapShader.useShaderProgram();
	myCustomShader.loadShader("shaders/shaderStart.vert", "shaders/shaderStart.frag");
	myCustomShader.useShaderProgram();
	lightShader.loadShader("shaders/lightCube.vert", "shaders/lightCube.frag");
	lightShader.useShaderProgram();
}

void initUniforms() {
	myCustomShader.useShaderProgram();

	model = glm::mat4(1.0f);
	modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	normalMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "normalMatrix");
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	
	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//set the light direction (direction towards the light)
	lightDir = glm::vec3(30.0f, 40.0f, -60.0f);
	lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");	
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

	//set light color
	lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

	isShadowLoc = glGetUniformLocation(myCustomShader.shaderProgram, "isShadow");
	glUniform1i(isShadowLoc, isShadow);

	isDirLightLoc = glGetUniformLocation(myCustomShader.shaderProgram, "isDirLight");
	glUniform1i(isDirLightLoc, isDirLight);

	isPointLightLoc = glGetUniformLocation(myCustomShader.shaderProgram, "isPointLight");
	glUniform1i(isPointLightLoc, isPointLight);

	isLampLoc[0] = glGetUniformLocation(myCustomShader.shaderProgram, "isLamp0");
	isLampLoc[1] = glGetUniformLocation(myCustomShader.shaderProgram, "isLamp1");
	isLampLoc[2] = glGetUniformLocation(myCustomShader.shaderProgram, "isLamp2");
	isLampLoc[3] = glGetUniformLocation(myCustomShader.shaderProgram, "isLamp3");


	lightPointLoc[0] = glGetUniformLocation(myCustomShader.shaderProgram, "lightPoint0"); 
	lightPointLoc[1] = glGetUniformLocation(myCustomShader.shaderProgram, "lightPoint1");
	lightPointLoc[2] = glGetUniformLocation(myCustomShader.shaderProgram, "lightPoint2");
	lightPointLoc[3] = glGetUniformLocation(myCustomShader.shaderProgram, "lightPoint3");

	for (int i = 0; i < 4; i++)
	{
		glUniform1i(isLampLoc[i], false);
		glUniform3fv(lightPointLoc[i], 1, glm::value_ptr(lampPosition[i]));
	}

	isFogLoc = glGetUniformLocation(myCustomShader.shaderProgram, "isFog");
	glUniform1i(isFogLoc, isFog);

	isThorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "isThor");
	glUniform1i(isThorLoc, false);

	lightShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	isNightLoc = glGetUniformLocation(lightShader.shaderProgram, "isNIght");
	glUniform1i(isNightLoc, isNight);
}

void initFBO() {
	glGenFramebuffers(1, &shadowMapFBO);
	
	//create depth texture for FBO
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

	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture,
		0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 computeLightSpaceTrMatrix() {
	glm::mat4 lightView = glm::lookAt(glm::mat3(lightRotation)*lightDir, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	lightView = glm::rotate(lightView, glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
	const GLfloat near_plane = 0.01f, far_plane = 125.0f;
	glm::mat4 lightProjection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, near_plane, far_plane);
	return lightProjection * lightView;
}

void drawTrees(gps::Shader shader, bool depthPass) {
	for (int i = 0; i < NR_TREES; i++)
	{
		shader.useShaderProgram();
		model = modelTrees[i];
		glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
		// do not send the normal matrix if we are rendering in the depth map
		if (!depthPass) {
			normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
			glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
		}
		tree.Draw(shader);
	}
}
glm::mat4 water_model_1(float y)
{
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(37.12f, y, 52.26f));
	model = glm::translate(model, glm::vec3(0, -1.0f, 0));
	model = glm::rotate(model, glm::radians(-water_rotation), glm::vec3(0, 0, 1.0f));
	return model;
}
glm::mat4 water_model_2(float y)
{
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(37.12f, y, 52.26f));
	model = glm::translate(model, glm::vec3(0, -1.0f, 0));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(water_rotation), glm::vec3(0, 0, 1.0f));
	return model;
}
void drawWater(gps::Shader shader, bool depthPass)
{
	shader.useShaderProgram();

	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}
	model = water_model_1(18.92f);
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	small_water.Draw(shader);

	shader.useShaderProgram();
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}
	model = water_model_2(18.92f);
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	small_water.Draw(shader);

	shader.useShaderProgram();
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}
	model = water_model_1(14.93f);
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	medium_water.Draw(shader);

	shader.useShaderProgram();
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}
	model = water_model_2(14.93f);
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	medium_water.Draw(shader);

	shader.useShaderProgram();
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}
	model = water_model_1(6.96);
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	big_water.Draw(shader);

	shader.useShaderProgram();
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}
	model = water_model_2(6.96);
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	big_water.Draw(shader);

}
void drawSea(gps::Shader shader, bool depthPass) {
	shader.useShaderProgram();
	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -10.0f, 0.0f));
	model = glm::rotate(model, glm::radians(sea_rotation), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	// do not send the normal matrix if we are rendering in the depth map
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}
	sea.Draw(shader);
}
void drawLamps(gps::Shader shader, bool depthPass)
{
	for (int i = 0; i < 4; i++)
	{
		shader.useShaderProgram();
		model = glm::translate(glm::mat4(1.0f), lampPosition[i]);
		glUniform1i(isLampLoc[i], true);
		glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
		// do not send the normal matrix if we are rendering in the depth map
		if (!depthPass) {
			normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
			glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
		}
		lamps.Draw(shader);
		glUniform1i(isLampLoc[i], false);
	}
}
void drawWaterDrops(gps::Shader shader, bool depthPass) {
	for (int i = 0; i < NR_WATER_DROPS; i++)
	{
		shader.useShaderProgram();
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(posWaterDrops[i].x, posWaterDrops[i].y, posWaterDrops[i].z));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
		// do not send the normal matrix if we are rendering in the depth map
		if (!depthPass) {
			normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
			glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
		}
		water_drop.Draw(shader);
		posWaterDrops[i] -= glm::vec3(0.0f, speedWaterDrops[i], 0.0f);
		if (posWaterDrops[i].y <= -2.0f)
		{
			posWaterDrops[i] = generatePozWaterDrops();
			speedWaterDrops[i] = generatspeedWaterDrops();
		}
	}
}
void drawClouds(gps::Shader shader, bool depthPass) {

	for (int i = 0; i < NR_CLOUDS; i++)
	{
		shader.useShaderProgram();
		model = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians(rotateClouds[i]),glm::vec3(0.0f, 1.0f ,0.0f));
		model = glm::translate(model, posClouds[i]);
		model = glm::scale(model, scaleClouds[i]);
		glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
		// do not send the normal matrix if we are rendering in the depth map
		if (!depthPass) {
			normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
			glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
		}
		cloud.Draw(shader);
		posClouds[i] += glm::vec3(cloudSpeed, 0.0f, cloudSpeed);
		if (posClouds[i].x >= 250.0f)
		{
			posClouds[i] = generateCloudPost();
			scaleClouds[i] = generateCloudScale();
			rotateClouds[i] = generateCloudRotate();
		}
	}
}
void drawWheel(CarAnimation currentCar,gps::Shader shader, bool depthPass,glm::vec3 deplasament,float rotation,float sens) {
	model = glm::mat4(1.0f);
	model = glm::translate(model, currentCar.current_car_pos + deplasament);
	model = glm::rotate(model, glm::radians(rotation), glm::vec3(0, 1.0f, 0));
	model = glm::rotate(model, glm::radians(sens * wheels_rotation), glm::vec3(0, 0, 1.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	// do not send the normal matrix if we are rendering in the depth map
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}
	wheels.Draw(shader);
}
glm::vec3 rotateVec(const glm::vec3& inputVector) {
	glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::vec4 rotatedVector = rotationMatrix * glm::vec4(inputVector.x, inputVector.y, inputVector.z, 1.0f);
	return glm::vec3(rotatedVector);
}
CarAnimation drawCar(CarAnimation myCar,gps::Shader shader, bool depthPass) {
	model = glm::mat4(1.0f);
	model = glm::translate(model, myCar.current_car_pos);
	model = glm::rotate(model, glm::radians(myCar.corner_rotation), glm::vec3(0, 1.0f, 0));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	// do not send the normal matrix if we are rendering in the depth map
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}
	car.Draw(shader);
	drawWheel(myCar, shader, depthPass, myCar.wheel_back_deplasament[0], myCar.unghiu_st, 1.0f);
	drawWheel(myCar, shader, depthPass, myCar.wheel_back_deplasament[1], myCar.unghiu_dr, -1.0f);
	drawWheel(myCar, shader, depthPass, myCar.wheel_front_deplasament[0], myCar.unghiu_st, 1.0f);
	drawWheel(myCar, shader, depthPass, myCar.wheel_front_deplasament[1], myCar.unghiu_dr, -1.0f);


	myCar.current_car_pos += myCar.direction * myCar.car_speed;
	if (!myCar.checkFirstcorner && myCar.current_car_pos.z >= myCar.corner_car_pos.z)
	{
		myCar.unghiu_dr = -180.0f;
		myCar.unghiu_st = 0.0f;
		myCar.corner_rotation = -90.0f;
		myCar.direction = glm::vec3(-1.0f, 0, 0);
		myCar.wheel_back_deplasament[0] = rotateVec(myCar.wheel_back_deplasament[0]);
		myCar.wheel_back_deplasament[1] = rotateVec(myCar.wheel_back_deplasament[1]);
		myCar.wheel_front_deplasament[0] = rotateVec(myCar.wheel_front_deplasament[0]);
		myCar.wheel_front_deplasament[1] = rotateVec(myCar.wheel_front_deplasament[1]);
		myCar.checkFirstcorner = true;
	}
	else if (myCar.checkFirstcorner && myCar.current_car_pos.x <= myCar.final_corner_pos.x)
	{
		myCar = initCarData(myCar.car_speed,myCar.init_car_pos, myCar.corner_car_pos, myCar.final_corner_pos);
	}
	return myCar;
}
void drawGround(gps::Shader shader, bool depthPass) {
	shader.useShaderProgram();
	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	// do not send the normal matrix if we are rendering in the depth map
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}

	ground.Draw(shader);
}
void drawObjects(gps::Shader shader, bool depthPass) {
	for (int i = 0; i < NR_CARS; i++){
		myCars[i] = drawCar(myCars[i], shader, depthPass);
	}
	if (rain) drawWaterDrops(shader, depthPass);
	drawTrees(shader, depthPass);
	drawWater(shader, depthPass);
	drawSea(shader, depthPass);
	if (!isNight) drawClouds(shader, depthPass);
	drawGround(shader, depthPass);
	drawLamps(shader, depthPass);
}

void initDaySkyBox()
{
	std::vector<const GLchar*> faces;
	faces.push_back("skybox/day/right.jpg");
	faces.push_back("skybox/day/left.jpg");
	faces.push_back("skybox/day/top.jpg");
	faces.push_back("skybox/day/bottom.jpg");
	faces.push_back("skybox/day/back.jpg");
	faces.push_back("skybox/day/front.jpg");
	daySkyBox.Load(faces);
}
void initNightSkyBox()
{
	std::vector<const GLchar*> faces;
	faces.push_back("skybox/night/right.jpg");
	faces.push_back("skybox/night/left.jpg");
	faces.push_back("skybox/night/top.jpg");
	faces.push_back("skybox/night/bottom.jpg");
	faces.push_back("skybox/night/back.jpg");
	faces.push_back("skybox/night/front.jpg");
	nightSkyBox.Load(faces);
}

bool changeGenerator() {
	int sansa = 5; 
	if (rand() % 100 + 1 <= sansa) return true;
	return false;
}

void renderShadows()
{
	depthMapShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"),
		1,
		GL_FALSE,
		glm::value_ptr(computeLightSpaceTrMatrix()));
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	drawObjects(depthMapShader,true);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void renderScene() {

	wheels_rotation += 3.0f;
	water_rotation += 0.5f;
	sea_rotation += 0.05f;
	presentationAnimation();

	renderShadows();

	// final scene rendering pass (with shadows)

	glViewport(0, 0, retina_width, retina_height);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	myCustomShader.useShaderProgram();

	if (isThunder)
	{
		myCustomShader.useShaderProgram();
		glUniform1i(isThorLoc, changeGenerator());
	}

	if (!isNight)
	{
		lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
		glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
	}
	else
	{
		lightColor = glm::vec3(0.5f, 0.5f, 0.5f); //gray light
		glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
	}

	view = myCamera.getViewMatrix();
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

	//bind the shadow map
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "shadowMap"), 3);

	glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "lightSpaceTrMatrix"),
		1,
		GL_FALSE,
		glm::value_ptr(computeLightSpaceTrMatrix()));

	drawObjects(myCustomShader, false);

	lightShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
	model = lightRotation;
	model = glm::translate(model, glm::vec3(210.0f, 333.0f, 450.0f));
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniform1i(isNightLoc, isNight);
	sun.Draw(lightShader);

	if (isNight) mySkyBox = nightSkyBox;
	else mySkyBox = daySkyBox;

	mySkyBox.Draw(skyboxShader, view, projection);
}

void cleanup() {
	glDeleteTextures(1,& depthMapTexture);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &shadowMapFBO);
	glfwDestroyWindow(glWindow);
	//close GL context and any other GLFW resources
	glfwTerminate();
}

int main(int argc, const char * argv[]) {

	if (!initOpenGLWindow()) {
		glfwTerminate();
		return 1;
	}

	initOpenGLState();
	initObjects();
	initShaders();
	initUniforms();
	initFBO();

	initDaySkyBox();
	initNightSkyBox();

	while (!glfwWindowShouldClose(glWindow)) {
		processMovement();
		renderScene();		
		glfwPollEvents();
		glfwSwapBuffers(glWindow);
	}

	cleanup();

	return 0;
}