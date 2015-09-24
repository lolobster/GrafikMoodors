#define GLM_FORCE_RADIANS

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

#include <cstdio>
#include <cstdlib>
//using namespace glm;
#include <common/shader.hpp>
#include <common/texture.hpp>
#include <iostream>

#include "vertexshaderx.h"

namespace{
	GLuint programID;
	GLuint Texture;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

const GLuint WIDTH = 800, HEIGHT = 600;

int main()
{
	glfwInit();

	// asetetaan GLFW asetukset

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);



	// luodaan ikkuna

	GLFWwindow* window;
	window = glfwCreateWindow(WIDTH, HEIGHT, "VITUNNELIOT", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, key_callback);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Init GLEW
	glewInit();

	glViewport(0, 0, WIDTH, HEIGHT);

	programID = LoadShaders("VertexShader.vertexshader", "FragmentShader.fragmentshader");

	// tehään ne vitun neliöt

	GLfloat neliö1[] = {
		// positions		// Värit			// Texture Coords
		0.5f,  0.5f, 0.0f,	1.0f, 0.0f, 0.0f,	1.0f, 1.0f, // Top Right
		0.5f, -0.5f, 0.0f,	0.0f, 1.0f, 0.0f,	1.0f, 0.0f, // Bottom Right
	   -0.5f, -0.5f, 0.0f,	0.0f, 0.0f, 1.0f,	0.0f, 0.0f, // Bottom Left
	   -0.5f,  0.5f, 0.0f,	1.0f, 1.0f, 0.0f,	0.0f, 1.0f	// Top Left
	};

	GLuint indices1[] = {
		0, 1, 3, // eka kolomio
		1, 2, 3 // toka kolomio
	};

	


	GLuint VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(neliö1), neliö1, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices1), indices1, GL_STATIC_DRAW);

	// positiot
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// värit
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// textuurien koordinaatit
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Unbind VAO


	// ladataan ja sähelletään textuurin kanssa
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture); // kaikki GL_TEXTURE_2D säädöt säätää nyt tätä objektia

	// textuurin wrapping parametrit
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // Set texture wrapping to GL_REPEAT (usually basic wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// textuurin filtteröintiparametrit
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// ladataan tekstuuri
	Texture = loadBMP_custom("BM_pattern.bmp");

	glBindTexture(GL_TEXTURE_2D, 0);


	////// ITE TOIMINTALOOPPI////////

	while (!glfwWindowShouldClose(window))
	{
		// tarkistetaan eventit
		glfwPollEvents();

		// render
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// bindataan tekstuuri
		glBindTexture(GL_TEXTURE_2D, Texture);

		// aktivoidaan shader
		glUseProgram(programID);

		glm::vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);
		glm::mat4 trans;
		mitäkosmistavittuamiksieikäännyDUDUDUDUDUUU//trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
		trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));
		vec = trans * vec;
		std::cout << std::endl << vec.x << vec.y << vec.z << std::endl;
		GLuint transformLoc = glGetUniformLocation(programID, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

		// piirretään container
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// vaihetaan screen bufferi
		glfwSwapBuffers(window);
	}


	// vapautetaan resursseja
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	// terminaattoroidaan GLFW, vapautetaan kaikki GLFW resurssit
	glfwTerminate();
	return 0;
}

// kututaan aina nappia painettaessa
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}


// http://learnopengl.com/#!Getting-started/Textures