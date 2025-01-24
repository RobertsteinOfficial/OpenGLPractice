#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"

#include "VertexBuffer.h"
#include "IndexBuffer.h"

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

	//Chiedo a GLFW di inizializzare la finestra nella col profilo core, che non gestisce automaticamente i VAO
	//Prima setto la versione di opengl a 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// The two options can be stated simply: (A) `COMPAT_PROFILE` means recycling one default/global VAO 
	// for everything; this is the default profile for backward-compatibility. (B) `CORE_PROFILE` means 
	// defining as many VAOs as you like. Changing VAOs is fewer-calls/optimal/easier/faster than otherwise 
	// changing the VAAs + VBOs + IBs many times per frame.
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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

	//Faccio questo scope perchè i buffer che creo qua dentro sono allocati sullo stack, e vengono
	//in teoria puliti quando esco da main. Il problema è che quando ne chiamo il distruttore siamo fuori dal
	//conteso opengl, che è stato chiuso prima. Quindi mi parte il check error che si trova nella macro 
	//GLCall. Però anche questo da errore, perchè siamo sempre fuori da un contesto valido, e quando
	//glGetError da un errore, chiama a sua volta glGetError :)
	//Dato che di solito non è che vado a creare i buffer in main, per la risolvo così

	{


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

		//Creo il VAO esplicitamente
		unsigned int vao; GLCall(glGenVertexArrays(1, &vao)); GLCall(glBindVertexArray(vao));

		VertexBuffer vb(positions, 4 * 2 * sizeof(float));


		//Abilito l'array di attributi. Posso farlo anche prima di definire gli attributi,
		// tanto OpenGl funziona a state machine, quindi non è che controlla
		GLCall(glEnableVertexAttribArray(0));

		//Definisco gli attributi del buffer. In questo caso,2D vertex positions. Fa anche da binding tra 
		//vertex array e buffer, perchè di base prende il vertex array attualmente bindato e il buffer
		//attualmente bindato e li lega insieme
		GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0));

		//Faccio la stessa cosa per l'index buffer
		IndexBuffer ib(indices, 6);

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

		//Sgancio i miei buffer. Al momento è solo un esempio per usare i VAO, dato che se devo disegnare più roba 
		//non posso tenere gli stessi buffer, ma devo scambiarli al volo
		//Ha senso sbindare prima il VAO, se no poi quest salverebbe lo stato di sbinding di buffer e ibo, e quindi 
		//dovrei ribindarli di nuovo. Mentre se sgancio prima il VAO non salva le modifiche ai buffer, e quindi 
		//mi basta ribindare il VAO e funziona tutto
		GLCall(glBindVertexArray(0));
		GLCall(glUseProgram(0));
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

		//Protip if you want to get as modern as OpenGL gets, 4.5 introduced glVertexArrayVertexBuffer 
		// and glVertexArrayElementBuffer, which explicitly bind a vertex or element buffer to a specific
		// vertex array, rather than just leaving it out in the open like glBindBuffer(GL_ARRAY_BUFFER) and 
		// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER) do.

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

			//Qua faccio il binding di nuovo dei buffer e seleziono lo shader da usare
			GLCall(glUseProgram(shader));


			GLCall(glBindVertexArray(vao));
			//Devo anche runnare l'attribpointer di nuovo, dato che ho rebindato tutto. Chiamo anche l'enable, 
			//anche questo potrei averlo disabilitato prima da qualche parte.
			//N.B. queste chiamate non sono più necessarie perchè ho creato il VAO sopra
			//GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));
			/*GLCall(glEnableVertexAttribArray(0));
			GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0));*/
			//GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));

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
	}
	glfwTerminate();
	return 0;
}