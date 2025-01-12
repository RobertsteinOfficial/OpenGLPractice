#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

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

	//Definisco un vertex buffer
	unsigned int buffer;
	glGenBuffers(1, &buffer);

	//Seleziono (bind) il buffer, in questo caso sarà un array
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	//Fillo il buffer
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);

	//Abilito l'array di attributi. Posso farlo anche prima di definire gli attributi,
	// tanto OpenGl funziona a state machine, quindi non è che controlla
	glEnableVertexAttribArray(0);

	//Definisco gli attributi del buffer. In questo caso,2D vertex positions
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

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