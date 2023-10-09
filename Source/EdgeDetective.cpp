/*
	Sobel算子实现边缘检测；
	通过两个3×3滤波器实现，分别为梯度的垂直和水平分量；
		  -1	0	 1			     -1	  -2   -1
	Sx =  -2	0    2         Sy =	  0    0    0
		  -1	0	 1				  1    2    1
	计算出的每个采样点与一初始亮度lume = vec3(0.2126, 0.7152, 0.0722) 进行dot操作，累加后进行g =	(pow(Sx, 2), pow(Sy, 2))计算；
	结果与设置的阈值0.05作比较，若大于则认为是边缘部分；

*/

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "../Headers/Model.h"
#include "../Headers/Shader.h"
#include "../Headers/Texture.h"
#include "../Headers/Camera.h"
#include "../Headers/VertexArrayObject.h"
#include "../Headers/VertexBufferObject.h"

Camera camera{ glm::vec3(0, 5.0f, 10.0f), 0.0f, 0.0f, glm::vec3(0, 1.0f, 0) };
#pragma region Interaction
float x_last, y_last;
bool first_mouse = true;
void MouseCallback(GLFWwindow* window, double x_pos, double y_pos)
{
	if (first_mouse == true)
	{
		x_last = x_pos;
		y_last = y_pos;
		first_mouse = false;
	}

	float x_offset = x_pos - x_last;
	float y_offset = y_pos - y_last;

	x_last = x_pos;
	y_last = y_pos;

	camera.UpdateCameraView(x_offset, y_offset);
	camera.GetViewMatrix();
}

void ProcessInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera.speedZ = -0.03f;
	}
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera.speedZ = 0.03f;
	}
	else
	{
		camera.speedZ = 0.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera.speedX = -0.03f;
	}
	else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera.speedX = 0.03f;
	}
	else
	{
		camera.speedX = 0.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		camera.speedY = 0.03f;
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		camera.speedY = -0.03f;
	}
	else
	{
		camera.speedY = 0.0f;
	}
}
#pragma endregion Interaction

void RenderQuad();
unsigned int LoadTexture(const char*);

int main()
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	glfwWindowHint(GLFW_SAMPLES, 16);
	window = glfwCreateWindow(800, 600, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	if (!gladLoadGL())
	{
		std::cout << "glad loading error \n";
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, false);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, MouseCallback);

#pragma region Configue
	Model object{ "./Models/ball/ball.obj" };
	Model object2{ "./Models/spot_triangulated_good.obj" };
	Shader shader{ "./Shaders/EdgeDetective/edge_detective.vs", "./Shaders/EdgeDetective/edge_detective.fs" };
	Shader quad_shader{ "./Shaders/EdgeDetective/quad.vs", "./Shaders/EdgeDetective/quad.fs" };
	glm::vec3 lightPos = glm::vec3(10.0f, 20.0f, 0.0f);
	glm::vec3 lightColor = glm::vec3(1.0f);

#pragma region FBO
	unsigned int fbo;
	unsigned int fbo_texture, rbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glGenTextures(1, &fbo_texture);
	glBindTexture(GL_TEXTURE_2D, fbo_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_texture, 0);

	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, 800, 600);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
#pragma endregion regionName

#pragma endregion Configue

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.2f, 0.2f, 0.2f, 0.2f);
		ProcessInput(window);
		camera.UpdateCameraPosition();

		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)800 / (float)600, 0.1f, 1000.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(0.3f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

		shader.Bind();
		shader.SetUniform1i("is_quad", 0);
		shader.SetUniform3v("lightPos", lightPos);
		shader.SetUniform3v("lightColor", lightColor);
		shader.SetUniform3v("cameraPos", camera.position);

#pragma region Object1
		shader.SetUniformMat4("model", model);
		shader.SetUniformMat4("view", view);
		shader.SetUniformMat4("projection", projection);

		object.Draw(shader);
#pragma endregion Object1

#pragma region Object2
		model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(2.0f));
		model = glm::translate(model, glm::vec3(2.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		shader.SetUniformMat4("model", model);

		object2.Draw(shader);
#pragma endregion Object2

#pragma region Quad
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, fbo_texture);

		quad_shader.Bind();
		quad_shader.SetUniform1i("screen_texture", 0);
		RenderQuad();
#pragma endregion Quad

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void RenderQuad()
{
	float quad_vertice[] = {
		// positions						  // texture Coords
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		-1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		 1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		 1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
	};

	VertexBufferObject vbo{ quad_vertice, sizeof(quad_vertice) };
	vbo.Bind();

	VertexArrayObject vao{};
	vao.AddBuffer(0, 3, GL_FLOAT, false, 8 * sizeof(float), 0);
	vao.AddBuffer(1, 3, GL_FLOAT, false, 8 * sizeof(float), 3 * sizeof(float));
	vao.AddBuffer(2, 2, GL_FLOAT, false, 8 * sizeof(float), 6 * sizeof(float));
	vao.Bind();

	glDrawArrays(GL_TRIANGLES, 0, 6);
}

unsigned int LoadTexture(const char* file)
{
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	int width, height, number_of_channel;
	unsigned char* data = stbi_load(file, &width, &height, &number_of_channel, 0);

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	}
	else
	{
		std::cout << file << " loading error \n";
	}

	return texture;
}