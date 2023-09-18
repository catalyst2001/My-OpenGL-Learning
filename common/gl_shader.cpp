#include "gl_shader.h"

#define SWITCH_MACRO_BEGIN(i) switch(i) { 
#define CASE_MACRO(_const) case _const: return #_const; 
#define SWITCH_MACRO_END() \
	default: \
		return "INVALID_PARAMETER!!!";\
	}

const char *gl_shader_object_status_to_string(GL_SHADER_OBJECT_STATUS status)
{
	SWITCH_MACRO_BEGIN(status)
	CASE_MACRO(GL_SHADER_OBJECT_STATUS_OK)
	CASE_MACRO(GL_SHADER_OBJECT_STATUS_COMPILE_ERROR)
	CASE_MACRO(GL_SHADER_OBJECT_STATUS_CREATE_ERROR)
	SWITCH_MACRO_END()
}

GL_SHADER_OBJECT_STATUS gl_shader_object_compile(GLuint *p_dst_object, gl_err_buf_s *p_dst_err, GLenum shader_type, const char *p_text)
{
	GLint status;
	GLuint shader;

	/* create shader object */
	*p_dst_object = 0;
	shader = glCreateShader(shader_type);
	if (!shader)
		return GL_SHADER_OBJECT_STATUS_CREATE_ERROR;

	/* set shader soruce */
	glShaderSource(shader, 1, &p_text, NULL);
	glCompileShader(shader);

	/* get compilation status */
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE) {
		glGetShaderInfoLog(shader, (GLsizei)p_dst_err->maxlen, NULL, p_dst_err->p_buffer);
		return GL_SHADER_OBJECT_STATUS_COMPILE_ERROR;
	}

	*p_dst_object = shader;
	return GL_SHADER_OBJECT_STATUS_OK;
}

GL_SHADER_OBJECT_STATUS gl_shader_object_delete(GLuint *src_object)
{
	glDeleteShader(*src_object);
	*src_object = 0;
	return GL_SHADER_OBJECT_STATUS_OK;
}

const char *gl_shader_program_status_to_string(GL_SHADER_PROGRAM_STATUS status)
{
	SWITCH_MACRO_BEGIN(status)
	CASE_MACRO(GL_SHADER_PROGRAM_STATUS_OK)
	CASE_MACRO(GL_SHADER_PROGRAM_STATUS_CREATE_ERROR)
	CASE_MACRO(GL_SHADER_PROGRAM_STATUS_LINK_ERROR)
	SWITCH_MACRO_END()
}

GL_SHADER_PROGRAM_STATUS gl_shader_program_create_and_link(GLuint *p_dst_program_object, gl_err_buf_s *p_dst_err,
	const GLuint *p_objects, GLuint num_objects,
	const program_attrib_binding_s *p_bindings, GLuint num_bindings)
{
	GLuint i;
	GLuint program;
	GLint  link_status;

	/* create program */
	*p_dst_program_object = 0;
	program = glCreateProgram();
	if (!program)
		return GL_SHADER_PROGRAM_STATUS_CREATE_ERROR;

	/* set bindings */
	for (i = 0; i < num_bindings; i++)
		glBindAttribLocation(program, p_bindings[i].binding, p_bindings[i].p_attrib_name);

	/* attach shaders objects */
	for (i = 0; i < num_objects; i++)
		glAttachShader(program, p_objects[i]);

	glLinkProgram(program);
	
	glGetProgramiv(program, GL_LINK_STATUS, &link_status);
	if (link_status != GL_TRUE) {
		glGetProgramInfoLog(program, (GLsizei)p_dst_err->maxlen, NULL, p_dst_err->p_buffer);
		return GL_SHADER_PROGRAM_STATUS_LINK_ERROR;
	}
	*p_dst_program_object = program;
	return GL_SHADER_PROGRAM_STATUS_OK;
}

GL_SHADER_PROGRAM_STATUS gl_shader_program_validate(GLuint src_object, char *p_dst_err, size_t dst_maxlen)
{
	return GL_SHADER_PROGRAM_STATUS_OK;
}

GL_SHADER_PROGRAM_STATUS gl_shader_program_delete(GLuint src_object)
{
	return GL_SHADER_PROGRAM_STATUS_OK;
}

GL_SHADER_PROGRAM_STATUS gl_shader_link_program_from_sources(GLuint *p_dst_program_object,
	gl_err_buf_s *p_dst_err, GL_SHADER_OBJECT_STATUS *p_err_shader_status, GL_SHADER_INDICES *p_dst_err_shader_idx,
	const program_sources_s *p_source_ptrs)
{
	GLuint shaders[MAX_SHADERS];
	GL_SHADER_OBJECT_STATUS shader_status;

	*p_dst_err_shader_idx = MAX_SHADERS;
	*p_err_shader_status = GL_SHADER_OBJECT_STATUS_OK;

#define SHADER_PROGRAM_CHECK_SHADER_COMPILE_STATUS(idx) \
	if (shader_status != GL_SHADER_OBJECT_STATUS_OK) {\
		*p_err_shader_status = shader_status; \
		*p_dst_err_shader_idx = idx; \
		return GL_SHADER_PROGRAM_STATUS_LINK_ERROR; \
	}

	/* VERTEX_SHADER */
	if (p_source_ptrs->p_vert_src) {
		shader_status = gl_shader_object_compile(&shaders[VERTEX_SHADER], p_dst_err, GL_VERTEX_SHADER, p_source_ptrs->p_vert_src);
		SHADER_PROGRAM_CHECK_SHADER_COMPILE_STATUS(VERTEX_SHADER);
	}

	///* TESSELATION_CONTROL_SHADER */
	//if (p_source_ptrs->p_vert_src) {
	//	shader_status = gl_shader_object_compile(&shaders[TESSELATION_CONTROL_SHADER], p_dst_err, GL_VERTEX_SHADER, p_source_ptrs->p_vert_src);
	//	SHADER_PROGRAM_CHECK_SHADER_COMPILE_STATUS(TESSELATION_CONTROL_SHADER);
	//}

	///* TESSELATION_EVAL_SHADER */
	//if (p_source_ptrs->p_vert_src) {
	//	shader_status = gl_shader_object_compile(&shaders[VERTEX_SHADER], p_dst_err, GL_VERTEX_SHADER, p_source_ptrs->p_vert_src);
	//	SHADER_PROGRAM_CHECK_SHADER_COMPILE_STATUS(VERTEX_SHADER);
	//}

	///* GEOMETRY_SHADER */
	//if (p_source_ptrs->p_vert_src) {
	//	shader_status = gl_shader_object_compile(&shaders[VERTEX_SHADER], p_dst_err, GL_VERTEX_SHADER, p_source_ptrs->p_vert_src);
	//	SHADER_PROGRAM_CHECK_SHADER_COMPILE_STATUS(VERTEX_SHADER);
	//}

	/* FRAGMENT_SHADER */
	if (p_source_ptrs->p_frag_src) {
		shader_status = gl_shader_object_compile(&shaders[FRAGMENT_SHADER], p_dst_err, GL_FRAGMENT_SHADER, p_source_ptrs->p_frag_src);
		SHADER_PROGRAM_CHECK_SHADER_COMPILE_STATUS(FRAGMENT_SHADER);
	}
#undef SHADER_PROGRAM_CHECK_SHADER_COMPILE_STATUS
	return GL_SHADER_PROGRAM_STATUS_OK;
}
