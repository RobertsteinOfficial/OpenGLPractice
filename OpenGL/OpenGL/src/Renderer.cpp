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

void Renderer::Clear() const
{
	GLCall(glClear(GL_COLOR_BUFFER_BIT));
}

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const
{

	shader.Bind();
	va.Bind();
	//Non dovrebbe servire bindare ib, perchè è già bindato su va
	//ib.Bind();

	//Per usare l'index buffer invece di glDrawArrays uso glDrawElements
	//Al momento assumo che l'index buffer lavori solo con uint e quindi l'ho hardcodato. Se per esempio 
	//usassi anche ushort dovrei tirarmelo fuori dal buffer
	GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
}
