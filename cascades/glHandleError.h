#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

inline void glHandleError(const char* info)
{
	GLenum err = glGetError();

	if (err != GL_NO_ERROR)
	{
		char* fill = "";
		if (strlen(info) > 0)
		{
			std::cout << info << ":\n";
			fill = "\t";
		}

		while (err != GL_NO_ERROR)
		{
			std::cout << fill;
			switch (err)
			{
			case GL_INVALID_ENUM: std::cout << fill << "GL_INVALID_ENUM\n";
				break;
			case GL_INVALID_VALUE: std::cout << "GL_INVALID_VALUE\n";
				break;
			case GL_INVALID_OPERATION: std::cout << "GL_INVALID_OPERATION\n";
				break;
			case GL_INVALID_FRAMEBUFFER_OPERATION: std::cout << "GL_INVALID_FRAMEBUFFER_OPERATION\n";
				break;
			case GL_OUT_OF_MEMORY: std::cout << "GL_OUT_OF_MEMORY\n";
				break;
			case GL_STACK_UNDERFLOW: std::cout << "GL_STACK_UNDERFLOW\n";
				break;
			case GL_STACK_OVERFLOW: std::cout << "GL_STACK_OVERFLOW\n";
				break;
			}
			err = glGetError();
		}
	}
}

inline void glHandleError(const char * file, int line)
{
	GLenum err = glGetError();

	if (err != GL_NO_ERROR)
	{
		std::cout << file << ":" << line;

		while (err != GL_NO_ERROR)
		{
			std::cout << std::endl << "\t";
			switch (err)
			{
			case GL_INVALID_ENUM: std::cout << "GL_INVALID_ENUM\n";
				break;
			case GL_INVALID_VALUE: std::cout << "GL_INVALID_VALUE\n";
				break;
			case GL_INVALID_OPERATION: std::cout << "GL_INVALID_OPERATION\n";
				break;
			case GL_INVALID_FRAMEBUFFER_OPERATION: std::cout << "GL_INVALID_FRAMEBUFFER_OPERATION\n";
				break;
			case GL_OUT_OF_MEMORY: std::cout << "GL_OUT_OF_MEMORY\n";
				break;
			case GL_STACK_UNDERFLOW: std::cout << "GL_STACK_UNDERFLOW\n";
				break;
			case GL_STACK_OVERFLOW: std::cout << "GL_STACK_OVERFLOW\n";
				break;
			}
			err = glGetError();
		}
	}
}
