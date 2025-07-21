#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"

//Math library
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

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

	//Faccio questo scope perch� i buffer che creo qua dentro sono allocati sullo stack, e vengono
	//in teoria puliti quando esco da main. Il problema � che quando ne chiamo il distruttore siamo fuori dal
	//conteso opengl, che � stato chiuso prima. Quindi mi parte il check error che si trova nella macro 
	//GLCall. Per� anche questo da errore, perch� siamo sempre fuori da un contesto valido, e quando
	//glGetError da un errore, chiama a sua volta glGetError :)
	//Dato che di solito non � che vado a creare i buffer in main, per la risolvo cos�

	{


		//Determino le posizioni dei vertici e le texcoord
		float positions[] =
		{
			-0.5f, -0.5f, 0.0f, 0.0f,
			 0.5f, -0.5f, 1.0f, 0.0f,
			 0.5f,  0.5f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 1.0f
		};

		//Index buffer, mi serve per sapere quali vertici mi servono per disegnare i triangoli
		unsigned int indices[] =
		{
			0, 1, 2,
			2, 3, 0
		};

		//Abilito blending
		GLCall(glEnable(GL_BLEND));
		//GL_SRC_ALPHA e GL_ONE_MINUS_SRC_ALPHA sono due moltiplicatori, il primo per la source e il
		//secondo per la destination. I loro valori di default sono 1 e 0
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
		//Dopo in teoria sarebbe da settare la modde tramite glBlendEquation. Dato che di default � 
		//additiva (GL_FUNC_ADD) non ne ho bisogno al momento. 

		//Creo il vertex array. Contiene posizione dei vertici e texcoord
		VertexArray va;
		VertexBuffer vb(positions, 4 * 4 * sizeof(float));

		
		VertexBufferLayout layout;
		layout.Push<float>(2);
		layout.Push<float>(2);
		va.AddBuffer(vb, layout);

		//Faccio la stessa cosa per l'index buffer
		IndexBuffer ib(indices, 6);


		//Imposto la matrice di proiezione. Prima me la calcolo
		//Di default sarebbe quadrata, ma al momento sto lavorando su una finestra
		//rettangolare. Faccio matrice ortografica tanto sto lavorando in 2D
		//Per gli argomenti basta specificare delle coordinate che aderiscano all'aspect ratio della finestra
		glm::mat4 proj = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, -1.0f, 1.0f);




		//Recupero lo shader
		Shader shader("res/shaders/Basic.shader");
		shader.Bind();
		//Setuppo la uniform dopo aver fatto il binding dello shader, se no non sa a chi mandarla
		shader.SetUniform4f("u_Color", 0.2f, 0.8f, 0.3f, 1.0f);

		//Passo la matrice di proiezione come uniform allo shader
		shader.SetUniformMat4f("u_MVP", proj);


		//Recupero la texture, bindo, passo allo shader
		Texture texture("res/textures/shrekfest.png");
		texture.Bind();
		//La texture � bindata allo slot 0, quindi passo 0
		shader.SetUniform1i("u_Texture", 0);

		//Sgancio i miei buffer. Al momento � solo un esempio per usare i VAO, dato che se devo disegnare pi� roba 
		//non posso tenere gli stessi buffer, ma devo scambiarli al volo
		//Ha senso sbindare prima il VAO, se no poi quest salverebbe lo stato di sbinding di buffer e ibo, e quindi 
		//dovrei ribindarli di nuovo. Mentre se sgancio prima il VAO non salva le modifiche ai buffer, e quindi 
		//mi basta ribindare il VAO e funziona tutto
		va.Unbind();
		vb.Unbind();
		ib.Unbind();
		shader.Unbind();
		//Protip if you want to get as modern as OpenGL gets, 4.5 introduced glVertexArrayVertexBuffer 
		// and glVertexArrayElementBuffer, which explicitly bind a vertex or element buffer to a specific
		// vertex array, rather than just leaving it out in the open like glBindBuffer(GL_ARRAY_BUFFER) and 
		// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER) do.

		Renderer renderer;


		float g = 0.0f;
		float increment = 0.05f;

		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{
			/* Render here */

			renderer.Clear();

			//Sta roba sarebbe da spostare, ma dovrei implementare i material
			shader.Bind();
			shader.SetUniform4f("u_Color", 0.3f, g, 0.8f, 1.0f);
			
			renderer.Draw(va, ib, shader);

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
		// EDIT: non mi serve pi� perch� ci pensa il distruttore
		// di Shader quando esce dallo scope
		//glDeleteProgram(shader);
	}
	glfwTerminate();
	return 0;
}