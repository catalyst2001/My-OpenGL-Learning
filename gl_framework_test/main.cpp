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
GLuint def_vao;

char buffer[1024];
gl_err_buf_s err_buf = { buffer, sizeof(buffer) };

GLuint make_default_program()
{
	static const char *p_vtx = {
		"#version 330 core\n"
		""
		"in vec3 vertex;"
		"in vec3 normal;"
		"in vec2 texcoord;"

		"out vec2 frag_texcoord;"

		"uniform mat4 MVP;"

		"out vec4 world_position;"

		"void main()"
		"{"
			""
			""
			"world_position = MVP * vec4(vertex, 1.0);"
		"}"
	};

	static const char *p_frag = {
		"#version 330 core\n"

		"in vec2 frag_texcoord;"

		"uniform sampler2D diffuse_sampler;"

		"out vec4 out_color;"
		
		"void main()"
		"{"
			""
			"out_color = texture(diffuse_sampler, frag_texcoord);"
		"}"
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
		printf("gl_shader_program_create_and_link failed with status (%s). Error:\n", gl_shader_program_status_to_string(prog_status));
		printf(buffer);
		return 0;
	}

	printf("Program created!\n");
	return program;
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

	def_program = make_default_program();
	if (!def_program) {
		printf("Program creating failed!\n");
		return 1;
	}

	glGenVertexArrays(1, &def_vao);
	glBindVertexArray(def_vao);


	glEnable(GL_DEPTH_TEST);
	while (window.is_window_alive()) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);






		window.swap_buffers();
		window.poll_events();
	}
	window.shutdown();
	return 0;
}