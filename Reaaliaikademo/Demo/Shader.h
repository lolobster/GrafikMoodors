#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <GL/glew.h>

class Shader
{
public:
	GLuint Program;
	// Constructor generates the shader on the fly
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* geometryPath = "")
	{
		// 1. Retrieve the vertex/fragment source code from filePath
		std::string vertexCode;
		std::string fragmentCode;
		std::string geometryCode;
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;
		std::ifstream gShaderFile;
		// ensures ifstream objects can throw exceptions:
		vShaderFile.exceptions(std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::badbit);
		gShaderFile.exceptions(std::ifstream::badbit);
		try
		{
			// Open files
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);
			std::stringstream vShaderStream, fShaderStream;
			// Read file's buffer contents into streams
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();
			// close file handlers
			vShaderFile.close();
			fShaderFile.close();
			// Convert stream into string
			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
			if (geometryPath != "")
			{
				gShaderFile.open(geometryPath);
				std::stringstream gShaderStream;
				gShaderStream << gShaderFile.rdbuf();
				gShaderFile.close();
				geometryCode = gShaderStream.str();
			}
		}
		catch (std::ifstream::failure e)
		{
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		}

#pragma region CreateShadersAndProgram////////////////////////////////////////////////////
		//Vertex shader//////////////////////////////////////////////////////////////////////
		const GLchar* vertexShaderSource = vertexCode.c_str();
		GLuint vertexShader; //vertex shader ID
		vertexShader = glCreateShader(GL_VERTEX_SHADER); //creates an empty shader object behind the scenes and returns an unsigned reference ID to this shader object for further use
		glShaderSource(vertexShader, 1, &vertexShaderSource, NULL); //replaces the source code of a given shader object
		glCompileShader(vertexShader); //compiles the given shader object with the attached source of the shader object
		GLint success;
		GLchar infoLog[512];
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success); //Returns a parameter from a shader object
		if (!success)
		{
			glGetShaderInfoLog(vertexShader, 512, NULL, infoLog); //returns the information log for a shader object
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		}

		//Fragment shader//////////////////////////////////////////////////////////////////////
		const GLchar* fragmentShaderSource = fragmentCode.c_str();
		GLuint fragmentShader; //vertex shader ID
		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER); //creates an empty shader object behind the scenes and returns an unsigned reference ID to this shader object for further use
		glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL); //replaces the source code of a given shader object
		glCompileShader(fragmentShader); //compiles the given shader object with the attached source of the shader object
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success); //Returns a parameter from a shader object
		if (!success)
		{
			glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog); //returns the information log for a shader object
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		}

		//Geometry shader//////////////////////////////////////////////////////////////////////
		GLuint geometryShader; //geometry shader ID
		if (geometryPath != "")
		{
			const GLchar* geometryShaderSource = geometryCode.c_str();
			geometryShader = glCreateShader(GL_GEOMETRY_SHADER); //creates an empty shader object behind the scenes and returns an unsigned reference ID to this shader object for further use
			glShaderSource(geometryShader, 1, &geometryShaderSource, NULL); //replaces the source code of a given shader object
			glCompileShader(geometryShader); //compiles the given shader object with the attached source of the shader object
			glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success); //Returns a parameter from a shader object
			if (!success)
			{
				glGetShaderInfoLog(geometryShader, 512, NULL, infoLog); //returns the information log for a shader object
				std::cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << infoLog << std::endl;
			}
		}

		//Shader program//////////////////////////////////////////////////////////////////////
		this->Program = glCreateProgram(); //creates a program object and returns a refernce ID
		glAttachShader(this->Program, vertexShader); //attaches a compiled shader object to a shader program
		glAttachShader(this->Program, fragmentShader); //attaches a compiled shader object to a shader program
		if (geometryPath != "")
		{
			glAttachShader(this->Program, geometryShader); //attaches a compiled shader object to a shader program
		}
		glLinkProgram(this->Program); //links all the attached shaders in one final shader program object
		glGetProgramiv(this->Program, GL_LINK_STATUS, &success); //Returns a parameter from a program object
		if (!success)
		{
			glGetProgramInfoLog(this->Program, 512, NULL, infoLog); //returns the information log for a program object
			std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		}
		glDeleteShader(vertexShader); //removes the given shader object, cleaning all allocated memory
		glDeleteShader(fragmentShader); //removes the given shader object, cleaning all allocated memory
		if (geometryPath != "")
		{
			glDeleteShader(geometryShader);
		}
#pragma endregion CreateShadersAndProgram////////////////////////////////////////////////////

	}
	~Shader()
	{
		glDeleteProgram(this->Program);
	}
	// Uses the current shader
	void Use()
	{
		glUseProgram(this->Program);
	}
};

#endif