#pragma once
#include <fstream>
#include <string>
#include "defs.h"

class Shader {

public:
	Shader(const char* vertexPath, const char* fragmentPath)
	{
		Set(vertexPath, fragmentPath);
	}
	Shader() {

	}

	void Set(const char* vertexPath, const char* fragmentPath)
	{
		unsigned int shaderProgram;
		shaderProgram = glCreateProgram();

		loadShader(vertexPath, true);
		loadShader(fragmentPath, false);

		glAttachShader(shaderProgram, vertex);
		glAttachShader(shaderProgram, fragment);
		glLinkProgram(shaderProgram);

		shaderId = shaderProgram;
	}

	void use()
	{
		glUseProgram(shaderId);
	}

	void setBool(const std::string& name, bool value) const
	{
		glUniform1i(glGetUniformLocation(shaderId, name.c_str()), (int)value);
	}
	void setInt(const std::string& name, int value) const
	{
		glUniform1i(glGetUniformLocation(shaderId, name.c_str()), value);
	}
	void setFloat(const std::string& name, float value) const
	{
		glUniform1f(glGetUniformLocation(shaderId, name.c_str()), value);
	}

	void setMatrix4x4(const std::string& name, glm::mat4 value) const
	{
		glUniformMatrix4fv(glGetUniformLocation(shaderId, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
	}

	void setVector3(const std::string& name, glm::vec3 value) const
	{
		glUniform3fv(glGetUniformLocation(shaderId, name.c_str()), 1, glm::value_ptr(value));
	}

private:
	unsigned int vertex;
	unsigned int fragment;
	unsigned int shaderId;

	void loadShader(const char* path, bool isVertex)
	{
		unsigned int shader;
		if (isVertex)
		{
			shader = glCreateShader(GL_VERTEX_SHADER);
		}
		else
		{
			shader = glCreateShader(GL_FRAGMENT_SHADER);
		}
		std::string fileContents;
		std::ifstream file(path);

		std::string line = "";
		while (!file.eof())
		{
			std::getline(file, line);
			fileContents.append(line + "\n");
		}
		const char* contents = fileContents.c_str();
		file.close();
		glShaderSource(shader, 1, &contents, NULL);
		glCompileShader(shader);
		int success;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		char infoLog[512];
		if (!success)
		{
			glGetShaderInfoLog(shader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
		if (isVertex)
		{
			vertex = shader;
		}
		else
		{
			fragment = shader;
		}
	}
};

