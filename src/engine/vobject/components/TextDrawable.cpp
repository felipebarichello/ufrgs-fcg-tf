// TextDrawable implementation using TextRendering.cpp helpers
#include <engine>
#include "TextDrawable.hpp"
#include <GLFW/glfw3.h>

namespace engine {
	TextDrawable::TextDrawable() {
		static bool text_rendering_initialized = false;
		if (!text_rendering_initialized) {
			engine::init_text_renderer();
			text_rendering_initialized = true;
		}
		this->window = engine::EngineController::get_instance()->get_window();
	}

	TextDrawable::~TextDrawable(){
	}

	void TextDrawable::setText(const std::string& text, float scale, float x, float y) {
		this->text = text;
		this->scale = scale;
		this->color = color;
		this->x = x;
		this->y = y;
	}

	void TextDrawable::draw() {
		render_text(window, this->text, this->x, this->y, this->scale);
	}

	float TextDrawable::get_text_width() const {
		return set_text_char_width(window);
	}

	float TextDrawable::get_text_height() const {
		return set_text_line_height(window);
	}
}
