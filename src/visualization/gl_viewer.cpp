#include "gl_viewer.h"
#ifdef __APPLE__
#include <gl/freeglut.h>
#else
#include <GL/freeglut.h>
#endif
#include <glm/glm.hpp>
//#include <glut/glut.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "utils/io_utils.h"

//used by Subdivision
#include "base/model.h"
#include "utils/io_utils.h"
#include "subdivision/mesh.h"
#include "subdivision/catmull_solver.h"
#include "subdivision/loop_solver.h"
#include "utils/geometry_utils.h"
#include "subdivision/doo_solver.h"
#include <numeric>


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
					   const std::string& vertex_shader_path, const std::string& fragment_shader_path,  const CommonUtils::Config& config) 
	{
		screen_width_ = screen_width;
		screen_height_ =screen_height;
		vertex_shader_path_ = vertex_shader_path, fragment_shader_path_ = fragment_shader_path;
		b_show_wireframe_ = false;
		used_method = -1;
		view_level_ = -1;
		CallBackController::Instance().SetUp(0.0f, 0.0f, 5.0f);  //set up camera
		light_pos_ = glm::vec3(screen_width / 2.0, screen_height / 2.0, 1000.0f);
		b_initialization_window_ = true;
		config_ = config;
		if(InitializeContext()) {
			b_setup_ = true;
		}
		return b_setup_;
		
	}

	void Viewer::ResetContollingVariables() {
		CallBackController::Instance().SetUp(0.0f, 0.0f, 5.0f);  //set up camera
		view_level_ = 0;
		b_show_wireframe_ = false;
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
			//std::cerr << "Failed to create GLFW window_" << std::endl;
			glfwTerminate();
			return false;
		}
		glfwMakeContextCurrent(window_);


		glfwSetFramebufferSizeCallback(window_, CallBackController::FramebufferSizeCallback);
		glfwSetCursorPosCallback(window_, CallBackController::MouseMotionCallback);
		//glfwSetScrollCallback(window_, scroll_callback);
		//glfwSetMouseButtonCallback(window_, DefaultMotionFunc);

		// tell GLFW to capture our mouse
		glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		//glfwSetInputMode(window_, GLFW_STICKY_KEYS, GL_TRUE);
		// glad: load all OpenGL function pointers
		// ---------------------------------------

		if (glewInit() != GLEW_OK)
		{
			//std::cerr << "Failed to initialize GLEW" << std::endl;
			return false;
		}

    	if(!InitializeShader(shader_, vertex_shader_path_.c_str(), fragment_shader_path_.c_str(), "Vs", "Fs")) {
			//std::cerr << "Failed to initialize shader " << std::endl;
			return false;
		}

		//Initialize glut for rendering text
		int tmp_val = 1;
    	char *tmp_char = nullptr;
    	glutInit(&tmp_val, &tmp_char);

		glEnable(GL_DEPTH_TEST);
		return true;

	}

	/**
	* @brief Main Loop of OpenGL Rendering in a modern pipeline
	*
	* @param None
	*/
	void Viewer::Run() {

		if (!b_setup_) {
			//std::cerr << "Error: Please first invoke SetUp() function.\n";
			return;
		}

		// -----------------------------


		while (!glfwWindowShouldClose(window_))
		{
			//// per-frame time logic
			//// --------------------
			float currentframe = glfwGetTime();

			KeyBoardCallBack();

			
			// ------
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//	========================================================
			Display();
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

	void Viewer::DisplayInitializationWindow() {
		PutText(-0.9f, 0.85f, "Please enter a key to select subdivision method:");
		PutText(-0.9f, 0.78f, "Z: Loop subdivision");
		PutText(-0.9f, 0.71f, "X: Catmull-Clark subdivision");
		PutText(-0.9f, 0.64f, "C: Doo-Sabin subdivision");
		if(used_method >= 0) {
			if(used_method == 0) {
				method_name = "Loop subdivision";
			}else if(used_method == 1) {
				method_name = "Catmull-Clark subdivision";
			} else {
				method_name = "Doo-Sabin subdivision";

			}
			if(Subdivision()) {
				used_method = -1;
				ResetContollingVariables();
				b_initialization_window_ = false;
			} else {
				
			}
			
		}
			
	}

	void Viewer::Display() {

		if(b_initialization_window_) {
			DisplayInitializationWindow();
			return;
		}
		
		if(view_level_ == -1) {
			return;
		}
    	
		if(b_show_wireframe_) {
			RenderFixed(models_.at(view_level_));
		} else {
			Render(models_.at(view_level_));
		}
		PutText(-0.9f, 0.85f, "Key:");
    	PutText(-0.9f, 0.78f, "W: next level of subdivision");
    	PutText(-0.9f, 0.71f, "D: previous level of subdivision");
    	PutText(-0.9f, 0.64f, "F: switch between wireframe and mesh");
		PutText(-0.9f, 0.57f, method_name);

	}

	void Viewer::RenderFixed(const ModelAttrib& modelItem) {
		const auto& camera = CallBackController::Instance().GetCamera();

		glMatrixMode(GL_PROJECTION); //project matrix
		glLoadIdentity();
		//gluPerspective(glm::radians(camera.zoom_), (float)screen_width_ / (float)screen_height_, 0.1f, 10000.0f);
		glm::mat4 projection = glm::perspective(glm::radians(camera.zoom_), (float)screen_width_ / (float)screen_height_, 0.1f, 10000.0f);
		glMultMatrixf(glm::value_ptr(projection));

		if(verbose) {
			std::cout<<"/// Rendering Fixed() \n";
		}
		
		glMatrixMode(GL_MODELVIEW); //view matrix
		glPushMatrix();
		glLoadIdentity();
		const glm::mat4 view_matrix = camera.ViewMatrix();
		glMultMatrixf(glm::value_ptr(view_matrix));

		if(verbose) {
			Output("projection", projection);
			Output("view_matrix", view_matrix);
		}
		glBegin(GL_LINES);
		
		//for(auto& modelItem: models_) {
			if(verbose) {
				Output("color", modelItem.modelColor);
			}
			glColor3f(modelItem.modelColor.x, modelItem.modelColor.y, modelItem.modelColor.z); 
			const std::vector<float>& data = modelItem.edges_data;
			// data must satisfy (data.size() / 3) %2 == 0
			// for (data[j*3] . .) and (data[(j+1)*3] . .) producing  a line
			for(int j = 0 ; j < data.size() ; j+=3) {
				if(verbose) {
					std::cout<<"vertex ("<<j / 3<<")"<<data.at(j)<<", "<<data.at(j+1)<<", "<<data.at(j+2)<<"\n";
				}
				glVertex3f(data.at(j), data.at(j+1), data.at(j+2));
			}
		//}
		glEnd();
		glPopMatrix();

	}

	void Viewer::Render(ModelAttrib& modelItem){
		
		const auto& camera = CallBackController::Instance().GetCamera();
		glm::mat4 projection = glm::perspective(glm::radians(camera.zoom_), (float)screen_width_ / (float)screen_height_, 0.1f, 100.0f);
		//glm::mat4 view_matrix = camera.ViewMatrix(); //occasion 2
		glm::mat4 view_matrix = glm::mat4(1.0); //occasion 1
		glm::mat4 model_matrix = camera.ViewMatrix();
		//Comment(BCHO): camera.ViewMatrix() transform model from world to camera frame
		//				which denotes how we move model to camera frame
		//				If we use it as view matrix , camera moves in the scene
		//				but model is still that the normal of model is same all the time.
		//				So we will see the back(unlighted) of model opposite to the light
		//				But it is incompatible to our target that camera and light is still 
		//				and user always sees the lighted side of model. In the target,
		//				we should use the camera.ViewMatrix() as model matrix to modify 
		//				the normal of model all the time.
		//glm::vec3 eye_pos = camera.EyePos();
		glm::vec3 eye_pos = glm::vec3(-view_matrix[3][0], -view_matrix[3][1],
                                 -view_matrix[3][2]);  //?
		//Output("view_matrix", view_matrix);


		shader_.Bind();
		shader_.SetUniformMat4("projectionMatrix", projection);
		shader_.SetUniformMat4("viewMatrix", view_matrix);
		shader_.SetUniformVec3("eyePos", eye_pos);
		shader_.SetUniformVec3("lightPos", light_pos_);


		//{
			//for(auto& modelItem: models_) {
				shader_.SetUniformMat4("modelMatrix",model_matrix); //occasion 1
				//shader_.SetUniformMat4("modelMatrix",modelItem.second.modelMatrix); //occasion 2

				shader_.SetUniformVec3("modelColor",modelItem.modelColor);

				modelItem.model->Submit(modelItem.renderingMode);
			//}
		//}
		
		shader_.Unbind();
	}


	//reference code:
	void Viewer::PutText(float x, float y, std::string str) {
		glMatrixMode( GL_PROJECTION ) ;
		glPushMatrix() ; // save
		glLoadIdentity();// and clear
		glMatrixMode( GL_MODELVIEW ) ;
		glPushMatrix() ;
		glLoadIdentity() ;

		//glDisable(GL_LIGHTING);
		glDisable(GL_DEPTH_TEST);
		glColor3d(1.0, 1.0, 1.0);
		glRasterPos2d(x, y);
		for (int n = 0; n < str.size(); ++n) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str[n]);
		}
		//glEnable(GL_LIGHTING);
		glEnable(GL_DEPTH_TEST);
		glMatrixMode( GL_PROJECTION ) ;
		glPopMatrix() ; // revert back to the matrix I had before.
		glMatrixMode( GL_MODELVIEW ) ;
		glPopMatrix() ;
	}
	

	ModelAttrib* Viewer::CreateMeshPNC(const float* pData, const int stride, const int num_vertex, const glm::vec3& modelColor,
										const glm::mat4& modelMatrix) {  
		
		if(verbose) {
			std::cout<<"pData:\n";
			std::cout<<"("<<stride<<", "<<num_vertex<<")\n";
			for(int i = 0 ; i < num_vertex ; i++) {
				int index = i * stride;
				std::cout<<"("<<i<<") ";
				for(int j = 0 ; j < 9 ;j++) {
					std::cout<<pData[index + j]<<" ";
				}
				std::cout<<"indices : "<< index + 8 <<"\n";
				std::cout<<"\n";
			}
		}

		const int num_indices = num_vertex;
		unsigned int* indices = new unsigned int[num_indices];
		for(int i = 0 ; i < num_indices ; i++) {
			indices[i] = i;
		}

		const int idx = models_.size();
		models_[idx].model = CreateGeometryPNC(pData, stride, num_vertex,indices, num_indices);
		models_[idx].modelMatrix = modelMatrix;
		models_[idx].modelColor = modelColor;
		models_[idx].renderingMode = GL_TRIANGLES;


		//preserve data for drawing wireframe
		
		models_[idx].edges_data.reserve(3 * num_vertex);
		for(int i = 0 ; i < num_vertex ; i++) {
			int edge_id1 = i * stride;
			int edge_id2 = (i % 3 == 2 ? ((i - 2) * stride) : ((i + 1) * stride));
			models_[idx].edges_data.emplace_back(pData[edge_id1]);
			models_[idx].edges_data.emplace_back(pData[edge_id1 + 1]);
			models_[idx].edges_data.emplace_back(pData[edge_id1 + 2]);

			models_[idx].edges_data.emplace_back(pData[edge_id2]);
			models_[idx].edges_data.emplace_back(pData[edge_id2 + 1]);
			models_[idx].edges_data.emplace_back(pData[edge_id2 + 2]);
		}

		if(verbose) {
			std::cout<<"Points:\n";
			for(int i = 0 ; i < models_[idx].edges_data.size() ; i+=3) {
				std::cout<<"("<< i / 3<<") ";
				std::cout<<models_[idx].edges_data.at(i)<<" ";
				std::cout<<models_[idx].edges_data.at(i + 1)<<" ";
				std::cout<<models_[idx].edges_data.at(i + 2)<<" ";
				std::cout<<"\n";
			}
		}

		delete[] indices;

		if(view_level_ == -1) {
			view_level_ = 0;
		}

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

	bool Viewer::Subdivision() {

		size_t num_levels = config_.maximum_level;
		std::vector<Eigen::Vector3d> vertices;
		std::vector<std::vector<index_t>> polygons;
		bool b_split_triangle = false;
		if(used_method == 0) {
			b_split_triangle = true; // split input mesh into triangles for loop subdivison 
		}
		if(!CommonUtils::LoadObj(config_.model_path, vertices, polygons,b_split_triangle)) {
			return false;
		}
		std::vector<SubDivision::Mesh> meshes;
		meshes.reserve(num_levels);
		meshes.emplace_back(SubDivision::Mesh());
		meshes[0].SetUp(vertices, polygons);
		meshes[0].ScaleModel(3.0);
		
		//meshes[0].PrintObj();
		//meshes[0]->PrintPolygon();
		SubDivision::SubDivisionSolver* division_solver;
		SubDivision::CatmullClarkSolver clark_division_solver;
		SubDivision::LoopSolver loop_division_solver;
		SubDivision::DooSabinSolver doo_division_solver;
		if(used_method == 0) {
			std::cout<<"Start Loop division\n";
			division_solver = &loop_division_solver;
		} else if(used_method == 1) {
			std::cout<<"Start CatmullClark division\n";
			division_solver = &clark_division_solver;
		}else {
			std::cout<<"Start DooSabin division\n";
			division_solver = &doo_division_solver;
		}
		

		for(int i = 1 ; i < num_levels ; i++) {
			std::cout<<"/////level "<<i<<"\n";
			SubDivision::Mesh updated_mesh;
			if(!division_solver->Run((meshes[i-1]), (updated_mesh))) {
				break;
			}
			meshes.emplace_back(updated_mesh);
		}
		num_levels = meshes.size();
		std::cout<<"max level : "<<num_levels<<"\n";

		for(int i = 0 ; i < num_levels ; i++) {
            std::vector<float> pData;
            size_t num_vertex;
            std::tie(pData, num_vertex) = meshes[i].ConvertToTriangularMesh();
            std::cout<<i<<" pData->size(): "<<pData.size()<<"\n";
            CreateMeshPNC(pData.data(), 9, num_vertex, glm::vec3(0.4, 0.4, 0.0) ,glm::mat4(1.0));
        }
		return true;

	}

	/**
	* @brief Implementation of keyboard clicking callback function
	*
	* @param[in] window_
	*/
	void Viewer::KeyBoardCallBack()
	{
		if (glfwGetKey(window_, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window_, true);

		if(b_initialization_window_) {
			if (glfwGetKey(window_, GLFW_KEY_Z) == GLFW_PRESS) {
				used_method = 0;
				return;
			}

			if (glfwGetKey(window_, GLFW_KEY_X) == GLFW_PRESS) {
				used_method = 1;
				return;
			}

			if (glfwGetKey(window_, GLFW_KEY_C) == GLFW_PRESS) {
				used_method = 2;
				return;
			}
		} else {
			if (glfwGetKey(window_, GLFW_KEY_F) == GLFW_PRESS)
			b_show_wireframe_ = !b_show_wireframe_;

			if (glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS) {
				if(models_.count(view_level_ + 1)) {
					view_level_ += 1;
				}
			}

			if (glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS) {
				if(models_.count(view_level_ - 1)) {
					view_level_ -= 1;
				}
			}
		}
			
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

}