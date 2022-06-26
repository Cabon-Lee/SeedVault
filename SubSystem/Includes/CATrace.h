#pragma once

#include <malloc.h>
#include <windows.h>
#include <stdarg.h>
#include <stdio.h>
#include <debugapi.h>

#define CA_TRACE(x, ...) Trace(x, ##__VA_ARGS__)
#define CA_TRACE_LOG Trace("File : %s ", __FILE__); Trace("Func : %s ", __FUNCTION__); 

inline void Trace(char const* const format, ...)
{
	va_list args;
	int     len;
	char* buffer;

	// retrieve the variable arguments
	va_start(args, format);


	len = _vscprintf(format, args) // _vscprintf doesn't count
		+ 1; // terminating '\0'

	buffer = (char*)malloc(len * sizeof(char));

	if (0 != buffer)
	{
		vsprintf_s(buffer, len, format, args); // C4996
		// Note: vsprintf is deprecated; consider using vsprintf_s instead
		puts(buffer);

		OutputDebugStringA(buffer);
		OutputDebugStringA("\n");

		free(buffer);
	}
	va_end(args);
}
