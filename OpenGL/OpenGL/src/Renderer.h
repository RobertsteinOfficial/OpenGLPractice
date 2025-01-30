#pragma once

#include <GL/glew.h>

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"

//Assertion, se argomento è falso blocco il programma. debugbreak è dipendente dal compilatore
#define ASSERT(x) if(!(x)) __debugbreak();

//Macro per gestione errore. Pulisco errori vecchi, chiamo funzione, controllo errori nuovi
//usando # converto la funzione in una stringa, le altre due macro mi restituiscono file e riga della chiamata
#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))

void GLClearError();

bool GLLogCall(const char* function, const char* file, int line);

//C'è chi rende questa classe singleton, dipende da come voglio usarlo immagino
class Renderer
{
public:
	void Clear() const;
	void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
};
