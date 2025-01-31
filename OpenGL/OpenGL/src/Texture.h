#pragma once

#include "Renderer.h"

class Texture
{
private:
	unsigned int m_RendererID;
	std::string m_FilePath;
	unsigned char* m_LocalBuffer;
	int m_Width, m_Height, m_BPP; //bits per pixel

public:

	Texture(const std::string& path);
	~Texture();

	//Posso bindare più texture contemporaneamente. Ho un numero di slot variabile in base alla piattaforma.
	void Bind(unsigned int slot = 0) const;
	void Unbind() const;

	inline int GetWidth() const { return m_Width; }
	inline int GetHeight() const { return m_Height; }
};