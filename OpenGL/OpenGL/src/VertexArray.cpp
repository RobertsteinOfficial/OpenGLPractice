#include "VertexArray.h"

#include "Renderer.h"

VertexArray::VertexArray()
{
	GLCall(glGenVertexArrays(1, &m_RendererID));
}

VertexArray::~VertexArray()
{
	GLCall(glDeleteVertexArrays(1, &m_RendererID));
}

//Faccio bind del vertex buffer e setuppo il layout
void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
{
	//Bindo il vertex array
	Bind();

	//Bindo il vertex buffer
	vb.Bind();

	//Layout setup
	const auto& elements = layout.GetElements();

	for (unsigned int i = 0; i < elements.size(); i++)
	{
		const auto& element = elements[i];
		unsigned int offset = 0;

		//Abilito l'array di attributi. Posso farlo anche prima di definire gli attributi,
		// tanto OpenGl funziona a state machine, quindi non è che controlla
		GLCall(glEnableVertexAttribArray(i));

		//Definisco gli attributi del buffer. Fa anche da binding tra 
		//vertex array e buffer, perchè di base prende il vertex array attualmente bindato e il buffer
		//attualmente bindato e li lega insieme
		GLCall(glVertexAttribPointer
		(i, element.count, element.type, element.normalized, layout.GetStride(), (const void*)offset));

		offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
	}
}

void VertexArray::Bind() const
{
	GLCall(glBindVertexArray(m_RendererID));
}

void VertexArray::Unbind() const
{
	GLCall(glBindVertexArray(m_RendererID));
}
