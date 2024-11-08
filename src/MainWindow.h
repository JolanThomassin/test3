#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <iostream>
#include <vector>
#include <memory>

#include "ShaderProgram.h"
#include "Camera.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

class MainWindow
{
public:
	MainWindow();

	// Main functions (initialization, run)
	int Initialisation();
	int RenderLoop();

	// Callback to intersept GLFW calls
	void FramebufferSizeCallback(int width, int height);
	void MouseButtonCallback(int button, int action, int mods);
	void CursorPositionCallback(double xpos, double ypos);

private:
	// Initialize GLFW callbacks
	void InitializeCallback();
	// Intiialize OpenGL objects (shaders, ...)
	int InitializeGL();

	// Rendering scene (OpenGL)
	void RenderScene(float delta_time);
	// Rendering interface ImGUI
	void RenderImgui();

	void initGeometryCube();

	// Perform cube/face/control point selection on the object
	void PerformCubeSelection(int xpos, int ypos);
	void PerformFaceSelection(int xpos, int ypos);
	void PerformControlPointSelection(int xpos, int ypos);
private:
	// settings
	unsigned int m_windowWidth = 900;
	unsigned int m_windowHeight = 900;

	int gridSize = 10;
	float cubeSize = 1.0f;
	float offset = (gridSize - 1) * cubeSize / 2.0f;

	// GLFW Window
	GLFWwindow* m_window = nullptr;

	// Shader informations
	std::unique_ptr<ShaderProgram> m_mainShader = nullptr;
	struct {
		GLint modelView = 0;
		GLint modelViewNormal = 1;
		GLint projection = 2;
	} m_mainShaderUniform;
	std::unique_ptr<ShaderProgram> m_pickingShader = nullptr;
	struct {
		GLint uProjMatrix;
		GLint uColor;
		GLint uMatrix;
	} m_pickingShaderLocations;

	// Camera
	Camera m_camera;
	bool m_imGuiActive = false;

	// VAOs and VBOs
	enum VAO_IDs { VAO_Cube, NumVAOs };
	enum Buffer_IDs { Position, Normal , Indices, NumBuffers };
	GLuint m_VAOs[NumVAOs];
	GLuint m_Buffers[NumBuffers];
	size_t m_nbTri;
};
