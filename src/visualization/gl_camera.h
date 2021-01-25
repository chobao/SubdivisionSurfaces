#pragma once

//#define GLEW_STATIC
//#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include "utils/common_utils.h"

namespace GLRendering {

	const float ZOOM = 45.0f;

	enum MouseButton {
		LEFT_BUTTON,
		MIDDLE_BUTTON,
		RIGHT_BUTTON
	};

	class TurnTableCamera {
	public:
		TurnTableCamera(float eye_x = 0.0f, float eye_y = 0.0f, float eye_z = 0.0f): 
			eye_x_(eye_x_), eye_y_(eye_y), eye_z_(eye_z){
			spin_x_ = 0.0f, spin_y_ = 0.0f;
			zoom_ = ZOOM;
			sensitivity_ = 1000;
		}

		void ProcessMouseScroll(float yoffset) {
			if (zoom_ >= 1.0f && zoom_ <= 45.0f)
				zoom_ -= yoffset;
			if (zoom_ <= 1.0f)
				zoom_ = 1.0f;
			if (zoom_ >= 45.0f)
				zoom_ = 45.0f;
		}
		void ProcessMouseButton(MouseButton press, float x, float y) {
			if (press == LEFT_BUTTON) {
				spin_x_ += float(x - old_x_);
				spin_y_ += float(y - old_y_);
			}
			else if (press == MIDDLE_BUTTON) {
				eye_x_ -= float(x - old_x_) / (3 * sensitivity_);
				eye_y_ += float(y - old_y_) / (3 * sensitivity_);
			}
			else if (press == RIGHT_BUTTON) {
				eye_z_ += float(y - old_y_) / (0.1 * sensitivity_);
			}
		}

		void UpdateOldMousePos(float x, float y) {
			old_x_ = x;
			old_y_ = y;
		}

		glm::mat4 ViewMatrix() const{
			glm::mat4 view_matrix = glm::lookAt(glm::vec3(eye_x_, eye_y_, eye_z_), glm::vec3(eye_x_, eye_y_, eye_z_ - 1), glm::vec3(0, 1, 0));
			view_matrix = glm::rotate(view_matrix, static_cast<float>(spin_y_ * M_PI / 180.0), glm::vec3(1,0,0));
			view_matrix = glm::rotate(view_matrix, static_cast<float>(spin_x_ * M_PI / 180.0), glm::vec3(0,1,0));
			return view_matrix;
			// 	turn table
			// // glRotatef(spin_y_, 1, 0, 0);
			// // glRotatef(spin_x_, 0, 1, 0);
		}

		glm::vec3 EyePos() const {
			return glm::vec3(eye_x_, eye_y_, eye_z_);
		}


		float eye_x_, eye_y_, eye_z_;
		float spin_x_, spin_y_;
		float old_x_, old_y_;
		float zoom_;
		float sensitivity_;
	};
}