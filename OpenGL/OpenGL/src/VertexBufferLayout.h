#pragma once

#include <vector>
#include "Renderer.h"

struct VertexBufferElement
{
	unsigned int type;	//opengl type
	unsigned int count;
	unsigned char normalized;

	VertexBufferElement(unsigned int _type, unsigned int _count, unsigned char _normalized) :
		type(_type), count(_count), normalized(_normalized) { }

	static unsigned int GetSizeOfType(unsigned int type)
	{
		//Al momento supporto solo tre tipi, poi all'occorrenza li aggiungo
		switch (type)
		{
		case GL_FLOAT: return 4;
		case GL_UNSIGNED_INT: return 4;
		case GL_UNSIGNED_BYTE: return 1;
		}

		ASSERT(false);
		return 0;
	}
};

class VertexBufferLayout
{
private:
	std::vector<VertexBufferElement> m_Elements;
	unsigned int m_Stride;

public:
	VertexBufferLayout() : m_Stride(0) {}

	template<typename T>
	void Push(unsigned int count)
	{
		//In VS2022 will trigger the assert when it's parsed, rather than when it's instantiated
		//static_assert(false);
		static_assert(sizeof(T) == 0, "No suitable template specialization for this type.");
	}

	template<>
	void Push<float>(unsigned int count)
	{
		m_Elements.push_back(VertexBufferElement(GL_FLOAT, count, GL_FALSE));
		m_Stride += VertexBufferElement::GetSizeOfType(GL_FLOAT) * count;
	}

	template<>
	void Push<unsigned int>(unsigned int count)
	{
		m_Elements.push_back(VertexBufferElement(GL_FLOAT, count, GL_FALSE));
		m_Stride += VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT) * count;
	}

	template<>
	void Push<unsigned char>(unsigned int count)
	{
		m_Elements.push_back(VertexBufferElement(GL_FLOAT, count, GL_FALSE));
		m_Stride += VertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE) * count;
	}

	//dovrei restituire const& ?
	inline const std::vector<VertexBufferElement> GetElements() const { return m_Elements; }
	inline unsigned int GetStride() const { return m_Stride; }
};