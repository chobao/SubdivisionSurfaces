#include "gl_viewer.h"

#include <glm/glm.hpp>
//#include <glut/glut.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "utils/common_utils.h"

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
	bool Viewer::SetUp(int screen_width, int screen_height, 
					   const std::string& vertex_shader_path, const std::string& fragment_shader_path) 
	{
		screen_width_ = screen_width;
		screen_height_ =screen_height;
		firstMouse_ = true;
		vertex_shader_path_ = vertex_shader_path, fragment_shader_path_ = fragment_shader_path;

		CallBackController::Instance().SetUp(0.0f, 0.0f, 5.0f);  //set up camera

		if(InitializeContext()) {
			b_setup_ = true;
		}
		return b_setup_;
		

	}

	bool Viewer::InitializeContext() {
		// glfw: initialize and configure
		// ------------------------------

		glfwInit();
		/*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);*/

#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

		// glfw window_ creation
		// --------------------
		window_ = glfwCreateWindow(screen_width_, screen_height_, "MyOpenGL", NULL, NULL);
		if (window_ == NULL)
		{
			std::cout << "Failed to create GLFW window_" << std::endl;
			glfwTerminate();
			return false;
		}
		glfwMakeContextCurrent(window_);


		glfwSetFramebufferSizeCallback(window_, CallBackController::FramebufferSizeCallback);
		glfwSetCursorPosCallback(window_, CallBackController::MouseMotionCallback);
		//glfwSetScrollCallback(window_, scroll_callback);
		//glfwSetMouseButtonCallback(window_, DefaultMotionFunc);

		// tell GLFW to capture our mouse
		glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		//glfwSetInputMode(window_, GLFW_STICKY_KEYS, GL_TRUE);
		// glad: load all OpenGL function pointers
		// ---------------------------------------

		if (glewInit() != GLEW_OK)
		{
			std::cout << "Failed to initialize GLEW" << std::endl;
			return false;
		}

    	if(!InitializeShader(shader_, vertex_shader_path_.c_str(), fragment_shader_path_.c_str(), "Vs", "Fs")) {
			return false;
		}

		glEnable(GL_DEPTH_TEST);
		return true;

	}
	/**
	* @brief Main Loop of OpenGL Rendering in a fixed pipeline
	*
	* @param None
	*/
	void Viewer::RunFixed() {

		if (!b_setup_) {
			std::cerr << "Error: Please first invoke SetUp() function.\n";
			return;
		}

		float w = 0.1f;
		float h = w * 0.5f;
		float z = w * 0.6f;

		// -----------------------------


		while (!glfwWindowShouldClose(window_))
		{
			//// per-frame time logic
			//// --------------------
			float currentframe = glfwGetTime();

			CallBackController::KeyBoardCallBack(window_);

			const auto& camera = CallBackController::Instance().GetCamera();

			glMatrixMode(GL_PROJECTION); //project matrix
			glLoadIdentity();
			//gluPerspective(glm::radians(camera.zoom_), (float)screen_width_ / (float)screen_height_, 0.1f, 10000.0f);
			glm::mat4 projection = glm::perspective(glm::radians(camera.zoom_), (float)screen_width_ / (float)screen_height_, 0.1f, 10000.0f);
			glMultMatrixf(glm::value_ptr(projection));


			
			glMatrixMode(GL_MODELVIEW); //view matrix
			glPushMatrix();
			glLoadIdentity();

			{
				// gluLookAt(camera.eye_x_, camera.eye_y_, camera.eye_z_, camera.eye_x_, camera.eye_y_, camera.eye_z_ - 1, 0, 1, 0);
				// //	turn table
				// glRotatef(camera.spin_y_, 1, 0, 0);
				// glRotatef(camera.spin_x_, 0, 1, 0);
				glMultMatrixf(glm::value_ptr(camera.ViewMatrix()));
			}

			
			// ------
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// //Display
				// //global coordinate 
				// glBegin(GL_LINES); 
				// glColor3f(1.0, 0.0, 0.0); 
				// glVertex3f(0.0, 0.0, 0.0); 
				// glVertex3d(1.0, 0.0, 0.0); 

				// glColor3f(0.0, 1.0, 0.0); 
				// glVertex3f(0.0, 0.0, 0.0); 
				// glVertex3d(0.0, 1.0, 0.0); 

				// glColor3f(0.0, 0.0, 1.0); 
				// glVertex3f(0.0, 0.0, 0.0); 
				// glVertex3d(0.0, 0.0, 1.0); 
				// glEnd(); 
			// render

			//	========================================================

			glPopMatrix(); // paired with glPushMatrix() above


			// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
			// -------------------------------------------------------------------------------
			glfwSwapBuffers(window_);
			glfwPollEvents();
		}
		// glfw: terminate, clearing all previously allocated GLFW resources.
		// ------------------------------------------------------------------
		glfwTerminate();
		return;
	}

	/**
	* @brief Main Loop of OpenGL Rendering in a modern pipeline
	*
	* @param None
	*/
	void Viewer::Run() {

		if (!b_setup_) {
			std::cerr << "Error: Please first invoke SetUp() function.\n";
			return;
		}

		// -----------------------------


		while (!glfwWindowShouldClose(window_))
		{
			//// per-frame time logic
			//// --------------------
			float currentframe = glfwGetTime();

			CallBackController::KeyBoardCallBack(window_);

			
			// ------
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//	========================================================
			Render();
			//  ========================================================


			// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
			// -------------------------------------------------------------------------------
			glfwSwapBuffers(window_);
			glfwPollEvents();
		}
		// glfw: terminate, clearing all previously allocated GLFW resources.
		// ------------------------------------------------------------------
		glfwTerminate();
		return;
	}



	bool Viewer::InitializeShader(Shader &whichShader, const char *vertexPath, const char *fragmentPath,
                     const char *vertexName, const char *fragmentName)
    {
        int fileBuffSize;

        char *pVsBuffer = (char *) CommonUtils::GetFileBuffer(vertexPath, &fileBuffSize);
        if (pVsBuffer == NULL) {
            return false;
        }

        char *pFsBuffer = (char *) CommonUtils::GetFileBuffer(fragmentPath, &fileBuffSize);
        if (pFsBuffer == NULL) {
            free(pVsBuffer);
            return false;
        }

        int numVertStrings = 0;
        int numFragStrings = 0;
        const char *vertShaderStrings[16];
        const char *fragShaderStrings[16];

        vertShaderStrings[numVertStrings++] = pVsBuffer;
        fragShaderStrings[numFragStrings++] = pFsBuffer;
        bool succ = whichShader.Initialize(numVertStrings, vertShaderStrings, numFragStrings, fragShaderStrings,
                               vertexName, fragmentName);

        free(pVsBuffer);
        free(pFsBuffer);
		return succ;
    }

	void Viewer::Render(){
		const auto& camera = CallBackController::Instance().GetCamera();
		glm::mat4 projection = glm::perspective(glm::radians(camera.zoom_), (float)screen_width_ / (float)screen_height_, 0.1f, 100.0f);
		glm::mat4 view_matrix = camera.ViewMatrix();
		//Output("view_matrix", view_matrix);


		shader_.Bind();
		shader_.SetUniformMat4("projectionMatrix", projection);
		shader_.SetUniformMat4("viewMatrix", view_matrix);

		{
			for(auto& modelItem: models_) {
				shader_.SetUniformMat4("modelMatrix",modelItem.second.modelMatrix);
				modelItem.second.model->Submit(modelItem.second.renderingMode);
			}
		}
		
		shader_.Unbind();
	}
	

	ModelAttrib* Viewer::CreateMeshPNC(const float* pData, const int stride, const int num_vertex,
										const glm::mat4& modelMatrix) {  
		
		
		const int num_indices = num_vertex;
		unsigned int* indices = new unsigned int[num_indices];
		for(int i = 0 ; i < num_indices ; i++) {
			indices[i] = i;
		}

		const int idx = models_.size();
		models_[idx].model = CreateGeometryPNC(pData, stride,num_vertex,indices, num_indices);
		models_[idx].modelMatrix = modelMatrix;
		models_[idx].renderingMode = GL_TRIANGLES;
		return &(models_[idx]);
		
	}

	/**
		* @brief Create Geometry as the order `position normal color`
		*
	*/
	std::shared_ptr<Geometry> Viewer::CreateGeometryPNC(const float* pData, const int stride,
                                  const int num_vertex,
                                  unsigned int* indices, const int num_indices) {
		
		const int vertexSize = stride * sizeof(float);
		ProgramAttribute attribs[3];
		int nAttribs = 3;
		attribs[0].index = kPosition;
		attribs[0].size = 3;
		attribs[0].type = GL_FLOAT;
		attribs[0].normalized = false;
		attribs[0].stride = vertexSize;
		attribs[0].offset = 0;

		attribs[1].index = kNormal;
		attribs[1].size = 3;
		attribs[1].type = GL_FLOAT;
		attribs[1].normalized = false;
		attribs[1].stride = vertexSize;
		attribs[1].offset = 3 * sizeof(float);

		attribs[2].index = kColor;
		attribs[2].size = 3;
		attribs[2].type = GL_FLOAT;
		attribs[2].normalized = false;
		attribs[2].stride = vertexSize;
		attribs[2].offset = 6 * sizeof(float);

		

		std::shared_ptr<Geometry> new_model = std::make_shared<Geometry>();
		new_model->Initialize(&attribs[0], nAttribs,
							indices, num_indices,
							(const void*)pData, vertexSize * num_vertex, num_vertex);
		{
			printf("INFO(BCHO): mnew_model[] != NULL %d\n",  new_model != NULL);
			printf("INFO(BCHO): GetVertexCount %d\n", new_model->GetVertexCount());
			printf("INFO(BCHO): GetIndexCount %d\n", new_model->GetIndexCount());
			printf("INFO(BCHO): GetIbId %d\n", new_model->GetIbId());
			printf("INFO(BCHO): GetVaoId %d\n", new_model->GetVaoId());
			printf("INFO(BCHO): GetVbId %d\n", new_model->GetVbId());
		}
    	return new_model;
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
	* @brief Implementation of window_ size callback function
	*
	* @param[in] window_
	* @param[in] width
	* @param[in] height
	*/
	void CallBackController::FramebufferSizeCallbackImpl(GLFWwindow* window_, int width, int height)
	{
		// make sure the viewport matches the new window_ dimensions; note that width and 
		// height will be significantly larger than specified on retina displays.
		glViewport(0, 0, width, height);
	}
	/**
	* @brief Implementation of mouse button clicking callback function
	*
	* @param[in] window_
	* @param[in] x
	* @param[in] y
	*/
	void CallBackController::MouseMotionCallbackImpl(GLFWwindow* window_, double x, double y) {
		int LEFT_BUTTON = glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_LEFT);
		int MIDDLE_BUTTON = glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_MIDDLE);
		int RIGHT_BUTTON = glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_RIGHT);


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
	* @param[in] window_
	*/
	void CallBackController::KeyBoardCallBackImpl(GLFWwindow *window_)
	{
		if (glfwGetKey(window_, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window_, true);
			
	}

}