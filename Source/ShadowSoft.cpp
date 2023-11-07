/*
	泊松盘采样进行降噪；
	PCSS软阴影；
	PCF软阴影；
*/

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "../Headers/Geometries.h"
#include "../Headers/Model.h"
#include "../Headers/Shader.h"
#include "../Headers/Texture.h"
#include "../Headers/Camera.h"
#include "../Headers/VertexArrayObject.h"
#include "../Headers/VertexBufferObject.h"

Camera camera{ glm::vec3(0, 0.0f, 5.0f), 0.0f, 0.0f, glm::vec3(0, 1.0f, 0) };
float speed = 0.0001f;
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
		camera.speedZ = -speed;
	}
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera.speedZ = speed;
	}
	else
	{
		camera.speedZ = 0.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera.speedX = -speed;
	}
	else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera.speedX = speed;
	}
	else
	{
		camera.speedX = 0.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		camera.speedY = speed;
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		camera.speedY = -speed;
	}
	else
	{
		camera.speedY = 0.0f;
	}
}
#pragma endregion Interaction

int main()
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	glfwWindowHint(GLFW_SAMPLES, 16);
	window = glfwCreateWindow(800, 600, "Hello World", nullptr, nullptr);
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
	glfwWindowHint(GLFW_SAMPLES, 16);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, MouseCallback);

#pragma region Configue
	std::string shader_dir = "./Shaders/ShadowSoft/";
	Shader depth_shader{ shader_dir + "shadow_depth.vs", shader_dir + "shadow_depth.fs" };
	Shader shader{ shader_dir + "shadow.vs", shader_dir + "shadow.fs" };
	Shader debug_shader{ shader_dir + "debug_quad.vs", shader_dir + "debug_quad.fs" };
	Shader light_shader{ shader_dir + "light.vs", shader_dir + "light.fs" };

	Model obj{ "./Models/building.obj" };
	Model obj2{ "./Models/spot_triangulated_good.obj" };

	depth_shader.Bind();
	shader.Bind();
	debug_shader.Bind();

	glm::vec3 lightPos = glm::vec3(-13.0f, 10.0f, 10.0f);
	glm::vec3 lightColor = glm::vec3(1.0f);

#pragma region FBO
	unsigned int fbo;
	unsigned int depth_texture;

	glGenFramebuffers(1, &fbo);

	glGenTextures(1, &depth_texture);
	glBindTexture(GL_TEXTURE_2D, depth_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float border[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_texture, 0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "fbo error! \n";
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
#pragma endregion FBO

#pragma endregion Configue

	float fps = 0;
	float frames;
	double last_time = glfwGetTime();
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		glfwSwapInterval(0);
		glEnable(GL_DEPTH_TEST);

		double current_time = glfwGetTime();
		fps++;
		if (current_time - last_time >= 1.0)
		{
			glfwSetWindowTitle(window, std::to_string(int(fps)).c_str());
			frames = fps;
			fps = 0.0;
			last_time += 1.0;
		}

		/* Render here */

		glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.01f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 model = glm::mat4(1.0f);

#pragma region FBO
		glViewport(0, 0, 1024, 1024);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glClear(GL_DEPTH_BUFFER_BIT);

		glm::mat4 light_view = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 light_projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.01f, 50.0f);

		depth_shader.Bind();
		depth_shader.SetUniformMat4("view", light_view);
		depth_shader.SetUniformMat4("projection", light_projection);

		model = glm::translate(model, glm::vec3(0.0f, -0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f));
		//model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

		depth_shader.SetUniformMat4("model", model);

		obj.Draw(depth_shader);
		model = glm::translate(model, glm::vec3(10.0f, 0.0f, -5.0f));
		depth_shader.SetUniformMat4("model", model);
		obj2.Draw(depth_shader);
		//Geometries::RenderCube();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 20.0f, 0.0f));
		model = glm::scale(model, glm::vec3(20.0f));
		depth_shader.SetUniformMat4("model", model);
		Geometries::RenderCube();

#pragma endregion FBO

#pragma region Map
		glViewport(0, 0, 800, 600);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depth_texture);

		shader.Bind();
		shader.SetUniform1i("depth_texture", 0);
		shader.SetUniformMat4("view", view);
		shader.SetUniformMat4("projection", projection);
		shader.SetUniform3v("lightPos", lightPos);
		shader.SetUniform3v("lightColor", lightColor);
		shader.SetUniform3v("cameraPos", camera.position);
		shader.SetUniform1i("normalReverse", 0);
		shader.SetUniform3v("color", glm::vec3(0.5f, 0.4f, 0.3f));
		shader.SetUniformMat4("lightProjection", light_projection);
		shader.SetUniformMat4("lightViwe", light_view);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f));
		//model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

		shader.SetUniformMat4("model", model);

		obj.Draw(shader);
		model = glm::translate(model, glm::vec3(10.0f, 0.0f, -5.0f));
		shader.SetUniformMat4("model", model);
		obj2.Draw(shader);
		//Geometries::RenderCube();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 20.0f, 0.0f));
		model = glm::scale(model, glm::vec3(20.0f));
		shader.SetUniformMat4("model", model);
		shader.SetUniform1i("normalReverse", 1);
		shader.SetUniform3v("color", glm::vec3(0.5f));
		Geometries::RenderCube();

#pragma endregion Map

#pragma region Light
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f));
		light_shader.Bind();
		light_shader.SetUniformMat4("view", view);
		light_shader.SetUniformMat4("projection", projection);
		light_shader.SetUniformMat4("model", model);
		Geometries::RenderCube();
#pragma endregion Light

#pragma region Screen
		glViewport(0, 0, 200, 200);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depth_texture);

		debug_shader.Bind();
		debug_shader.SetUniform1i("shadow_texture", 0);
		Geometries::RenderQuad();
#pragma endregion Screen

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();

		double delta_time = current_time - last_time;
		float time_step = 1.0 / 144.0;
		if (delta_time >= time_step)
		{
			speed = 0.005;
			ProcessInput(window);
			camera.UpdateCameraPosition();
		}
	}

	glfwTerminate();
	return 0;
}