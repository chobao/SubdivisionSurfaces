#pragma once

//#define GLEW_STATIC
//#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

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
				eye_x_ -= float(x - old_x_) / 30;
				eye_y_ += float(y - old_y_) / 30;
			}
			else if (press == RIGHT_BUTTON) {
				eye_z_ += float(y - old_y_) / 10;
			}
		}

		void UpdateOldMousePos(float x, float y) {
			old_x_ = x;
			old_y_ = y;
		}


		float eye_x_, eye_y_, eye_z_;
		float spin_x_, spin_y_;
		float old_x_, old_y_;
		float zoom_;
	};
}