#pragma once
#include <windows.h>
#include <stdio.h> //vsprintf_s
#include <assert.h>
#include "glad.h"
#include "wglext.h"

#pragma comment(lib, "opengl32.lib")

#define WINDOW_ID "gl_framework_window_class"

class gl_window; //declared bottom

/* window callbacks */
class iwindow_callbacks
{
public:
	virtual void on_mouse_move(gl_window *p_window, int x, int y, int button, int state) = 0;
	virtual void on_mouse_click(gl_window *p_window, int x, int y, int button, int state) = 0;
	virtual void on_key_action(gl_window *p_window, int keycode, int action) = 0;
	virtual void on_window_resize(gl_window *p_window, int width, int height) = 0;
	virtual void on_window_disactivate(gl_window *p_window, bool b_activated) = 0;
};

/* copy this class to main code */
class window_callbacks_base : public iwindow_callbacks {
protected:
	virtual void on_mouse_move(gl_window *p_window, int x, int y, int button, int state) {}
	virtual void on_mouse_click(gl_window *p_window, int x, int y, int button, int state) {}
	virtual void on_key_action(gl_window *p_window, int keycode, int action) {}
	virtual void on_window_resize(gl_window *p_window, int width, int height) {}
	virtual void on_window_disactivate(gl_window *p_window, bool b_activated) {}
};

/* gl window class */
class gl_window
{
	HWND h_window;
	HDC h_devctx;
	HGLRC h_glctx;
	iwindow_callbacks *p_cbs;
	bool b_window_active;
	char last_error[512];
	PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB;

	/* context attribs fill helper */
	class context_attribs_fill_helper {
		int curr_num_attribs;
		struct context_attrib_s {
			int id;
			int value;
		} attribs[16];
	public:
		context_attribs_fill_helper() : curr_num_attribs(0) {}
		~context_attribs_fill_helper() {}

		void push_attrib(int id, int value) {
			attribs[curr_num_attribs].id = id;
			attribs[curr_num_attribs].value = value;
			curr_num_attribs++;
			assert(curr_num_attribs < (sizeof(attribs) / sizeof(attribs[0])));
		}

		void finalize_attribs() {
			push_attrib(0, 0);
		}

		int get_num_attribs() { return curr_num_attribs; }
		int *get_attribs()    { return (int *)attribs; }
	};

	/* errors */
	void errorf(const char *p_format, ...);

	static LRESULT CALLBACK window_proc(HWND, UINT, WPARAM, LPARAM);
public:
	gl_window(iwindow_callbacks *p_callbacks);
	~gl_window();

	bool init(const char *p_title, int width, int height, bool b_multisampled = true, int num_samples = 2);
	const char *get_last_error_string();

	void show_cursor(bool b_show);
	bool is_cusor_visible();

	bool is_window_alive();
	void make_context_current();
	void swap_buffers();
	void poll_events();
	void close(int code);
	bool extension_avalible(const char *p_extname);

	bool shutdown();
};

