#define GLEW_STATIC
#define GLM_FORCE_RADIANS
#include <cstdio>
#include <cstdlib>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
using namespace glm;
#include <common/shader.hpp>
#include <iostream>
#include <string>

GLFWwindow* window;
GLuint programID;
GLuint vertexbuffer;
GLuint VertexArrayID;
GLuint colorbuffer;


void Render(void) {
  glClear( GL_COLOR_BUFFER_BIT );
  glUseProgram(programID);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
  glVertexAttribPointer(
			0,       // must match the layout in the shader.
			3,       // size
			GL_FLOAT,// type
			GL_FALSE,// normalized
			0,       // stride
			(void*)0 // array buffer offset
			);

  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
  glVertexAttribPointer(
			1,         // must match the layout in the shader.
			3,         // size
			GL_FLOAT,  // type
			GL_FALSE,  // normalized?
			0,         // stride
			(void*)0   // array buffer offset
			);
  glDrawArrays(GL_TRIANGLES, 0, 3);
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glfwSwapBuffers(window);
}

void Init(void) {
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  glGenVertexArrays(1, &VertexArrayID);
  glBindVertexArray(VertexArrayID);

  programID = LoadShaders(
			  "VertexShader.vertexshader",
			  "FragmentShader.fragmentshader" );
 
  static const GLfloat g_vertex_buffer_data[] = { 
    -0.5f, -0.5f, 0.0f, 0.5f,  0.0f, 0.0f,
    0.0f,  0.5f, 0.0f, };

  glGenBuffers(1, &vertexbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
  glBufferData(GL_ARRAY_BUFFER,
	       sizeof(g_vertex_buffer_data),
	       g_vertex_buffer_data, GL_STATIC_DRAW);
  static const GLfloat g_color_buffer_data[] = { 
    1.0f,  0.0f, 0.0f,
    0.0f,  1.0f, 0.0f,
    0.0f,  0.0f, 1.0f,
  };
  glGenBuffers(1, &colorbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
  glBufferData(GL_ARRAY_BUFFER,
	       sizeof(g_color_buffer_data),
	       g_color_buffer_data, GL_STATIC_DRAW);
}

void Uninit(void) {
  glDeleteBuffers(1, &vertexbuffer);
  glDeleteBuffers(1, &colorbuffer);
  glDeleteVertexArrays(1, &VertexArrayID);
  glDeleteProgram(programID);
}


void PrintVec(const vec4 &v) {

	std::cout << std::endl;

	for(int i = 0; i < 4; i++) {
		std::cout << v[i] << "\n";
	}
		std::cout << "\n";
	
}

void PrintMatrix(const mat4 &m) {

	std::cout << std::endl;

	for(int i = 0; i < 4; i++) {
		for(int j = 0; j < 4; j++) {
			std::cout << m[j][i] << " ";
		}
		std::cout << "\n";
	}
	std::cout << "\n";
}

int main( void ) {
  if( !glfwInit() )
    {
      fprintf( stderr, "Failed to initialize GLFW\n" );
      return -1;
    }

  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE,
                 GLFW_OPENGL_CORE_PROFILE);

  window = glfwCreateWindow( 1024, 768,
			     "Tutorial 02 - Red triangle", NULL, NULL);
  if( window == NULL ){
    fprintf( stderr, "Failed to open GLFW window.");
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glewExperimental = true; // Needed for core profile
  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW\n");
    return -1;
  }

  ////////////////////////////				MATRIISIHÄSSÄKKÄ   EX 23

  std::cout << std::endl;
  std::cout << "EX 23: MATRIISIHÄSSÄKKÄ" << std::endl;
  std::cout << std::endl;

  mat4 mat1(1.0, 0.0, 2.0, 2.0,
	  0.0, 1.0, 0.0, 0.0,
	  1.0, 1.0, 1.0, 2.0,
	  0.0, 0.0, 1.0, 0.0);

  mat4 mat2(0.0, 0.0, 0.0, 2.0,
	  1.0, 1.0, 0.0, 0.0,
	  1.0, 1.0, 0.0, 2.0,
	  0.0, 0.0, 1.0, 0.0);

  vec4 vekki(3.0, 4.0, -2.0, 1);

  vec4 tulosvekki = mat1* mat2 *vekki;

  std::cout << "Lasketaan matriiseja :DD vektoriksi :DDD" << std::endl;
  std::cout << std::endl;

  for (int i = 0; i < 4; i++)
  {
	  std::cout << tulosvekki[i];
	  std::cout << std::endl;
  }

  //////////////////////////////

  /////////////////////////			MUUNNOSMATRIISI   EX 25

  std::cout << std::endl;
  std::cout << "EX 25: MUUNNOSMATRIISIN LASKU" << std::endl;
  std::cout << std::endl;

  vec4 p1(0.0, 0.0, 0.0, 1.0);
  vec4 p2(1.0, 0.0, 0.0, 1.0);
  vec4 p3(0.0, 1.0, 0.0, 1.0);

  vec3 x_axis(1.0, 0.0, 0.0);
  vec3 y_axis(0.0, 1.0, 0.0);
  vec3 z_axis(0.0, 0.0, 1.0);

  vec3 s(0.3); // skaalaus
  vec3 t(-2.0, -1.0, 0.0); // siirto
  vec3 r = z_axis; //kierto

  mat4 R = rotate(3.14159265f / 6, r);
  mat4 S = scale(s);
  mat4 T = translate(t);

  mat4 T_total = T*S*R;

  PrintVec(T_total * p1);
  PrintVec(T_total * p2);
  PrintMatrix(T_total);

  ///////////////////////////

  ////////////////////////////  KAMERAN SIIRTO  EX 26

  std::cout << std::endl;
  std::cout << "EX 26: KAMERAN SIIRTO" << std::endl;
  std::cout << std::endl;

  float v_width = 8.0;
  float v_height = 6.0;

  vec3 cam_pos(1.2, 0.1, 0.0);
  vec3 cam_up = y_axis;
  vec3 cam_right = x_axis;
  vec3 cam_front = z_axis;

  mat4 C = lookAt(cam_pos, cam_pos + cam_front, cam_up);
  T_total = C*T_total;

  PrintVec(T_total*p1);
  PrintVec(T_total*p2);
  PrintMatrix(T_total);

  ////////////////////////////////

  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
  Init();

  do{
    Render();
    glfwPollEvents();
  }
  while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
	 glfwWindowShouldClose(window) == 0 );

  glfwTerminate();

  return 0;
}
