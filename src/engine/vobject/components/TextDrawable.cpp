// TextDrawable implementation using TextRendering.cpp helpers
#include <engine>
#include "TextDrawable.hpp"
#include <GLFW/glfw3.h>

namespace engine {
	TextDrawable::TextDrawable()
	{
		static bool text_rendering_initialized = false;
		if (!text_rendering_initialized) {
			engine::init_text_renderer();
			text_rendering_initialized = true;
		}
	}

	TextDrawable::~TextDrawable()
	{
	}

	void TextDrawable::setText(const std::string& text, float scale, engine::Vec3 color, float x, float y)
	{
		this->text = text;
		this->scale = scale;
		this->color = color;
		this->x = x;
		this->y = y;
	}

	void TextDrawable::draw()
	{
		engine::EngineController* ec = engine::EngineController::get_instance();
		if (!ec) return;
		GLFWwindow* window = ec->get_window();
		if (!window) return;

		// Currently the text shader in TextRendering.cpp uses a fixed color
		render_text(window, this->text, this->x, this->y, this->scale);
	}
}
