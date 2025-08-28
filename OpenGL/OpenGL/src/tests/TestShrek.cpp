#include "TestShrek.h"

#include "imgui/imgui.h"
namespace test
{
	TestShrek::TestShrek()
		: vb(positions, 4 * 4 * sizeof(float)), ib(indices, 6),
		shader("res/shaders/Basic.shader"), texture("res/textures/shrekfest.png"),
		translation(200, 200, 0)
	{
		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));


		VertexBufferLayout layout;
		layout.Push<float>(2);
		layout.Push<float>(2);

		va.AddBuffer(vb, layout);


		proj = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f);
		view = glm::translate(glm::mat4(1.0f), glm::vec3(-100, 0, 0));

		shader.Bind();
		shader.SetUniform4f("u_Color", 0.3f, g, 0.8f, 1.0f);


		texture.Bind();
		//La texture  bindata allo slot 0, quindi passo 0
		shader.SetUniform1i("u_Texture", 0);


		//va.Unbind();
		//vb.Unbind();
		//ib.Unbind();
		//shader.Unbind();

	}


	TestShrek::~TestShrek()
	{
	}
	void TestShrek::OnUpdate(float deltaTime)
	{
	}
	void TestShrek::OnRender()
	{
		GLCall(glClearColor(0.1f, 0.9f, 0.0f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT));

		model = glm::translate(glm::mat4(1.0f), translation);
		mvp = proj * view * model;


		//Sta roba sarebbe da spostare, ma dovrei implementare i material
		shader.Bind();
		shader.SetUniform4f("u_Color", 0.3f, g, 0.8f, 1.0f);
		//Passo la matrice di proiezione come uniform allo shader
		shader.SetUniformMat4f("u_MVP", mvp);


		renderer.Draw(va, ib, shader);

		if (g > 1.0f)
			increment = -0.05f;
		else if (g < 0.0f)
			increment = 0.05f;

		g += increment;

		
	}
	void TestShrek::OnImGuiRender()
	{
		ImGui::SliderFloat3("Translation", &translation.x, 0.0f, 960.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
}
