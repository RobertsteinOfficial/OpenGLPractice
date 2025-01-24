#include "Renderer.h"

#include <iostream>

//Chiamo glGetError fino a che non mi tira fuori tutti gli errori, in modo da pulire tutto
void GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char* function, const char* file, int line)
{
	while (GLenum error = glGetError())
	{
		std::cout << "[OpenGL Error] (" << error << "):"
			<< " " << file << ":" << line << std::endl;
		return false;
	}

	return true;
}