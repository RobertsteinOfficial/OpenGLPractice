#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(type);
	//c_str punta all'inizio della stringa
	const char* src = source.c_str();

	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);
	
	//Error handling
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);

	//Se la compilazione è andata male, mi recupero il messaggio di errore e lo stampo
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

		//Voglio allocare l'array di char sullo stack, solo che non posso semplicemente fare
		//char message[length]; perchè length è una variabile, non una costante. 
		//Soluzione uno sarebbe allocare sullo heap, ma non mi piace
		//Soluzione due
		char* message = (char*)alloca(length * sizeof(char));

		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << "shader" << std::endl;
		std::cout << message << std::endl;

		glDeleteShader(id);
		return 0;
	}

	return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	//Faccio attachment dei due shader al programma, linko e valido il programma
	glAttachShader(program, vs);
	glAttachShader(program, fs);

	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

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


	if (glewInit() != GLEW_OK)
		std::cout << "Error with GLEW" << std::endl;

	std::cout << glGetString(GL_VERSION) << std::endl;

	float positions[6] =
	{
		-0.5f, -0.5f, 
		 0.0f,  0.5f, 
		 0.5f, -0.5f
	};

	unsigned int VAO; glGenVertexArrays(1, &VAO); glBindVertexArray(VAO);

	//Definisco un vertex buffer
	unsigned int buffer;
	glGenBuffers(1, &buffer);

	//Seleziono (bind) il buffer, in questo caso sarà un array
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	//Fillo il buffer. Posso anche allocare solo e fillare poi, in caso
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);

	//Abilito l'array di attributi. Posso farlo anche prima di definire gli attributi,
	// tanto OpenGl funziona a state machine, quindi non è che controlla
	glEnableVertexAttribArray(0);

	//Definisco gli attributi del buffer. In questo caso,2D vertex positions
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);


	//Creo gli shader. Un po' uno sbatti farlo come stringa, poi lo passo su un file
	std::string vertexShader = R"glsl(
		#version 330 core
		
		layout(location = 0) in vec4 position;
		
		void main()
		{
		gl_Position = position;
		};
		)glsl";



	std::string fragmentShader = R"glsl(
		#version 330 core
		
		layout(location = 0) out vec4 color;
		
		void main()
		{
		color = vec4(1.0, 0.0, 0.0, 1.0);
		};
		)glsl";

	unsigned int shader = CreateShader(vertexShader, fragmentShader);
	glUseProgram(shader);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		//Trangolo legacy OPENGL
		/*glBegin(GL_TRIANGLES);
		glVertex2f(-0.5f, -0.5f);
		glVertex2f( 0.0f,  0.5f);
		glVertex2f( 0.5f, -0.5f);
		glEnd();*/

		//Disegno un triangolo usando il vertex buffer di cui sopra. Non avendo un index buffer,
		// uso glDrawArrays. Se ne avessimo uno, useremmo glDrawElements
		glDrawArrays(GL_TRIANGLES, 0, 3);

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}