#include "MainWindow.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <iostream>
#include <vector>

#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))
#define ACTIVE_DEBUG_CALLBACK 0

MainWindow::MainWindow() :
	m_camera(m_windowWidth, m_windowHeight,
		glm::vec3(2.0,2.0,2.0),
		glm::vec3(0.0,0.0,0.0))
{
}

int MainWindow::Initialisation()
{
	// OpenGL version (usefull for imGUI and other libraries)
	const char* glsl_version = "#version 460 core";

	// glfw: initialize and configure
	// ------------------------------
	glfwInit();

	// Request OpenGL 4.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#if ACTIVE_DEBUG_CALLBACK
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	m_window = glfwCreateWindow(m_windowWidth, m_windowHeight, "Labo 2", nullptr, nullptr);
	if (m_window == nullptr)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return 1;
	}

	glfwMakeContextCurrent(m_window);
	InitializeCallback();

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Failed to initialize GLAD" << std::endl;
		return 2;
	}

	// imGui: create interface
	// ---------------------------------------
	// Setup Dear ImGui context
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;// Enable Keyboard Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(m_window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	// Activate debug callback (if needed)
	// -----------------------------
#if ACTIVE_DEBUG_CALLBACK
	int flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
	{
		std::cout << "Debug context created\n";
		// initialize debug output 
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(glDebugOutput, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_FALSE);
		glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_ERROR, GL_DONT_CARE, 0, NULL, GL_TRUE);
	}
#endif

	// Other openGL initialization
	// -----------------------------
	return InitializeGL();
}

void MainWindow::InitializeCallback() {
	glfwSetWindowUserPointer(m_window, reinterpret_cast<void*>(this));
	glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow* window, int width, int height) {
		MainWindow* w = reinterpret_cast<MainWindow*>(glfwGetWindowUserPointer(window));
		w->FramebufferSizeCallback(width, height);
		});
	glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods) {
		MainWindow* w = reinterpret_cast<MainWindow*>(glfwGetWindowUserPointer(window));
		w->MouseButtonCallback(button, action, mods);
		});
	
	glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double xpos, double ypos) {
		MainWindow* w = reinterpret_cast<MainWindow*>(glfwGetWindowUserPointer(window));
		w->CursorPositionCallback(xpos, ypos);
	});

}

int MainWindow::InitializeGL()
{
	const std::string directory = SHADERS_DIR;

	// Main shader loading
	bool mainShaderSuccess = true;
	m_mainShader = std::make_unique<ShaderProgram>();
	mainShaderSuccess &= m_mainShader->addShaderFromSource(GL_VERTEX_SHADER, directory + "basicShader.vert");
	mainShaderSuccess &= m_mainShader->addShaderFromSource(GL_FRAGMENT_SHADER, directory + "basicShader.frag");
	mainShaderSuccess &= m_mainShader->link();
	if (!mainShaderSuccess) {
		std::cerr << "Error when loading main shader\n";
		return 4;
	}

	// Check the uniform locations
	m_mainShaderUniform.modelView = m_mainShader->uniformLocation("uMVMatrix");
	m_mainShaderUniform.modelViewNormal = m_mainShader->uniformLocation("uNormalMVMatrix");
	m_mainShaderUniform.projection = m_mainShader->uniformLocation("uProjMatrix");
	if (m_mainShaderUniform.modelView < 0 || m_mainShaderUniform.modelViewNormal < 0 || m_mainShaderUniform.projection < 0) {
		std::cerr << "Error when getting uniform locations\n";
		return 5;
	}

	// Create our VertexArrays Objects and VertexBuffer Objects
	glCreateVertexArrays(NumVAOs, m_VAOs);
	glCreateBuffers(NumBuffers, m_Buffers);
	initGeometryCube();

	// Init GL properties
	glPointSize(10.0f);
	glEnable(GL_DEPTH_TEST);

	// Setup projection matrix (a bit hacky)
	FramebufferSizeCallback(m_windowWidth, m_windowHeight);

	return 0;
}

void MainWindow::RenderImgui()
{
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	//imgui 
	{
		ImGui::Begin("Labo 2");
		bool notSelectedCube = true;
		ImGui::Text("Rotation Cube: ");
		ImGui::BeginDisabled(notSelectedCube);
		// X
		bool rotationXN = ImGui::Button("X-"); 
		ImGui::SameLine(); 
		bool rotationXP = ImGui::Button("X+");
		// Y
		bool rotationYN = ImGui::Button("Y-"); 
		ImGui::SameLine(); 
		bool rotationYP = ImGui::Button("Y+");
		// Z
		bool rotationZN = ImGui::Button("Z-"); 
		ImGui::SameLine(); 
		bool rotationZP = ImGui::Button("Z+");
		ImGui::EndDisabled();

		// Position curve
		ImGui::Separator();
		bool notSelectedControlPoint = true;
		ImGui::Text("Point de controle: ");
		ImGui::BeginDisabled(notSelectedControlPoint);
		static glm::vec3 position;  // TODO: Utiliser un attribut de classe
		ImGui::SliderFloat3("Position", &position[0], -10, 10);
		ImGui::EndDisabled();

		ImGui::End();

		// Logic
		if(rotationXN) {
			std::cout << "Rotation -X\n";
		}
		if(rotationYN) {
			std::cout << "Rotation -Y\n";
		}
		if(rotationZN) {
			std::cout << "Rotation -Z\n";
		}
		if(rotationXP) {
			std::cout << "Rotation +X\n";
		}
		if(rotationYP) {
			std::cout << "Rotation +Y\n";
		}
		if(rotationZP) {
			std::cout << "Rotation +Z\n";
		}
	}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void MainWindow::RenderScene(float delta_time)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Bind our vertex/fragment shaders
	glUseProgram(m_mainShader->programId());

	glm::mat4 viewMatrix = m_camera.viewMatrix();
	glm::vec3 lightPosition = viewMatrix * glm::vec4(1.0);

	// Set projection matrix
	m_mainShader->setMat4(m_mainShaderUniform.projection, m_camera.projectionMatrix());


	// Loop that renders each cube
	for (int i = 0; i < gridSize; ++i)
	{
		for (int j = 0; j < gridSize; ++j)
		{
			// Calculate the position for each cube
			float x = i * cubeSize - offset;
			float z = j * cubeSize - offset;
			glm::vec3 position(x, 0.0f, z);

			// Move the cube to position (x, 0, z)
			glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), position);

			// Model-view and normal matrix
			glm::mat4 modelViewMatrix = viewMatrix * modelMatrix;
			glm::mat3 normalMat = glm::inverseTranspose(glm::mat3(modelViewMatrix));

			// Send matrices to shader
			m_mainShader->setMat4(m_mainShaderUniform.modelView, modelViewMatrix);
			m_mainShader->setMat3(m_mainShaderUniform.modelViewNormal, normalMat);

			// Draw a cube
			glBindVertexArray(m_VAOs[VAO_Cube]);
			glDrawElements(GL_TRIANGLES, (GLsizei)(m_nbTri * 3), GL_UNSIGNED_INT, nullptr);
		}
	}
}

int MainWindow::RenderLoop()
{
	float time = (float)glfwGetTime();
	while (!glfwWindowShouldClose(m_window))
	{
		// Compute delta time between two frames
		float new_time = (float)glfwGetTime();
		const float delta_time = new_time - time;
		time = new_time;

		// Check inputs: Does ESC was pressed?
		if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(m_window, true);
		if(!m_imGuiActive) {
			m_camera.keybordEvents(m_window, delta_time);
		}

		RenderScene(delta_time);
		RenderImgui();

		// Show rendering and get events
		glfwSwapBuffers(m_window);
		m_imGuiActive = ImGui::IsAnyItemActive();
		glfwPollEvents();
	}

	// Cleanup
	glfwDestroyWindow(m_window);
	glfwTerminate();

	return 0;
}

void MainWindow::initGeometryCube()
{
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	// Base positions
	positions = {
		glm::vec3(-0.5, -0.5,  0.5),
		glm::vec3(0.5, -0.5,  0.5),
		glm::vec3(0.5,  0.5,  0.5),
		glm::vec3(-0.5,  0.5,  0.5),
		glm::vec3(-0.5, -0.5, -0.5),
		glm::vec3(0.5, -0.5, -0.5),
		glm::vec3(0.5,  0.5, -0.5),
		glm::vec3(-0.5,  0.5, -0.5)
	};

	normals = {
		glm::vec3(0, 0, 1), glm::vec3(0, 0, 1), glm::vec3(0, 0, 1), glm::vec3(0, 0, 1), // Front face
		glm::vec3(0, 0, -1), glm::vec3(0, 0, -1), glm::vec3(0, 0, -1), glm::vec3(0, 0, -1), // Back face
		glm::vec3(1, 0, 0), glm::vec3(1, 0, 0), glm::vec3(1, 0, 0), glm::vec3(1, 0, 0), // Right face
		glm::vec3(-1, 0, 0), glm::vec3(-1, 0, 0), glm::vec3(-1, 0, 0), glm::vec3(-1, 0, 0), // Left face
		glm::vec3(0, 1, 0), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0), // Top face
		glm::vec3(0, -1, 0), glm::vec3(0, -1, 0), glm::vec3(0, -1, 0), glm::vec3(0, -1, 0) // Bottom face
	};

	std::vector<glm::uvec3> indices = {
		// Front face
		{0, 1, 2}, {0, 2, 3},
		// Back face
		{4, 5, 6}, {4, 6, 7},
		// Right face
		{1, 5, 6}, {1, 6, 2},
		// Left face
		{0, 4, 7}, {0, 7, 3},
		// Top face
		{3, 2, 6}, {3, 6, 7},
		// Bottom face
		{0, 1, 5}, {0, 5, 4}
	};
	m_nbTri = indices.size();

	// Upload data
	glNamedBufferData(m_Buffers[Position], sizeof(glm::vec3) * positions.size(), positions.data(), GL_STATIC_DRAW);
	glNamedBufferData(m_Buffers[Normal], sizeof(glm::vec3) * normals.size(), normals.data(), GL_STATIC_DRAW);
	glNamedBufferData(m_Buffers[Indices], sizeof(glm::uvec3) * indices.size(), indices.data(), GL_STATIC_DRAW);
	// Positions
	int vPositionLocation = m_mainShader->attributeLocation("vPosition");
	glVertexArrayAttribFormat(m_VAOs[VAO_Cube],
		vPositionLocation, // Attribute index 
		3, // Number of components
		GL_FLOAT, // Type 
		GL_FALSE, // Normalize 
		0 // Relative offset (first component)
	);
	glVertexArrayVertexBuffer(m_VAOs[VAO_Cube],
		vPositionLocation, // Binding point 
		m_Buffers[Position], // VBO 
		0, // Offset (when the position starts)
		sizeof(glm::vec3) // Stride
	);
	glEnableVertexArrayAttrib(m_VAOs[VAO_Cube],
		vPositionLocation // Attribute index
	);
	glVertexArrayAttribBinding(m_VAOs[VAO_Cube],
		vPositionLocation, // Attribute index
		vPositionLocation  // Binding point
	);
	// Normals
	int vNormalLocation = m_mainShader->attributeLocation("vNormal");
	glVertexArrayAttribFormat(m_VAOs[VAO_Cube],
		vNormalLocation, // Attribute index 
		3, // Number of components
		GL_FLOAT, // Type 
		GL_FALSE, // Normalize 
		0 // Relative offset (first component)
	);
	glVertexArrayVertexBuffer(m_VAOs[VAO_Cube],
		vNormalLocation, // Binding point 
		m_Buffers[Normal], // VBO 
		0, // Offset (when the position starts)
		sizeof(glm::vec3) // Stride
	);
	glEnableVertexArrayAttrib(m_VAOs[VAO_Cube],
		vNormalLocation // Attribute index
	);
	glVertexArrayAttribBinding(m_VAOs[VAO_Cube],
		vNormalLocation, // Attribute index
		vNormalLocation  // Binding point
	);
	// Indices
	glVertexArrayElementBuffer(m_VAOs[VAO_Cube], m_Buffers[Indices]);
}

void MainWindow::FramebufferSizeCallback(int width, int height) {
	m_windowWidth = width;
	m_windowHeight = height;
	glViewport(0, 0, width, height);
	m_camera.viewportEvents(width, height);
}

void MainWindow::CursorPositionCallback(double xpos, double ypos) {
	if(!m_imGuiActive) {
		int state = glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_LEFT);
		m_camera.mouseEvents(glm::vec2(xpos, ypos), state == GLFW_PRESS);
	}
}

void MainWindow::PerformCubeSelection(int xpos, int ypos)
{
	
}

void MainWindow::PerformFaceSelection(int xpos, int ypos)
{

}

void MainWindow::PerformControlPointSelection(int xpos, int ypos)
{

}

void MainWindow::MouseButtonCallback(int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && mods == GLFW_MOD_CONTROL)
	{
		double xpos, ypos;
		glfwGetCursorPos(m_window, &xpos, &ypos);
		PerformCubeSelection((int)xpos, (int)ypos);
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && mods == GLFW_MOD_SHIFT)
	{
		double xpos, ypos;
		glfwGetCursorPos(m_window, &xpos, &ypos);
		PerformFaceSelection((int)xpos, (int)ypos);
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && mods == GLFW_MOD_ALT)
	{
		double xpos, ypos;
		glfwGetCursorPos(m_window, &xpos, &ypos);
		PerformControlPointSelection((int)xpos, (int)ypos);
	}
}