#include "TestTexture2D.h"

#include "Renderer.h"
#include "imgui//imgui.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <memory>

namespace test {

	test::TestTexture2D::TestTexture2D()
		: m_Proj(glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f)), 
		m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0))),
		m_TranslationA(200, 200, 0), m_TranslationB(400, 200, 0)
	{
		// Positions for the Vertex Buffer (on each line, the first two are texture coorinates 
		// and the last two are vertices for the 2D position of the triangles.
		float positions[] = {
			-50.0f, -50.0f, 0.0f, 0.0f,
			50.0f, -50.0f, 1.0f, 0.0f,
			50.0f, 50.0f, 1.0f, 1.0f,
			-50.0f, 50.0f, 0.0f, 1.0f
		};

		// These indices will form our square
		unsigned int indices[] = {
			0, 1, 2,
			2, 3, 0
		};
		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		// Create VertexBuffer object, VertexBufferLayout object and Vertex Array object
		m_VAO = std::make_unique<VertexArray>();
		m_IndexBuffer = std::make_unique<IndexBuffer>(indices, 6);

		m_VertexBuffer = std::make_unique<VertexBuffer>(positions, 4 * 4 * sizeof(float));
		VertexBufferLayout layout;
		layout.Push<float>(2);
		layout.Push<float>(2);
		m_VAO->AddBuffer(*m_VertexBuffer, layout);
		IndexBuffer ib(indices, 6);		// Create the IndexBuffer object

		m_Shader = std::make_unique<Shader>("res/Basic.shader");				// Create the shader - program that runs on the GPU
		m_Shader->Bind();														// Bind the shader
		m_Texture = std::make_unique<Texture>("res/textures/texture.png");		// Create the texture object
		m_Shader->SetUniform1i("u_Texture", 0);
	}

	test::TestTexture2D::~TestTexture2D()
	{
	}

	void test::TestTexture2D::OnUpdate(float deltaTime)
	{
	}

	void test::TestTexture2D::OnRender()
	{
		GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT));

		Renderer renderer;

		m_Texture->Bind();

		{	// Render the first object
			glm::mat4 model = glm::translate(glm::mat4(1.0f), m_TranslationA);
			glm::mat4 mvp = m_Proj * m_View * model;
			m_Shader->Bind();
			m_Shader->SetUniformMat4f("u_MVP", mvp);

			renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
		}

		{	// Render the second object
			glm::mat4 model = glm::translate(glm::mat4(1.0f), m_TranslationB);
			glm::mat4 mvp = m_Proj * m_View * model;
			m_Shader->Bind();
			m_Shader->SetUniformMat4f("u_MVP", mvp);

			renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
		}
	}

	void test::TestTexture2D::OnImGuiRender()
	{
		// We'll create two sliders to set the position of the 2 rendered objects.
		ImGui::SliderFloat3("Translation A", &m_TranslationA.x, 0.0f, 960.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::SliderFloat3("Translation B", &m_TranslationB.x, 0.0f, 960.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}

}