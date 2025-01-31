#include "Shader.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"


Shader::Shader(const std::string& filepath)
	: m_FilePath(filepath), m_RendererID(0)
{
	ShaderProgramSource source = ParseShader(filepath);

	m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
}

Shader::~Shader()
{
	GLCall(glDeleteProgram(m_RendererID));
}

//Faccio parsing dello shader da file a stringa. Devo dividerlo in due, perchè 
//l'api si aspetta vertex e fragment separati.
ShaderProgramSource Shader::ParseShader(const std::string& filepath)
{
	//l'apertura del file la faccio con c++ moderno invece che c, anche se è più lento è 
	//più sicuro e leggibile
	std::ifstream stream(filepath);

	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::string line;
	std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;

	//Mi scorro le righe del file. Se trovo il marker shader imposto lo shader come vertex o come 
	//fragment a seconda di quello che indica il marker, in caso contrario pusho la riga nello shader
	//corrispondente alla modalità che è settata al momento. 
	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
				type = ShaderType::VERTEX;
			else if (line.find("fragment") != std::string::npos)
				type = ShaderType::FRAGMENT;
		}
		else
		{
			ss[(int)type] << line << '\n';
		}
	}

	return { ss[0].str(), ss[1].str() };
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
{
	GLCall(unsigned int id = glCreateShader(type));
	//c_str punta all'inizio della stringa
	const char* src = source.c_str();

	GLCall(glShaderSource(id, 1, &src, nullptr));
	GLCall(glCompileShader(id));

	int result;
	GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));

	//Se la compilazione è andata male, mi recupero il messaggio di errore e lo stampo
	if (result == GL_FALSE)
	{
		int length;
		GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));

		//Voglio allocare l'array di char sullo stack, solo che non posso semplicemente fare
		//char message[length]; perchè length è una variabile, non una costante. 
		//Soluzione uno sarebbe allocare sullo heap, ma non mi piace
		//Soluzione due
		char* message = (char*)alloca(length * sizeof(char));

		GLCall(glGetShaderInfoLog(id, length, &length, message));
		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << "shader" << std::endl;
		std::cout << message << std::endl;

		GLCall(glDeleteShader(id));
		return 0;
	}

	return id;
}

unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	GLCall(unsigned int program = glCreateProgram());
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	//Faccio attachment dei due shader al programma, linko e valido il programma
	GLCall(glAttachShader(program, vs));
	GLCall(glAttachShader(program, fs));

	GLCall(glLinkProgram(program));
	GLCall(glValidateProgram(program));

	GLCall(glDeleteShader(vs));
	GLCall(glDeleteShader(fs));

	return program;
}

//Binding. Anche se Opengl usa UseProgram come nomenclatura, rimango
//consistente e tengo Bind
void Shader::Bind() const
{
	GLCall(glUseProgram(m_RendererID));
}

void Shader::Unbind() const
{
	GLCall(glUseProgram(0));
}

void Shader::SetUniform1i(const::std::string& name, int value)
{
	GLCall(glUniform1i(GetUniformLocation(name), value));
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
	GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
}

int Shader::GetUniformLocation(const std::string& name)
{
	if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
		return m_UniformLocationCache[name];

	//Da opengl 4.3 posso specificare la location di una uniform esplicitamente. Per ora me la recupero
	//per nome
	GLCall(int location = glGetUniformLocation(m_RendererID, name.c_str()));
	
	//Potrei fare ASSERT, ma potrebbe essermi utile avere location = -1
	if (location == -1)
		std::cout << "Warning: uniform '" << name << "' does not exist" << std::endl;

	m_UniformLocationCache[name] = location;
	return location;
}
