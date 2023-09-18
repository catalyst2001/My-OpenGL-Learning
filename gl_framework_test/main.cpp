#include <stdio.h>

#include "../common/gl_window.h"
#include "../common/gl_shader.h"

class window_callbacks : public window_callbacks_base {
public:
	virtual void on_mouse_move(gl_window *p_window, int x, int y, int button, int state) {
	}

	virtual void on_mouse_click(gl_window *p_window, int x, int y, int button, int state) {
	}

	virtual void on_key_action(gl_window *p_window, int keycode, int action) {
	}
} callbacks;

gl_window window(&callbacks);

static const char *p_vtx = {
	""
	""
	""
	""
	""
	""
	""
};

static const char *p_frag = {
	""
	""
	""
	""
	""
	""
	""
};

GLuint def_program;

char buffer[1024];
gl_err_buf_s err_buf = { buffer, sizeof(buffer) };


int main()
{
	if (!window.init("gl frameworks test", 800, 600)) {
		printf("Error: '%s'\n", window.get_last_error_string());
		return 1;
	}

	if (!gladLoadGL()) {
		printf("failed to load gl extensions\n");
		window.shutdown();
		return 1;
	}


	/* init default shaders */
	GL_SHADER_INDICES last_shader_index;
	GL_SHADER_OBJECT_STATUS last_shader_status;
	program_sources_s sources;
	GL_PROGRAM_SOURCES_PTR_INIT(&sources);
	sources.p_vert_src = p_vtx;
	sources.p_frag_src = p_frag;
	if (gl_shader_link_program_from_sources(&def_program, &err_buf, &last_shader_status, &last_shader_index, &sources) != GL_SHADER_PROGRAM_STATUS_OK) {
		printf(
			"\n\n--- gl_shader_link_program_from_sources failed ---\n"
			"Error: %s\n"
			"last_shader_index: %d"
			"last_shader_status: %d"
			""
			""

		);
	}


	glEnable(GL_DEPTH_TEST);
	while (window.is_window_alive()) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);






		window.swap_buffers();
		window.poll_events();
	}
	window.shutdown();
	return 0;
}