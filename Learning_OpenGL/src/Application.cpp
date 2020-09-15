#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "Shader.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Texture.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(960, 560, "My window", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	// GLEW este folosit pentru a avea la dispozitie OpenGL-ul "modern"
	if (glewInit() != GLEW_OK)
		std::cout << "Error" << '\n';

	std::cout << glGetString(GL_VERSION) << '\n';

	{
		float positions[] = {
		 -50.f, -50.f, 0.0f, 3.0f, 3.0f,			 // top right
		 50.f,  -50.f, 0.0f, 3.0f, 0.0f,			 // bottom right
		 50.f,   50.f, 0.0f, 0.0f, 0.0f,			 // bottom left
		 -50.f,  50.f, 0.0f, 0.0f, 3.0f			 // top left 
		};

		// indicii care indica ce vertex-uri sa desenam
		unsigned int indices[] = {  // note that we start from 0!
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
		};

		GLCall(glEnable(GL_BLEND));	
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		glm::mat4 proj = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f);
		glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));

		Shader shader("res/Basic.shader");
		shader.Bind();

		Texture texture("res/textures/texture.jpg");
		texture.Bind();

		shader.SetUniform1i("u_Texutre", 0);

		VertexArray va;
		VertexBuffer vb(positions, 5 * 4 * sizeof(float));
		VertexBufferLayout layout;

		layout.Push<float>(3);
		layout.Push<float>(2);

		va.AddBuffer(vb, layout);

		IndexBuffer ib(indices, 6);

		va.Unbind();
		vb.Unbind();
		ib.Unbind();
		shader.Unbind();

		Renderer renderer;

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init((char*)glGetString(GL_NUM_SHADING_LANGUAGE_VERSIONS));

		glm::vec3 translationA(200, 200, 0);
		glm::vec3 translationB(500, 200, 0);

		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			processInput(window);

			/* Render here */
//			float red = (sin(glfwGetTime()) / 2.0f) + 0.5f;
//			float green = (cos(glfwGetTime()) / 2.0f) + 0.5f;
//			float blue = (red + green) / 2.0f;

			// Desenarea triunghiurilor
			renderer.Clear();
//			shader.SetUniform4f("col", red, green, blue, 1.0f);

			{
				glm::mat4 model = glm::translate(glm::mat4(1.0f), translationA);
				glm::mat4 mvp = proj * view * model;
				shader.Bind();
				shader.SetUniformMat4f("u_MVP", mvp);

				renderer.Draw(va, ib, shader);
			}

			{
				glm::mat4 model = glm::translate(glm::mat4(1.0f), translationB);
				glm::mat4 mvp = proj * view * model;
				shader.SetUniformMat4f("u_MVP", mvp);

				renderer.Draw(va, ib, shader);
			}

			va.Unbind();

			{
				ImGui::SliderFloat3("Translation A", &translationA.x, 0.0f, 960.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
				ImGui::SliderFloat3("Translation B", &translationB.x, 0.0f, 960.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			}


			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			/* Swap front and back buffers */
			GLCall(glfwSwapBuffers(window));

			/* Poll for and process events */
			GLCall(glfwPollEvents());
		}
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();

	ImGui::DestroyContext();

	glfwTerminate();
	return 0;
}