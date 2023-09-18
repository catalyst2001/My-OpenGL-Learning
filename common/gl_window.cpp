#include "gl_window.h"
#include <string.h>

void gl_window::errorf(const char *p_format, ...)
{
	va_list argptr;
	va_start(argptr, p_format);
	vsprintf_s(last_error, sizeof(last_error), p_format, argptr);
	va_end(argptr);
}

LRESULT gl_window::window_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	gl_window *p_this;

	/* handle window creation */
	if (WM_CREATE == msg) {
		p_this = (gl_window *)((CREATESTRUCTA *)lParam)->lpCreateParams;
		assert(p_this);
		SetWindowLongPtr(hWnd, 0, (LONG_PTR)p_this);
		return 0;
	}

	/* handle other messages */
	p_this = (gl_window *)GetWindowLongPtr(hWnd, 0);
	switch (msg) {
	case WM_SIZE: {
		GetClientRect(hWnd, &rect);
		p_this->p_cbs->on_window_resize(p_this, rect.right, rect.bottom);
		break;
	}

	case WM_CLOSE: {
		PostQuitMessage(0);
		return 0;
	}

	default:
		return DefWindowProcA(hWnd, msg, wParam, lParam);
	}
	return 0;
}

gl_window::gl_window(iwindow_callbacks *p_callbacks)
{
	h_window = NULL;
	h_devctx = NULL;
	h_glctx = NULL;
	p_cbs = p_callbacks;
	b_window_active = false;
}

gl_window::~gl_window()
{
}

bool gl_window::init(const char *p_title, int width, int height, bool b_multisampled, int num_samples)
{
	DWORD error;
	int xpos, ypos;
	int pixelformat;
	HGLRC h_newctx;
	WNDCLASSEXA wcex;
	context_attribs_fill_helper attribs_builder;

	memset(&wcex, 0, sizeof(wcex));
	wcex.cbSize = sizeof(wcex);
	wcex.cbWndExtra = sizeof(gl_window *);
	wcex.style = CS_OWNDC;
	wcex.hbrBackground = NULL;
	wcex.hCursor = LoadCursorW(NULL, MAKEINTRESOURCEW(IDC_ARROW));
	wcex.hInstance = GetModuleHandleW(NULL);
	wcex.lpszClassName = WINDOW_ID;
	wcex.lpfnWndProc = gl_window::window_proc;
	if (!RegisterClassExA(&wcex)) {
		error = GetLastError();
		errorf("failed to create window class! LastError() = %d (0x%x)", error, error);
		return false;
	}

	xpos = (GetSystemMetrics(SM_CXSCREEN) - width) >> 1;
	ypos = (GetSystemMetrics(SM_CYSCREEN) - height) >> 1;
	h_window = CreateWindowExA(0, WINDOW_ID, p_title, WS_OVERLAPPEDWINDOW ^ (WS_THICKFRAME | WS_MAXIMIZEBOX), xpos, ypos, width, height, HWND_DESKTOP, (HMENU)NULL, NULL, this);
	if (!h_window) {
		error = GetLastError();
		errorf("failed to create window! LastError() = %d (0x%x)", error, error);
		return false;
	}

	ShowWindow(h_window, SW_SHOW);
	UpdateWindow(h_window);

	/* get window device context */
	h_devctx = GetDC(h_window);
	if (!h_devctx) {
		error = GetLastError();
		errorf("GetDC failed! LastError() = %d (0x%x)", error, error);
		return false;
	}

	/* set pixel format */
	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),   // size of this pfd  
		1,                     // version number  
		PFD_DRAW_TO_WINDOW |   // support window  
		PFD_SUPPORT_OPENGL |   // support OpenGL  
		PFD_DOUBLEBUFFER,      // double buffered  
		PFD_TYPE_RGBA,         // RGBA type  
		32,                    // 24-bit color depth  
		0, 0, 0, 0, 0, 0,      // color bits ignored  
		0,                     // no alpha buffer  
		0,                     // shift bit ignored  
		0,                     // no accumulation buffer  
		0, 0, 0, 0,            // accum bits ignored  
		32,                    // 32-bit z-buffer  
		0,                     // no stencil buffer  
		0,                     // no auxiliary buffer  
		PFD_MAIN_PLANE,        // main layer  
		0,                     // reserved  
		0, 0, 0                // layer masks ignored  
	};

	pixelformat = ChoosePixelFormat(h_devctx, &pfd);
	if (!pixelformat) {
		error = GetLastError();
		errorf("ChoosePixelFormat failed! LastError() = %d (0x%x)", error, error);
		return false;
	}

	if (!SetPixelFormat(h_devctx, pixelformat, &pfd)) {
		error = GetLastError();
		errorf("SetPixelFormat failed! LastError() = %d (0x%x)", error, error);
		return false;
	}

	/* creating standard GL context */
	h_glctx = wglCreateContext(h_devctx);
	if (!h_glctx) {
		error = GetLastError();
		errorf("wglCreateContext failed! LastError() = %d (0x%x)", error, error);
		return false;
	}

	if (!wglMakeCurrent(h_devctx, h_glctx)) {
		error = GetLastError();
		errorf("wglMakeCurrent failed! LastError() = %d (0x%x)", error, error);
		return false;
	}

	/* get proc wglCreateContextAttribsARB */
	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
	wglGetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC)wglGetProcAddress("wglGetExtensionsStringARB");
	
	if (!wglCreateContextAttribsARB) {
		error = GetLastError();
		errorf("EXT: WGL_ARB_create_context not supported by your driver. LastError() = %d (0x%x)", error, error);
		return false;
	}

	/* crreate extended OpenGL context */
	
	attribs_builder.push_attrib(WGL_CONTEXT_MINOR_VERSION_ARB, 3);
	attribs_builder.push_attrib(WGL_CONTEXT_MAJOR_VERSION_ARB, 3);
	attribs_builder.push_attrib(WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB);
	attribs_builder.push_attrib(WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB);

	//if (b_multisampled && extension_avalible("WGL_ARB_multisample")) {
	//	/* work with WGL_ARB_multisample ext */
	//	attribs_builder.push_attrib(WGL_SAMPLE_BUFFERS_ARB, 1);
	//	attribs_builder.push_attrib(WGL_SAMPLES_ARB, 4);
	//}
	attribs_builder.finalize_attribs();

	h_newctx = wglCreateContextAttribsARB(h_devctx, NULL, attribs_builder.get_attribs());
	if (!h_newctx) {
		error = GetLastError();
		errorf("wglCreateContextAttribsARB failed. LastError() = %d (0x%x)", error, error);
		return false;
	}

	/* delete old context and make current new context */
	wglDeleteContext(h_glctx);
	wglMakeCurrent(h_devctx, h_newctx);
	h_glctx = h_newctx;
	b_window_active = true;
	return true;
}

const char *gl_window::get_last_error_string()
{
	return last_error;
}

void gl_window::show_cursor(bool b_show)
{
	ShowCursor((BOOL)b_show);
}

bool gl_window::is_cusor_visible()
{
	return false;
}

bool gl_window::is_window_alive()
{
	return b_window_active;
}

void gl_window::make_context_current()
{
	wglMakeCurrent(h_devctx, h_glctx);
}

void gl_window::swap_buffers()
{
	SwapBuffers(h_devctx);
}

void gl_window::poll_events()
{
	MSG msg;
	if (PeekMessageA(&msg, NULL, NULL, NULL, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessageA(&msg);
	}

	if (msg.message == WM_QUIT)
		b_window_active = false;
}

void gl_window::close(int code)
{
	PostQuitMessage(code);
}

bool gl_window::extension_avalible(const char *p_extname)
{
	if (!wglGetExtensionsStringARB)
		return false;

	const char *p_extnames = wglGetExtensionsStringARB(h_devctx);
	if (!p_extnames)
		return false;

	return strstr(p_extnames, p_extname) != NULL;
}

bool gl_window::shutdown()
{
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(h_glctx);
	ReleaseDC(h_window, h_devctx);
	DestroyWindow(h_window);
	UnregisterClassA(WINDOW_ID, GetModuleHandleW(0));
	return true;
}
