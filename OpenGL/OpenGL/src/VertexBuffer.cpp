#include "VertexBuffer.h"

#include "Renderer.h"

VertexBuffer::VertexBuffer(const void* data, unsigned int size)
{
	//Definisco un vertex buffer
	GLCall(glGenBuffers(1, &m_RendererID));

	//Seleziono (bind) il buffer
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));

	//Fillo il buffer. Posso anche allocare solo e fillare poi, in caso
	GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
}

VertexBuffer::~VertexBuffer()
{
	GLCall(glDeleteBuffers(1, &m_RendererID));
}

void VertexBuffer::Bind() const
{
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
}

void VertexBuffer::Unbind() const
{
	//Con 0 fa l'unbind
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}
