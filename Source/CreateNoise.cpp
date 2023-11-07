/*

*/

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/noise.hpp>

#include <iostream>
#include <memory>

#include "../Headers/Model.h"
#include "../Headers/Shader.h"
#include "../Headers/Texture.h"
#include "../Headers/Camera.h"
#include "../Headers/VertexArrayObject.h"
#include "../Headers/VertexBufferObject.h"
#include "../Headers/Geometries.h"

Camera camera{ glm::vec3(0, 0.0f, 0.0f), 0.0f, 0.0f, glm::vec3(0, 1.0f, 0) };
float speed = 0.0001f;

constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 600;

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

void RenderQuad();

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
	std::string shader_dir = "./Shaders/CrateNoise/";
	Shader shader{ shader_dir + "noise.vs", shader_dir + "noise.fs" };

	unsigned int width = 128;
	unsigned int height = 128;

	std::unique_ptr<float[]> data = std::make_unique < float[] >(width * height * 4);
	float x_pos = 1.0f / (static_cast<float>(width - 1));
	float y_pos = 1.0f / (static_cast<float>(height - 1));
	for (unsigned int row = 0; row < height; row++)
	{
		for (unsigned int col = 0; col < width; col++)
		{
			float x = col * x_pos;
			float y = row * y_pos;
			float sum = 0.0f;
			float freq = 4.0f;
			float scale = 0.5f;

			for (int oct = 0; oct < 4; oct++)
			{
				glm::vec2 p(x * freq, y * freq);
				float value = glm::perlin(p) / scale;
				sum += value;
				float result = (sum + 1.0f) / 2.0f;
				data[((row * width + col) * 4.0f) + oct] = abs(result);

				freq *= 2.0f;
				scale *= scale;
			}
		}
	}

	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_FLOAT, data.get());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	GLCall(glGenerateMipmap(GL_TEXTURE_2D));
#pragma endregion Configue

	float fps = 0;
	float frames;
	double last_time = glfwGetTime();
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		glfwSwapInterval(0);

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
		glClear(GL_COLOR_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		shader.Bind();
		shader.SetUniform1i("noise_texture", 0);

		Geometries::RenderCube();

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();

		double delta_time = current_time - last_time;
		float time_step = 1.0 / 144.0;
		if (delta_time >= time_step)
		{
			speed = 0.00005;
			ProcessInput(window);
			camera.UpdateCameraPosition();
		}
	}

	glfwTerminate();
	return 0;
}