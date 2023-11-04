/*
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

#pragma region Configue

glm::vec3 lightPos = glm::vec3(0.0f, 20.0f, 0.0f);
glm::vec3 lightColor = glm::vec3(1.0f);
#pragma endregion Configue

void RenderScene(Shader& shader, Model& obj, bool);

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
Camera camera{ glm::vec3(0.0f, 20.0f, 0.0f), 0.0f, 0.0f, glm::vec3(0, 1.0f, 0) };
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
	window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hello World", nullptr, nullptr);
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
	Model obj{ "./Models/ball/ball.obj" };
	std::string shader_dir = "./Shaders/ShadowVolume/";
	Shader render_shader{ shader_dir + "render_scene.vs", shader_dir + "render_scene.fs" };
	Shader debug_shader{ shader_dir + "debug.vs", shader_dir + "debug.fs" };
	Shader volume_shader{ shader_dir + "volume.vs", shader_dir + "volume.fs", shader_dir + "volume.gs" };

#pragma region FBO
	unsigned int depth_rbo;
	glGenRenderbuffers(1, &depth_rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, depth_rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCREEN_WIDTH, SCREEN_HEIGHT);

	unsigned int ambient_rbo;
	glGenRenderbuffers(1, &ambient_rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, ambient_rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, SCREEN_WIDTH, SCREEN_HEIGHT);

	unsigned int diff_spec_texture;
	glGenTextures(1, &diff_spec_texture);
	glBindTexture(GL_TEXTURE_2D, diff_spec_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	unsigned int color_depth_fbo;
	glGenFramebuffers(1, &color_depth_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, color_depth_fbo);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_rbo);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, ambient_rbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, diff_spec_texture, 0);

	unsigned int draw_buffers[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, draw_buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "Framebuffer is not complete! \n";
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
#pragma endregion

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

		//	正常渲染物体
#pragma region Pass 1
		glDepthMask(true);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_STENCIL_TEST);
		glBindFramebuffer(GL_FRAMEBUFFER, color_depth_fbo);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		RenderScene(render_shader, obj, false);
#pragma endregion Pass 1

#pragma region Pass 2
		glBindFramebuffer(GL_READ_FRAMEBUFFER, color_depth_fbo);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBlitFramebuffer(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);

		//glColorMask(false, false, false, false);
		//glDepthMask(false);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

		glClear(GL_STENCIL_BUFFER_BIT);
		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_ALWAYS, 0, 0xff);
		glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_KEEP, GL_INCR_WRAP);
		glStencilOpSeparate(GL_BACK, GL_KEEP, GL_KEEP, GL_DECR_WRAP);

		volume_shader.Bind();
		RenderScene(volume_shader, obj, true);

		glColorMask(true, true, true, true);
#pragma endregion Pass 2

#pragma region Debug
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		//glBlendFunc(1, 1);

		glStencilFunc(GL_EQUAL, 0, 0xff);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

		debug_shader.Bind();

		glActiveTexture(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, diff_spec_texture);

		Geometries::RenderQuad();

		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
#pragma endregion Debug

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

void RenderScene(Shader& shader, Model& obj, bool is_volume)
{
	glm::mat4 projection = glm::infinitePerspective(glm::radians(45.0f), 800.0f / 600.0f, 0.01f);
	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 model = glm::mat4(1.0f);

	shader.Bind();
	shader.SetUniformMat4("view", view);
	shader.SetUniformMat4("projection", projection);
	shader.SetUniform3v("lightPos", lightPos);
	shader.SetUniform3v("lightColor", lightColor);
	shader.SetUniform3v("cameraPos", camera.position);
	shader.SetUniform1i("normalReverse", 0);
	shader.SetUniform3v("color", glm::vec3(0.5f));

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 5.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.50f));
	//model = glm::scale(model, glm::vec3(5.0f));

	shader.SetUniformMat4("model", model);
	obj.Draw(shader);

	if (!is_volume)
	{
		model = glm::translate(model, glm::vec3(10.0f, 0.0f, -5.0f));

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 20.0f, 0.0f));
		model = glm::scale(model, glm::vec3(20.0f));
		shader.SetUniformMat4("model", model);
		shader.SetUniform1i("normalReverse", 1);
		shader.SetUniform3v("color", glm::vec3(0.5f));
		Geometries::RenderCube();
	}
}