/**
  * Touhou Community Reliant Automatic Patcher
  * Team Shanghai Alice support plugin
  *
  * ----
  *
  * Touhou-specific wrappers around Win32 API functions.
  */

#include <thcrap.h>
#include "thcrap_tsa.h"

/// Window position hacks
/// ---------------------
/**
  * Recent games call GetWindowRect() before closing to store the current
  * position of the game window in their configuration file. The next time the
  * game is started, this position is used as the parameter for CreateWindowEx().
  *
  * Now, if the game is closed while being minimized, GetWindowRect() reports
  * bogus values (left and top = -32000). With these, CreateWindowEx() will
  * spawn the window at some unreachable "pseudo-minimized" position.
  * (Yeah, maybe it *is* possible to get it back *somehow*, but that way is
  * certainly not user-friendly.)
  *
  * The best solution seems to be replacing GetWindowRect() with
  * GetWindowPlacement(), which always reports the expected window coordinates,
  * even when the window in question is minimized.
  */ 
BOOL WINAPI tsa_GetWindowRect(
    __in HWND hWnd,
    __out LPRECT lpRect
)
{
	BOOL ret;
	WINDOWPLACEMENT wp;
	if(!lpRect) {
		return FALSE;
	}
	ZeroMemory(&wp, sizeof(WINDOWPLACEMENT));
	wp.length = sizeof(WINDOWPLACEMENT);
	ret = GetWindowPlacement(hWnd, &wp);
	memcpy(lpRect, &wp.rcNormalPosition, sizeof(RECT));
	return ret;
}

/**
  * A similar issue like the one with GetWindowRect() occurs when using
  * multiple monitors. When the game window is moved to a secondary monitor,
  * it will also spawn there on new sessions...
  *
  * ... even if the monitor in question is disconnected and the screen space is
  * not available anymore, thus leaving the window in an unreachable position
  * as well. (On Windows 7, you *can* get it back by hovering over its field in
  * the taskbar, right-clicking on the thumbnail and selecting "Move".)
  *
  * The Win32 API provides the current bounding rectangle of the entire screen
  * space through GetSystemMetrics(). We check the coordinates against that,
  * and make sure that the window position comes as close as possible to what
  * is actually requested.
  */
HWND WINAPI tsa_CreateWindowExA(
	__in DWORD dwExStyle,
	__in_opt LPCSTR lpClassName,
	__in_opt LPCSTR lpWindowName,
	__in DWORD dwStyle,
	__in int X,
	__in int Y,
	__in int nWidth,
	__in int nHeight,
	__in_opt HWND hWndParent,
	__in_opt HMENU hMenu,
	__in_opt HINSTANCE hInstance,
	__in_opt LPVOID lpParam
)
{
	RECT screens;
	screens.left = GetSystemMetrics(SM_XVIRTUALSCREEN);
	screens.top = GetSystemMetrics(SM_YVIRTUALSCREEN);
	screens.right = screens.left + GetSystemMetrics(SM_CXVIRTUALSCREEN);
	screens.bottom = screens.top + GetSystemMetrics(SM_CYVIRTUALSCREEN);
	if((X + nWidth) < screens.left) {
		X = screens.left;
	} else if(X > screens.right) {
		X = screens.right - nWidth;
	}
	if((Y + nHeight) < screens.top) {
		Y = screens.top;
	} else if(Y > screens.bottom) {
		Y = screens.bottom - nHeight;
	}
	return CreateWindowExU(
		dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y,
		nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam
	);
}
/// ---------------------

int win32_tsa_patch(HMODULE hMod)
{
	return iat_patch_funcs_var(hMod, "user32.dll", 2,
		"CreateWindowExA", tsa_CreateWindowExA,
		"GetWindowRect", tsa_GetWindowRect
	);
}