#pragma once

class IndexBuffer
{
private:
	//opengl ha bisogno di un ID numerico per ogni oggetto creato. Il nome RendererID è generico, perchè 
	//se avessimo un livello più alto (tipo engine) potremmo star usando diverse api, non solo opengl.
	//Quindi questo è solo il nome della variabile interna, poi potremmo avere un ID lato engine
	unsigned int m_RendererID;
	unsigned int m_Count;
public:
	//Per ora supporto solo int32, avrebbe senso avere anche int16, ma poi si vede
	IndexBuffer(const unsigned int* data, unsigned int count);
	~IndexBuffer();

	void Bind() const;
	void Unbind() const;

	inline unsigned int GetCount() const { return m_Count; }
};