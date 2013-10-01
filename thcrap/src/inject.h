/**
  * Touhou Community Reliant Automatic Patcher
  * Main DLL
  *
  * ----
  *
  * DLL injector.
  * Adapted from http://www.codeproject.com/Articles/20084/completeinject
  */

#pragma once

/// Entry point determination
/// -------------------------
// After creating a process in suspended state, EAX is guaranteed to contain
// the correct address of the entry point, even when the executable has the
// DYNAMICBASE flag activated in its header.
//
// (Works on Windows, but not on Wine)
void* entry_from_context(HANDLE hThread);

// Reads the entry point from the PE header.
//
// (Works always, provided that [base_addr] is correct)
void* entry_from_header(HANDLE hProcess, void *base_addr);

// Returns the base address of the module with the given title in [hProcess].
//
// (Works on Wine, but not on Windows immediately after the target process was
//  created in suspended state.)
void* module_base_get(HANDLE hProcess, const char *module);
/// -------------------------

int ThreadWaitUntil(HANDLE hProcess, HANDLE hThread, void *addr);
int WaitUntilEntryPoint(HANDLE hProcess, HANDLE hThread, const char *module);

// CreateProcess with thcrap DLL injection.
// Careful! If you call this with CREATE_SUSPENDED set, you *must* resume the
// thread on your own! This is necessary for cooperation with other patches
// using DLL injection.
BOOL WINAPI inject_CreateProcessU(
	__in_opt LPCSTR lpAppName,
	__inout_opt LPSTR lpCmdLine,
	__in_opt LPSECURITY_ATTRIBUTES lpProcessAttributes,
	__in_opt LPSECURITY_ATTRIBUTES lpThreadAttributes,
	__in BOOL bInheritHandles,
	__in DWORD dwCreationFlags,
	__in_opt LPVOID lpEnvironment,
	__in_opt LPCSTR lpCurrentDirectory,
	__in LPSTARTUPINFOA lpSI,
	__out LPPROCESS_INFORMATION lpPI
);

// Injects thcrap into the given [hProcess], and passes [setup_fn].
int thcrap_inject(HANDLE hProcess, const char *setup_fn);
