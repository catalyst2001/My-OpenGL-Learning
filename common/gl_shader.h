/*
 Author: Deryabin K.
 Date: 19.09.2023
 File: gl_shader.h
 Purpose: common functions for working with GLSL shaders
*/

#pragma once
#include <stddef.h> //NULL
#include "glad.h"

struct gl_err_buf_s {
	char *p_buffer;
	size_t maxlen;
};

#define GL_CNT(a) (sizeof(a) / sizeof(a[0]))

/* GL SHADER OBJECT */
enum GL_SHADER_OBJECT_STATUS {
	GL_SHADER_OBJECT_STATUS_OK = 0,
	GL_SHADER_OBJECT_STATUS_COMPILE_ERROR,
	GL_SHADER_OBJECT_STATUS_CREATE_ERROR
};

const char *gl_shader_object_status_to_string(GL_SHADER_OBJECT_STATUS status);

GL_SHADER_OBJECT_STATUS gl_shader_object_compile(GLuint *p_dst_object, char *p_dsterr, size_t maxlen, GLenum shader_type, const char *p_text);
GL_SHADER_OBJECT_STATUS gl_shader_object_delete(GLuint *src_object);

/* GL PROGRAM OBJECT */
enum GL_SHADER_PROGRAM_STATUS {
	GL_SHADER_PROGRAM_STATUS_OK = 0,
	GL_SHADER_PROGRAM_STATUS_CREATE_ERROR,
	GL_SHADER_PROGRAM_STATUS_LINK_ERROR
};

const char *gl_shader_program_status_to_string(GL_SHADER_PROGRAM_STATUS status);

struct program_attrib_binding_s {
	GLuint binding;
	const char *p_attrib_name;
};

GL_SHADER_PROGRAM_STATUS gl_shader_program_create_and_link(GLuint *p_dst_program_object,
	char *p_dst_err, size_t maxlen,
	const GLuint *p_objects, GLuint num_objects,
	const program_attrib_binding_s *p_bindings, GLuint num_bindings);

GL_SHADER_PROGRAM_STATUS gl_shader_program_validate(GLuint src_object, gl_err_buf_s *p_dst_err);
GL_SHADER_PROGRAM_STATUS gl_shader_program_delete(GLuint src_object);

/* for shader cache */
GL_SHADER_PROGRAM_STATUS gl_shader_program_get_binary_size(GLsizeiptr *p_dst_size, GLuint src_program);
GL_SHADER_PROGRAM_STATUS gl_shader_program_get_binary_data(GLubyte *p_dst, GLuint src_program);
GL_SHADER_PROGRAM_STATUS gl_shader_program_load_from_binary();

/* high-level functions */

struct program_sources_s {
	const char *p_vert_src;
	const char *p_tess_ctl_src;
	const char *p_tess_eval_src;
	const char *p_geom_src;
	const char *p_frag_src;
};

#define GL_PROGRAM_SOURCES_PTR_INIT(ptr) \
	(ptr)->p_vert_src = NULL;\
	(ptr)->p_tess_ctl_src = NULL;\
	(ptr)->p_tess_eval_src = NULL;\
	(ptr)->p_geom_src = NULL;\
	(ptr)->p_frag_src = NULL;

enum GL_SHADER_INDICES {
	VERTEX_SHADER = 0,
	//TESSELATION_CONTROL_SHADER,
	//TESSELATION_EVAL_SHADER,
	//GEOMETRY_SHADER,
	FRAGMENT_SHADER,

	MAX_SHADERS
};

GL_SHADER_PROGRAM_STATUS gl_shader_link_program_from_sources(GLuint *p_dst_program_object,
	gl_err_buf_s *p_dst_err, GL_SHADER_OBJECT_STATUS *p_err_shader_status, GL_SHADER_INDICES *p_dst_err_shader_idx,
	const program_sources_s *p_source_ptrs);