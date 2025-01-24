#pragma once

class VertexBuffer
{
private:
	//opengl ha bisogno di un ID numerico per ogni oggetto creato. Il nome RendererID � generico, perch� 
	//se avessimo un livello pi� alto (tipo engine) potremmo star usando diverse api, non solo opengl.
	//Quindi questo � solo il nome della variabile interna, poi potremmo avere un ID lato engine
	unsigned int m_RendererID;
public:
	VertexBuffer(const void* data, unsigned int size);
	~VertexBuffer();

	void Bind() const;
	void Unbind() const;
};