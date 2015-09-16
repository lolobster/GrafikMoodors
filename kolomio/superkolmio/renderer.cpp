#define GLM_FORCE_RADIANS
#define GLFW_DLL

#include <cstdio>
#include <cstdlib>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
//using namespace glm;
#include <common/shader.hpp>
#include <common/texture.hpp>
#include <iostream>

#include "common\shader.hpp"
#include "renderer.h"
#include "vertexshaderx.h"

//Hide from other modules
namespace {
	GLuint programID;
	GLuint vertexbuffer;
	GLuint VertexArrayID;
	GLuint indexbuffer;
	GLFWwindow* window;
	float alpha = 0.0f;
	glm::mat4 MVP(1.0);
	GLuint MVP_MatrixID;
	GLuint TextureID;
	GLuint uvbuffer;
	GLuint Texture;
	glm::vec2 wh;
	GLuint wh_VectorID;
};


void renderer::FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	wh = glm::vec2(width, height);
	//std::cout<<width<<"x"<<height<<" "<<w<<"x"<<h<<"\n";
}


void renderer::Render(void) {
	const int N_repeat = 17;


	glm::vec3 x_axis(1.0, 0.0, 0.0);
	glm::vec3 y_axis(0.0, 1.0, 0.0);
	glm::vec3 z_axis(0.0, 0.0, 1.0);

	glm::vec3 cam_pos(0, 0, 0);
	glm::vec3 cam_up = y_axis;
	glm::vec3 cam_right = x_axis;
	glm::vec3 cam_front = -z_axis; //oikeakatinen koordinaatisto
	glm::mat4 P = glm::lookAt(cam_pos, cam_pos + cam_front, cam_up);


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(programID);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture);
	glUniform1i(TextureID, 0);

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glm::mat4 V = glm::ortho(-1.0f, 1.0f, -1.0f*height / width, 1.0f*height / width);
	glm::mat4 VP = V*P;
	for (int j = 0; j < N_repeat; j++) {
		glm::mat4 M = glm::rotate(alpha + j * 2.0f * 3.14159265f / N_repeat, z_axis) * glm::translate(glm::vec3(0.0, 0.3, 0)) * glm::rotate(alpha, z_axis)*glm::scale(glm::vec3(0.75));
		//glm::mat4 M=glm::translate(glm::vec3(0.0, 0.3, 0))*glm::rotate(alpha, z_axis)*glm::scale(glm::vec3(0.75));
		MVP = VP*M;
		glUniformMatrix4fv(MVP_MatrixID, 1, GL_FALSE, &MVP[0][0]);
		glUniform2fv(wh_VectorID, 1, &wh[0]);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			VERTEX_POSITION, //layout in the shader.
			3,       // size
			GL_FLOAT,// type
			GL_FALSE,// normalized
			0,       // stride
			(void*)0 // array buffer offset
			);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glVertexAttribPointer(
			TEXTURE_DATA,
			2,	//size: U+V => 2
			GL_FLOAT,	//type
			GL_FALSE,	// normalized?
			0,			// stride
			(void*)0	//array buffer offset
			);
		glDrawElements(GL_TRIANGLES, 3,
			GL_UNSIGNED_BYTE, (GLvoid*)0);
	}

	/*	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glVertexAttribPointer(
	VERTEX_COLOR,       // must match the layout in the shader.
	3,       // size
	GL_FLOAT,// type
	GL_FALSE,// normalized?
	0,       // stride
	(void*)0 // array buffer offset
	);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	}
	*/

	alpha += 0.005f;
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glfwSwapBuffers(window);
}

void renderer::Init(GLFWwindow* w) {
	window = w;
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);

	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	programID = LoadShaders("VertexShader.vertexshader", "FragmentShader.fragmentshader");
	MVP_MatrixID = glGetUniformLocation(programID, "MVP");
	wh_VectorID = glGetUniformLocation(programID, "wh");

	static const GLfloat g_vertex_buffer_data[] = {
		-0.1f, -0.2f, -0.85f,
		0.85f, 0.0f, -0.2f,
		0.1f, 0.1f, -0.9f,
	};

	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	static const GLubyte g_indices[] = {
		0, 1, 2,
	};
	/*	static const GLfloat g_color_buffer_data[] = {
	1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f,
	};
	*/
	glGenBuffers(1, &indexbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(g_indices), g_indices, GL_STATIC_DRAW);

	static const GLfloat g_uv_buffer_data[] = {
		0.0, 0.0,
		1.0, 0.0,
		1.0, 1.0,
	};
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_data),
		g_uv_buffer_data, GL_STATIC_DRAW);
	TextureID = glGetUniformLocation(programID,
		"myTextureSampler");

	Texture = loadBMP_custom("./uvtemplate.bmp");
}

void renderer::Uninit(void) {
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteProgram(programID);
}
