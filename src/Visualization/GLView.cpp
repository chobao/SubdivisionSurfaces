#include "GLViewer.h"

#include <glm/glm.hpp>
//#include <glut/glut.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

namespace GLRendering {


	Viewer::Viewer() {
		b_setup_ = false;
	}

	/**
	* @brief Set up the OpenGL Window for Display
	*
	* @param[in] screen_width
	* @param[in] screen_height
	*/
	void Viewer::SetUp(int screen_width, int screen_height) 
	{
		screen_width_ = screen_width;
		screen_height_ =screen_height;
		b_setup_ = true;
		firstMouse_ = true;
		CallBackController::Instance().SetUp(0.0f, 0.0f, 0.25f);  //set up camera
	}
	/**
	* @brief Main Loop of OpenGL Rendering
	*
	* @param None
	*/
	void Viewer::Run() {

		if (!b_setup_) {
			std::cerr << "Error: Please first invoke SetUp() function.\n";
			return;
		}

		float w = 0.1f;
		float h = w * 0.5f;
		float z = w * 0.6f;

		// glfw: initialize and configure
		// ------------------------------

		glfwInit();
		/*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);*/

#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

		// glfw window creation
		// --------------------
		GLFWwindow* window = glfwCreateWindow(screen_width_, screen_height_, "MyOpenGL", NULL, NULL);
		if (window == NULL)
		{
			std::cout << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
			return;
		}
		glfwMakeContextCurrent(window);


		glfwSetFramebufferSizeCallback(window, CallBackController::FramebufferSizeCallback);
		glfwSetCursorPosCallback(window, CallBackController::MouseMotionCallback);
		//glfwSetScrollCallback(window, scroll_callback);
		//glfwSetMouseButtonCallback(window, DefaultMotionFunc);

		// tell GLFW to capture our mouse
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		//glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
		// glad: load all OpenGL function pointers
		// ---------------------------------------

		if (glewInit() != GLEW_OK)
		{
			std::cout << "Failed to initialize GLEW" << std::endl;
			return;
		}
		// -----------------------------
		glEnable(GL_DEPTH_TEST);


		while (!glfwWindowShouldClose(window))
		{
			//// per-frame time logic
			//// --------------------
			float currentframe = glfwGetTime();

			CallBackController::KeyBoardCallBack(window);

			const auto& camera = CallBackController::Instance().GetCamera();
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			gluPerspective(glm::radians(camera.zoom_), (float)screen_width_ / (float)screen_height_, 0.1f, 10000.0f);

			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadIdentity();

			{
				gluLookAt(camera.eye_x_, camera.eye_y_, camera.eye_z_, camera.eye_x_, camera.eye_y_, camera.eye_z_ - 1, 0, 1, 0);
				//	turn table
				glRotatef(camera.spin_y_, 1, 0, 0);
				glRotatef(camera.spin_x_, 0, 1, 0);
			}

			//	draw function, we don't check whether draw and picking_draw match

			// render
			// ------
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//Display
			//global coordinate
			glBegin(GL_LINES);
			glColor3f(1.0, 0.0, 0.0);
			glVertex3f(0.0, 0.0, 0.0);
			glVertex3d(1.0, 0.0, 0.0);

			glColor3f(0.0, 1.0, 0.0);
			glVertex3f(0.0, 0.0, 0.0);
			glVertex3d(0.0, 1.0, 0.0);

			glColor3f(0.0, 0.0, 1.0);
			glVertex3f(0.0, 0.0, 0.0);
			glVertex3d(0.0, 0.0, 1.0);
			glEnd();



			//	========================================================

			glPopMatrix();


			// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
			// -------------------------------------------------------------------------------
			glfwSwapBuffers(window);
			glfwPollEvents();
		}
		// glfw: terminate, clearing all previously allocated GLFW resources.
		// ------------------------------------------------------------------
		glfwTerminate();
		return;
	}

	/////////////////////////////////////////////////
	//CallBackController Member Function Definition
	/////////////////////////////////////////////////

	/**
	* @brief set up camera
	*
	* @param[in] eye_x
	* @param[in] eye_y
	* @param[in] eye_z
	*/
	void CallBackController::SetUp(float eye_x, float eye_y, float eye_z) {
		camera = std::make_shared<TurnTableCamera>(eye_x ,eye_y, eye_z);
	}
	
	/**
	* @brief Implementation of window size callback function
	*
	* @param[in] window
	* @param[in] width
	* @param[in] height
	*/
	void CallBackController::FramebufferSizeCallbackImpl(GLFWwindow* window, int width, int height)
	{
		// make sure the viewport matches the new window dimensions; note that width and 
		// height will be significantly larger than specified on retina displays.
		glViewport(0, 0, width, height);
	}
	/**
	* @brief Implementation of mouse button clicking callback function
	*
	* @param[in] window
	* @param[in] x
	* @param[in] y
	*/
	void CallBackController::MouseMotionCallbackImpl(GLFWwindow* window, double x, double y) {
		int LEFT_BUTTON = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
		int MIDDLE_BUTTON = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE);
		int RIGHT_BUTTON = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);


		if (LEFT_BUTTON == GLFW_PRESS) {
			camera->ProcessMouseButton(MouseButton::LEFT_BUTTON, x, y);

		}
		else if (MIDDLE_BUTTON == GLFW_PRESS) {
			camera->ProcessMouseButton(MouseButton::MIDDLE_BUTTON, x, y);

		}
		else if (RIGHT_BUTTON == GLFW_PRESS) {
			camera->ProcessMouseButton(MouseButton::RIGHT_BUTTON, x, y);

		}

		camera->UpdateOldMousePos(x, y);
		//glutPostRedisplay();
	}
	/**
	* @brief Implementation of keyboard clicking callback function
	*
	* @param[in] window
	*/
	void CallBackController::KeyBoardCallBackImpl(GLFWwindow *window)
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);
			
	}

}