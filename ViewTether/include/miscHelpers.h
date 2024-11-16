#pragma once

#include <windows.h>
#include <shellapi.h>


void openLink(const wchar_t * link) {
	ShellExecute(0, 0, link, 0, 0 , SW_SHOW );
}

