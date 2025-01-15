#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

//Assertion, se argomento è falso blocco il programma. debugbreak è dipendente dal compilatore, in questo caso VS
#define ASSERT(x) if(!(x)) __debugbreak();

//Macro per gestione errore. Pulisco errori vecchi, chiamo funzione, controllo errori nuovi
//usando # converto la funzione in una stringa, le altre due macro mi restituiscono file e riga della chiamata
#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))

static void GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char* function, const char* file, int line)
{
	while (GLenum error = glGetError())
	{
		std::cout << "[OpenGL Error] (" << error << "):"
			<< " " << file << ":" << line << std::endl;
		return false;
	}

	return true;
}

struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
};

//Faccio parsing dello shader da file a stringa. Devo dividerlo in due, perchè 
//l'api si aspetta vertex e fragment separati.
static ShaderProgramSource ParseShader(const std::string& filepath)
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

static unsigned int CompileShader(unsigned int type, const std::string& source)
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

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
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

int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;


	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "OpenGl Test", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	glfwSwapInterval(1);

	if (glewInit() != GLEW_OK)
		std::cout << "Error with GLEW" << std::endl;

	std::cout << glGetString(GL_VERSION) << std::endl;

	//Determino le posizioni dei vertici
	float positions[] =
	{
		-0.5f, -0.5f,
		 0.5f, -0.5f,
		 0.5f,  0.5f,
		-0.5f,  0.5f,
	};

	//Index buffer, mi serve per sapere quali vertici mi servono per disegnare i triangoli
	unsigned int indices[] =
	{
		0, 1, 2,
		2, 3, 0
	};

	unsigned int VAO; GLCall(glGenVertexArrays(1, &VAO)); GLCall(glBindVertexArray(VAO));

	//Definisco un vertex buffer
	unsigned int buffer;
	GLCall(glGenBuffers(1, &buffer));

	//Seleziono (bind) il buffer, in questo caso sarà un array
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));

	//Fillo il buffer. Posso anche allocare solo e fillare poi, in caso
	GLCall(glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(float), positions, GL_STATIC_DRAW));

	//Abilito l'array di attributi. Posso farlo anche prima di definire gli attributi,
	// tanto OpenGl funziona a state machine, quindi non è che controlla
	GLCall(glEnableVertexAttribArray(0));

	//Definisco gli attributi del buffer. In questo caso,2D vertex positions
	GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0));

	//Faccio la stessa cosa per l'index buffer
	//ATTENZIONE: usare sempre uint per index buffer
	unsigned int ibo;
	GLCall(glGenBuffers(1, &ibo));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW));

	//Recupero lo shader
	ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");

	unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
	GLCall(glUseProgram(shader));

	//Setuppo la uniform dopo aver fatto il binding dello shader, se no non sa a chi mandarla
	//Da opengl 4.3 posso specificare la location di una uniform esplicitamente. Per ora me la recupero
	//per nome
	GLCall(int location = glGetUniformLocation(shader, "u_Color"));
	ASSERT(location != -1);
	//Setto la uniform
	GLCall(glUniform4f(location, 0.2f, 0.8f, 0.3f, 1.0f));

	float g = 0.0f;
	float increment = 0.05f;

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		GLCall(glClear(GL_COLOR_BUFFER_BIT));

		//Trangolo legacy OPENGL
		/*glBegin(GL_TRIANGLES);
		glVertex2f(-0.5f, -0.5f);
		glVertex2f( 0.0f,  0.5f);
		glVertex2f( 0.5f, -0.5f);
		glEnd();*/

		//Disegno un triangolo usando il vertex buffer di cui sopra. Non avendo un index buffer,
		// uso glDrawArrays. Se ne avessimo uno, useremmo glDrawElements
		//glDrawArrays(GL_TRIANGLES, 0, 6);

		//Animo il colore che passo allo shader
		GLCall(glUniform4f(location, 0.2f, g, 0.6f, 1.0f));

		//Per usare l'index buffer invece di glDrawArrays uso glDrawElements
		GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

		if (g > 1.0f)
			increment = -0.05f;
		else if (g < 0.0f)
			increment = 0.05f;

		g += increment;

		/* Swap front and back buffers */
		GLCall(glfwSwapBuffers(window));

		/* Poll for and process events */
		GLCall(glfwPollEvents());
	}

	//Pulisco lo shader una volta che ho finito
	glDeleteProgram(shader);

	glfwTerminate();
	return 0;
}