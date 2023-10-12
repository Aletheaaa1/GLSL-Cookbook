/*
	����͸���ȣ�
	ͨ��SSBO����ɫ���洢��������Լ�ͼ�����/�洢ʵ��OIT��

	1.	����Ⱦ���������壬��Ϊÿ�����ش洢һ������
	2.	����һ��Ļ�ռ䣬Ϊÿ�����ص���Ƭ����ɫ������ȡƬ����ɫ���е���������ȴӴ�С��Ƭ�ν�������
*/

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "../Headers/Model.h"
#include "../Headers/Shader.h"
#include "../Headers/Camera.h"

Camera camera{ glm::vec3(0, 0, 10.0f), 0.0f, 0.0f, glm::vec3(0, 1.0f, 0) };
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

int main()
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
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

#pragma region Configure
	Model object{ "./Models/ball/ball.obj" };
	Shader shader{ "./Shaders/OIT/.vs", "./Shaders/OIT/.fs" };

	//	nodeSize����Ƭ����ɫ����ʹ�õĽṹ��NodeType�Ĵ�С���ڴ���ĺ�벿�֣������Ǹ���std430���ּ���ġ�
	int node_size = 5 * sizeof(float) + sizeof(unsigned int);
	int max_nodes = 20 * 800 * 600;
	//	atomic counter
	unsigned int counter_buffer;
	glGenBuffers(1, &counter_buffer);
	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, counter_buffer);
	glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(unsigned int), nullptr, GL_DYNAMIC_DRAW);

	//	linked list storage
	unsigned int list_buffer;
	glGenBuffers(1, &list_buffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, list_buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, node_size * max_nodes, nullptr, GL_DYNAMIC_DRAW);

	//	The buffer for the head pointers, as an image texture
	unsigned int head_ptr_texture;
	glGenTextures(1, &head_ptr_texture);
	glBindTexture(GL_TEXTURE_2D, head_ptr_texture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32UI, 800, 600);
	glBindImageTexture(0, head_ptr_texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);

	//	��Ⱦ��ÿһ֡��������Ҫ��������������Ϊ 0xffffffffff ֵ���Ӷ��������
	std::vector<unsigned int> head_ptr_clear(800 * 600, 0xffffffff);
	unsigned int clear_buffer;
	glGenBuffers(1, &clear_buffer);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, clear_buffer);
	glBufferData(GL_PIXEL_UNPACK_BUFFER, sizeof(unsigned int) * head_ptr_clear.size(), head_ptr_clear.data(), GL_DYNAMIC_DRAW);

#pragma endregion Configure

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		ProcessInput(window);
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		camera.UpdateCameraPosition();

		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)800 / (float)600, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(0.3f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

		shader.Bind();

		shader.SetUniformMat4("model", model);
		shader.SetUniformMat4("view", view);
		shader.SetUniformMat4("projection", projection);
		object.Draw(shader);

		/* Swap front and back buffers */
		glfwSwapBuffers(window);
		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}