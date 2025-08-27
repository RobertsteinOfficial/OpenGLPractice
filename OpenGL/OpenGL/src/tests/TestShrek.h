#pragma once

#include "Test.h"

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "Texture.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace test
{
	class TestShrek : public Test
	{
	public:
		TestShrek();
		~TestShrek();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;

	private:

		//Determino le posizioni dei vertici e le texcoord
		float positions[16]
		{
			100.0f, 100.0f, 0.0f, 0.0f,
			200.0f, 100.0f, 1.0f, 0.0f,
			200.0f, 200.0f, 1.0f, 1.0f,
			100.0f, 200.0f, 0.0f, 1.0f
		};

		//Index buffer, mi serve per sapere quali vertici mi servono per disegnare i triangoli
		unsigned int indices[6]
		{
			0, 1, 2,
			2, 3, 0
		};


		VertexArray va;
		VertexBuffer vb;
		IndexBuffer ib;

		Shader shader;
		Texture texture;
		float g = 0.0f;
		float increment = 0.05f;


		glm::mat4 proj;
		glm::mat4 view;
		glm::mat4 model;
		glm::mat4 mvp;

		glm::vec3 translation;

		Renderer renderer;

	};
}