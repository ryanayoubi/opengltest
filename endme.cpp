#define GLEW_STATIC

#include <glew.h>
#include <glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct shadertext
{
	const char *vs;
	const char *fs;
};

static struct shadertext ParseShader(const char *filepath)
{
	FILE *shader = fopen(filepath, "r");
	char v[500];
	char f[500];
	fscanf(shader, "%c%[^#]%c%[^#]", v, v + 1, f, f + 1);
	fclose(shader);
	struct shadertext t = {v, f};
	return t;
}

unsigned int shaderProgram;

static void shaderstuff()
{
	struct shadertext shaders = ParseShader("basic.shader");
	shaderProgram = glCreateProgram();

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &shaders.vs, NULL);
	glCompileShader(vertexShader);
	int vresult;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vresult);
	if (vresult == GL_FALSE)
	{
		printf("vertexshader fucked");
	}
	glAttachShader(shaderProgram, vertexShader);

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &shaders.fs, NULL);
	glCompileShader(fragmentShader);
	int fresult;
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fresult);
	if (fresult == GL_FALSE)
	{
		printf("fragmentshader fucked");
	}
	glAttachShader(shaderProgram, fragmentShader);

	glLinkProgram(shaderProgram);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glUseProgram(shaderProgram);
}

float lastX = 1920 / 2;
float lastY = 1080 / 2;
float yaw = 90.0f;
float pitch = 0.0f;

static void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	const float sensitivity = 0.15f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;
	yaw -= xoffset;
	pitch += yoffset;
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_R)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		if (key == GLFW_KEY_F)
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

static void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
}

int main(void)
{
	if (!glfwInit())
		return -1;

	GLFWwindow *window = glfwCreateWindow(1920, 1080, "Hello World", glfwGetPrimaryMonitor(), NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK)
	{
		glfwTerminate();
		return -1;
	}

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glViewport(0, 0, 1920, 1080);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	shaderstuff();

	float vertices[] = {
		-0.5f, -0.5f, 0.5f,
		0.5f, -0.5f, 0.5f,
		0.5f, 0.5f, 0.5f,
		-0.5f, 0.5f, 0.5f,
		-0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f, 0.5f, -0.5f,
		-0.5f, 0.5f, -0.5f};

	unsigned int indices[] = {
		0, 1, 2, 0, 2, 3,
		0, 3, 4, 3, 4, 7,
		2, 3, 7, 2, 7, 6,
		4, 5, 7, 5, 6, 7,
		1, 2, 6, 1, 5, 6,
		0, 1, 4, 1, 4, 5};

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, 8 * 6 * sizeof(float), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	glEnable(GL_DEPTH_TEST);

	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);
	;
	int modelLoc = glGetUniformLocation(shaderProgram, "model");
	int viewLoc = glGetUniformLocation(shaderProgram, "view");
	int projectionloc = glGetUniformLocation(shaderProgram, "projection");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projectionloc, 1, GL_FALSE, glm::value_ptr(projection));

	float fov = 90.0f;

	glm::vec3 cameraPos = glm::vec3(0.0f, 1.0f, 3.0f);
	glm::vec3 cameraFront = glm::normalize(cameraFront = glm::normalize(glm::vec3(cos(glm::radians(yaw)) * cos(glm::radians(pitch)), sin(glm::radians(pitch)), (-1) * sin(glm::radians(yaw)) * cos(glm::radians(pitch)))));
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	float cameraSpeed = 0.3;
	float vspeed = 0.0;
	float vaccel = -0.03;
	bool ground = true;

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS)
		{
			fov = 10;
		}
		else
		{
			fov = 90;
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		{
			cameraSpeed = 1.0f;
			fov = 120;
		}
		else
		{
			cameraSpeed = 0.5f;
		}
		projection = glm::perspective(glm::radians(fov), 1920.0f / 1080.0f, 0.1f, 1000.0f);
		glUniformMatrix4fv(projectionloc, 1, GL_FALSE, glm::value_ptr(projection));

		if ((glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) && ground)
			vspeed = 0.8;
		ground = true;
		if (cameraPos[1] != 1.0F)
			ground = false;
		cameraPos[1] += vspeed;
		vspeed += vaccel;
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			cameraPos += cameraSpeed * glm::normalize(glm::vec3(cameraFront[0], 0.0f, cameraFront[2]));
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			cameraPos -= cameraSpeed * glm::normalize(glm::vec3(cameraFront[0], 0.0f, cameraFront[2]));
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;
		if (cameraPos[1] < 1)
			cameraPos[1] = 1;
		cameraFront = glm::normalize(glm::vec3(cos(glm::radians(yaw)) * cos(glm::radians(pitch)), sin(glm::radians(pitch)), (-1) * sin(glm::radians(yaw)) * cos(glm::radians(pitch))));
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		for (float i = 0; i < 20; i += 1)
		{
			for (float j = 0.0f; j < 20; j += 1)
			{
				for (float k = 0.0f; k < 20; k += 1)
				{
					model = glm::mat4(1.0f);
					model = glm::translate(model, glm::vec3((i * 4) - 38, k * 4, j * -4));
					glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
					glUniform4f(glGetUniformLocation(shaderProgram, "color"), i / 20, j / 20, k / 20, 1.0);
					glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);
				}
			}
		}
		for (float i = 0; i < 101; i++)
		{
			for (float j = 0; j < 101; j++)
			{
				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(i - 50, -1, (j * -1) + 12));
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
				glUniform4f(glGetUniformLocation(shaderProgram, "color"), 0.5, 0.5, 0.5, 1.0);
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);
			}
		}
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}