#pragma once
#include "gl_camera.h"
#include "gl_geometry.h"
#include "gl_shader.h"
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <map>
#include <functional>

namespace GLRendering {

	struct ModelAttrib{
			std::shared_ptr<Geometry> model;
			std::shared_ptr<std::vector<float>> data;
			glm::mat4 modelMatrix;
			glm::vec3 modelColor;
			GLenum renderingMode;
	};

	class Viewer {
	public:
		static Viewer& Instance() {
			static Viewer viewer;
			return viewer;
		}
		
		Viewer(const Viewer &) = delete;
		Viewer &operator=(const Viewer &) = delete;
		~Viewer() {}
		Viewer();

		

		/**
		* @brief Set up the OpenGL Window for Display
		*
		* @param[in] screen_width 
		* @param[in] screen_height 
		*/
		bool SetUp(int screen_width, int screen_height, 
					   const std::string& vertex_shader_path, const std::string& fragment_shader_path);

		/**
		* @brief Main Loop of OpenGL Rendering in a fixed pipeline
		*
		* @param None
		*/
		void RunFixed();

		/**
		* @brief Main Loop of OpenGL Rendering in a modern pipeline
		*
		* @param None
		*/
		void Run();

		void RenderFixed();


		void Render();

		ModelAttrib* CreateMeshPNC(const float* pData, const int stride, const int num_vertex, const glm::vec3& modelColor,
										const glm::mat4& modelMatrix);
		
		



	private:
		static std::shared_ptr<Geometry> CreateGeometryPNC(const float* pData, const int stride,
                                  const int num_vertex,
                                  unsigned int* indices, const int num_indices); //analogous to SimpleApp::CreateFromObjFile
		static bool InitializeShader(Shader &whichShader, const char *vertexPath, const char *fragmentPath,
                     				 const char *vertexName, const char *fragmentName);

		bool InitializeContext();
		int screen_width_, screen_height_;
		bool b_setup_;
		bool firstMouse_;
		std::string vertex_shader_path_, fragment_shader_path_;
		glm::vec3 light_pos_;
		std::map<int, ModelAttrib> models_;
		bool b_show_wireframe_;
		Shader shader_;
		GLFWwindow* window_;
		
	};

	class CallBackController {
	public:
		static CallBackController& Instance() {
			static CallBackController cb_controller;
			return cb_controller;
		}
		CallBackController(){
			camera = nullptr;
		}
		~CallBackController(){}
		CallBackController(const CallBackController &) = delete;
		CallBackController &operator=(const CallBackController &) = delete;

		/**
		* @brief access member variable `camera`
		*
		*/
		const TurnTableCamera& GetCamera() { return (*camera); }
		/**
		* @brief set up camera
		*
		* @param[in] eye_x
		* @param[in] eye_y
		* @param[in] eye_z
		*/
		void SetUp(float eye_x = 0.0f, float eye_y = 0.0f, float eye_z = 0.0f);
		
		///////////////////////////////////////////
		// OpenGL CallBack
		///////////////////////////////////////////
		/**
		* @brief Window size callback function
		*
		* @param[in] window
		* @param[in] width
		* @param[in] height
		*/
		static void FramebufferSizeCallback(GLFWwindow* window, int width, int height) { //forbid this as argument occuring in argument list
			CallBackController::Instance().FramebufferSizeCallbackImpl(window, width, height);
		}
		/**
		* @brief Mouse button clicking callback function
		*
		* @param[in] window
		* @param[in] x
		* @param[in] y
		*/
		static void MouseMotionCallback(GLFWwindow* window, double x, double y) {
			CallBackController::Instance().MouseMotionCallbackImpl(window, x, y);
		}
		/**
		* @brief Keyboard clicking callback function
		*
		* @param[in] window
		*/
		static void KeyBoardCallBack(GLFWwindow *window){
			CallBackController::Instance().KeyBoardCallBackImpl(window);
		}
		
		///////////////////////////////////////////
		// OpenGL CallBack Implementation
		///////////////////////////////////////////
		/**
		* @brief Implementation of window size callback function
		*
		* @param[in] window
		* @param[in] width
		* @param[in] height
		*/
		void FramebufferSizeCallbackImpl(GLFWwindow* window, int width, int height);
		/**
		* @brief Implementation of mouse button clicking callback function
		*
		* @param[in] window
		* @param[in] x
		* @param[in] y
		*/
		void MouseMotionCallbackImpl(GLFWwindow* window, double x, double y);
		/**
		* @brief Implementation of keyboard clicking callback function
		*
		* @param[in] window
		*/
		void KeyBoardCallBackImpl(GLFWwindow *window);

		

	private:
		std::shared_ptr<TurnTableCamera> camera;		

	};
}