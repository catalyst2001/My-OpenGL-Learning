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

GLuint def_program;

char buffer[1024];
gl_err_buf_s err_buf = { buffer, sizeof(buffer) };

GLuint make_default_program()
{
	static const char *p_vtx = {
		"#version 330 core\n"
		""
		""
		""
		""
		""
		""
	};

	static const char *p_frag = {
		"#version 330 core\n"
		""
		""
		""
		""
		""
		""
	};

	GLuint program;
	char buffer[1024];
	union {
		struct { GLuint vtx_obj, frag_obj; };
		GLuint objects[2];
	};

	GL_SHADER_OBJECT_STATUS shader_status;
	GL_SHADER_PROGRAM_STATUS prog_status;

	/* vertex shader */
	shader_status = gl_shader_object_compile(&vtx_obj, buffer, sizeof(buffer), GL_VERTEX_SHADER, p_vtx);
	if (GL_SHADER_OBJECT_STATUS_OK != shader_status) {
		printf("GL_VERTEX_SHADER compilation failed with status (%s). Error:\n", gl_shader_object_status_to_string(shader_status));
		printf(buffer);
		return 0;
	}

	/* fragment shader */
	shader_status = gl_shader_object_compile(&frag_obj, buffer, sizeof(buffer), GL_FRAGMENT_SHADER, p_frag);
	if (GL_SHADER_OBJECT_STATUS_OK != shader_status) {
		printf("GL_FRAGMENT_SHADER compilation failed with status (%s). Error:\n", gl_shader_object_status_to_string(shader_status));
		printf(buffer);
		return 0;
	}

	/* create program */
	const static program_attrib_binding_s attrib_bindings[] = {
		{ 0, "vertex" },
		{ 1, "normal" },
		{ 2, "texcoord" }
	};

	prog_status = gl_shader_program_create_and_link(&program, buffer, sizeof(buffer), objects, GL_CNT(objects), attrib_bindings, GL_CNT(attrib_bindings));
	if (GL_SHADER_PROGRAM_STATUS_OK != prog_status) {

	}
}

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
			"last_shader_index: %d\n"
			"last_shader_status: %d\n"
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