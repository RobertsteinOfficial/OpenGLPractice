#include "IndexBuffer.h"

#include "Renderer.h"

//ATTENZIONE: usare sempre uint per index buffer
IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int count) : m_Count(count)
{
	//Vedi commento chiamata glBufferData
	ASSERT(sizeof(unsigned int) == sizeof(GLuint));

	//Definisco un index buffer
	GLCall(glGenBuffers(1, &m_RendererID));

	//Seleziono (bind) il buffer
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));

	//Fillo il buffer. Posso anche allocare solo e fillare poi, in caso
	//Dovrei usare GLuint per la size, perchè potrei teoricamente trovarmi su una piattaforma in cui 
	//uint non è 4 byte, ma credo sia una cosa abbastanza rara. Magari metto un ASSERT in cima
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW));
}

IndexBuffer::~IndexBuffer()
{
	GLCall(glDeleteBuffers(1, &m_RendererID));
}

void IndexBuffer::Bind() const
{
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));
}

void IndexBuffer::Unbind() const
{
	//Con 0 fa l'unbind
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}
