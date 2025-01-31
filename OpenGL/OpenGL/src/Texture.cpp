#include "Texture.h"

#include "vendor/stb_image/stb_image.h"	//magari da mettere negli include path

Texture::Texture(const std::string& path)
	: m_RendererID(0), m_FilePath(path), m_LocalBuffer(nullptr), m_Width(0), m_Height(0), m_BPP(0)
{
	//dato che al momento sto usando png, devo flippare la texture, perchè opengl parte da in basso a sinistra, png
	//da in alto a sinistra
	stbi_set_flip_vertically_on_load(1);
	//Prendo la texture e scrivo in m_LocalBuffer
	m_LocalBuffer = stbi_load(path.c_str(), &m_Width, &m_Height, &m_BPP, 4);

	//Genero la texture, la bindo al buffer apposito
	GLCall(glGenTextures(1, &m_RendererID));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));

	//Setuppo settings per la texture. Devo specificarli obbligatoriamente
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	
	//Mando la texture.
	//Potrei anche solo allocare passando un nullptr invece di m_LocalBuffer in caso non avessi i dati pronti
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer));

	//Faccio poi l'unbind
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));

	//In sistemi più complicati magari voglio tenere una copia dei dati dei pixel sulla cpu per farci cose,
	//ma al momento non ci interessa. Se poi cambio idea, m_LocalBuffer è già un membro
	if (m_LocalBuffer)
		stbi_image_free(m_LocalBuffer);
}

Texture::~Texture()
{
	GLCall(glDeleteTextures(1, &m_RendererID));
}

void Texture::Bind(unsigned int slot) const
{
	//Specifico lo slot. Sempre perchè il sistema è una state machine
	GLCall(glActiveTexture(GL_TEXTURE0 + slot));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));
}

void Texture::Unbind() const
{
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}
