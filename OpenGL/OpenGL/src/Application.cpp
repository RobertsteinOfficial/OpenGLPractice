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

//imgui
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

//Test
#include "tests/TestClearColour.h"

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
	window = glfwCreateWindow(960, 540, "OpenGl Test", NULL, NULL);
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

		//Abilito blending
		GLCall(glEnable(GL_BLEND));
		//GL_SRC_ALPHA e GL_ONE_MINUS_SRC_ALPHA sono due moltiplicatori, il primo per la source e il
		//secondo per la destination. I loro valori di default sono 1 e 0
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
		//Dopo in teoria sarebbe da settare la modde tramite glBlendEquation. Dato che di default è 
		//additiva (GL_FUNC_ADD) non ne ho bisogno al momento. 


		Renderer renderer;

		//Inizializzo imgui
		ImGui::CreateContext();
		const char* glsl_version = "#version 130";
		ImGui_ImplOpenGL3_Init(glsl_version);
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui::StyleColorsDark();

		test::TestClearColour test;

		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{
			/* Render here */
			renderer.Clear();

			//Runno la scena test
			test.OnUpdate(0.0f);
			test.OnRender();

			//Creo un nuovo frame imgui. Prima di eseguire codice imgui devo aver chiamato questa funzione
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			test.OnImGuiRender();

			//Disegno la roba imgui
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			/* Swap front and back buffers */
			GLCall(glfwSwapBuffers(window));

			/* Poll for and process events */
			GLCall(glfwPollEvents());
		}

		//Pulisco lo shader una volta che ho finito
		// EDIT: non mi serve più perchè ci pensa il distruttore
		// di Shader quando esce dallo scope
		//glDeleteProgram(shader);
	}

	//Chiudo imgui
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
	return 0;
}