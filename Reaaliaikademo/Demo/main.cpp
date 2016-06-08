// Std. Includes
#include <string>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GL includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"

// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other Libs
#include "lodepng.h"

// Properties
GLuint WIDTH = 800, HEIGHT = 600;

GLuint planeVAO = 0, planeVBO; // quadVAO = 0, quadVBO;// , cubeVAO = 0, cubeVBO = 0;

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void Do_Movement();
GLuint loadTexture(GLchar* path);
void RenderQuad();
void RenderPlane();

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

bool normalMapping = true, inTangentSpace = true;

int main()
{
	// Init GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "ReaaliaikaDemo", nullptr, nullptr); // Windowed
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// Options
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Initialize GLEW to setup the OpenGL Function pointers
	glewExperimental = GL_TRUE;
	glewInit();

	// Define the viewport dimensions
	glViewport(0, 0, WIDTH, HEIGHT);

	// Setup some OpenGL options
	glEnable(GL_DEPTH_TEST);

	// Setup and compile our shaders

	GLuint diffuseMap = loadTexture("../resource/176.png");
	GLuint normalMap = loadTexture("../resource/176_norm.png");
	//Shader modelShader("../resource/shader.vs", "../shader.frag");
	Shader shader("../resource/normalMapping.vs", "../resource/normalMapping.frag");
	// Light position
	glm::vec3 lightPos(1.2f, 1.0f, 1.0f);

	glm::mat4 NMShader_modelMatrix, NMShader_modelMatrix_lightSource;
	NMShader_modelMatrix_lightSource = glm::translate(glm::mat4(), lightPos);
	NMShader_modelMatrix_lightSource = glm::scale(NMShader_modelMatrix_lightSource, glm::vec3(0.1f));
	NMShader_modelMatrix = glm::scale(NMShader_modelMatrix, glm::vec3(0.2f, 0.2f, 0.2f));
	// Set texture units 
	shader.Use();
	glUniform1i(glGetUniformLocation(shader.Program, "diffuseMap"), 0);
	glUniform1i(glGetUniformLocation(shader.Program, "normalMap"), 1);
	glUniform3fv(glGetUniformLocation(shader.Program, "lightPosition"), 1, glm::value_ptr(lightPos));

	GLuint NMShader_modelMatrix_Loc = glGetUniformLocation(shader.Program, "modelMatrix");
	GLuint NMShader_projMatrix_Loc = glGetUniformLocation(shader.Program, "projectionMatrix");
	GLuint NMShader_viewMatrix_Loc = glGetUniformLocation(shader.Program, "viewMatrix");
	GLuint NMShader_cameraPosition_Loc = glGetUniformLocation(shader.Program, "cameraPosition");
	GLuint NMShader_normalMapping_Loc = glGetUniformLocation(shader.Program, "normalMapping");
	GLuint NMShader_inTangentSpace_Loc = glGetUniformLocation(shader.Program, "inTangentSpace");

	Model ourModel("../resource/EM/EM-208.obj", true);
	//renderModel(shader);
	//renderCube();

	while (!glfwWindowShouldClose(window))
	{
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;  //the time it takes to render the last frame
		lastFrame = currentFrame;
		//countFPS(currentFrame);
		glfwPollEvents(); //This function processes only those events that are already in the event queue and then returns immediately.Processing events will cause the window and input callbacks associated with those events to be called.
		Do_Movement();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		NMShader_modelMatrix_lightSource = glm::rotate(NMShader_modelMatrix_lightSource, (GLfloat)glfwGetTime() * -0.01f, glm::normalize(glm::vec3(1.0, 0.0, 1.0)));

		shader.Use();
		glm::mat4 viewMatrix = camera.GetViewMatrix();
		glm::mat4 projMatrix = glm::perspective(camera.Zoom, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
		glUniformMatrix4fv(NMShader_viewMatrix_Loc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(NMShader_projMatrix_Loc, 1, GL_FALSE, glm::value_ptr(projMatrix));
		// Render normal-mapped quad
		//NMShader_modelMatrix = glm::rotate(glm::mat4(), glm::radians((GLfloat)glfwGetTime()*-10), glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f))); // Rotates the quad to show normal mapping works in all directions
		glUniformMatrix4fv(NMShader_modelMatrix_Loc, 1, GL_FALSE, glm::value_ptr(NMShader_modelMatrix));
		glUniform3fv(NMShader_cameraPosition_Loc, 1, glm::value_ptr(camera.Position));
		glUniform1i(NMShader_normalMapping_Loc, normalMapping);
		glUniform1i(NMShader_inTangentSpace_Loc, inTangentSpace);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, normalMap);
		RenderPlane();

		// Draw robot model
		ourModel.Draw(shader);

		// render light source (simply re-renders a smaller plane at the light's position for debugging/visualization)
		glUniformMatrix4fv(NMShader_modelMatrix_Loc, 1, GL_FALSE, glm::value_ptr(NMShader_modelMatrix_lightSource));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, normalMap);
		// Render Depth map to quad
		RenderPlane();

		glfwSwapBuffers(window); //swaps the front and back buffers of the specified window
	}
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glDeleteTextures(1, &diffuseMap);
	//glDeleteTextures(1, &normalMap);
	glfwTerminate(); //Terminate GLFW. This function destroys all remaining windows and cursors, restores any modified gamma ramps and frees any other allocated resources. 
	return 0;

}
// This function loads a texture from file. Note: texture loading functions like these are usually 
// managed by a 'Resource Manager' that manages all resources (like textures, models, audio). 
// For learning purposes we'll just define it as a utility function.
GLuint loadTexture(GLchar* path)
{
	//Generate texture ID and load texture data 
	GLuint textureID;
	glGenTextures(1, &textureID);
	std::vector<unsigned char> image, png; //the raw pixels
	unsigned width, height;
	lodepng::load_file(png, path);
	//decode
	unsigned error = lodepng::decode(image, width, height, png);
	if (error) // lataus ei onnistu --> virheilmotus
	{
		std::cout << "PNG load failed " << error << ": " << lodepng_error_text(error) << std::endl;
	}
	// Assign texture to ID
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	image.clear();
	return textureID;
}

void RenderPlane()
{
	if (planeVAO == 0)
	{
		//Positions
		glm::vec3 position1(-2.0f, 0.0f, 0.0f);
		glm::vec3 position2(-2.0f, -2.0f, 0.0f);
		glm::vec3 position3(0.0f, -2.0f, 0.0f);
		glm::vec3 position4(0.0f, 0.0f, 0.0f);
		//Texture coordinates
		glm::vec2 uv1(0.0, 1.0);
		glm::vec2 uv2(0.0, 0.0);
		glm::vec2 uv3(1.0, 0.0);
		glm::vec2 uv4(1.0, 1.0);
		//normal vector
		glm::vec3 nm(0.0f, 0.0f, 1.0f);

		// calculate tangent/bitangent vectors of both triangles
		glm::vec3 tangent1, tangent2, bitangent1, bitangent2;
		// - triangle 1
		glm::vec3 edge1 = position2 - position1;
		glm::vec3 edge2 = position3 - position1;
		glm::vec2 deltaUV1 = uv2 - uv1;
		glm::vec2 deltaUV2 = uv3 - uv1;

		GLfloat f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		tangent1.x = f * (deltaUV2.y*edge1.x - deltaUV1.y*edge2.x);
		tangent1.y = f * (deltaUV2.y*edge1.y - deltaUV1.y*edge2.y);
		tangent1.z = f * (deltaUV2.y*edge1.z - deltaUV1.y*edge2.z);
		tangent1 = glm::normalize(tangent1);

		bitangent1.x = f * (-deltaUV2.x*edge1.x + deltaUV1.x*edge2.x);
		bitangent1.y = f * (-deltaUV2.x*edge1.y + deltaUV1.x*edge2.y);
		bitangent1.z = f * (-deltaUV2.x*edge1.z + deltaUV1.x*edge2.z);
		bitangent1 = glm::normalize(bitangent1);

		// - triangle 2
		edge1 = position3 - position1;
		edge2 = position4 - position1;
		deltaUV1 = uv3 - uv1;
		deltaUV2 = uv4 - uv1;

		f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		tangent2.x = f * (deltaUV2.y*edge1.x - deltaUV1.y*edge2.x);
		tangent2.y = f * (deltaUV2.y*edge1.y - deltaUV1.y*edge2.y);
		tangent2.z = f * (deltaUV2.y*edge1.z - deltaUV1.y*edge2.z);
		tangent2 = glm::normalize(tangent2);

		bitangent2.x = f * (-deltaUV2.x*edge1.x + deltaUV1.x*edge2.x);
		bitangent2.y = f * (-deltaUV2.x*edge1.y + deltaUV1.x*edge2.y);
		bitangent2.z = f * (-deltaUV2.x*edge1.z + deltaUV1.x*edge2.z);
		bitangent2 = glm::normalize(bitangent2);

		GLfloat planeVertices[] = {
			// Positions       // Normals          // Texture Coords 
			position1.x, position1.y, position1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
			position2.x, position2.y, position2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
			position3.x, position3.y, position3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,

			position1.x, position1.y, position1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
			position3.x, position3.y, position3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
			position4.x, position4.y, position4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z
		};

		glGenVertexArrays(1, &planeVAO);
		glGenBuffers(1, &planeVBO);
		glBindVertexArray(planeVAO);
		glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(11 * sizeof(GLfloat)));
		glEnableVertexAttribArray(4);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	glBindVertexArray(planeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

#pragma region "User input"

bool keys[1024];
bool keysPressed[1024];
// Moves/alters the camera positions based on user input
void Do_Movement()
{
	// Camera controls
	if (keys[GLFW_KEY_W])
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S])
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A])
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D])
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key >= 0 && key <= 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
		{
			keys[key] = false;
			keysPressed[key] = false;
		}
	}
}

GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;
// Moves/alters the camera positions based on user input
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

#pragma endregion