#include "stdinc.h"
#include <string>

void remove_colors(std::string & str)
{
	unsigned int len = str.length();

	for (unsigned int i = 0; i < len; ++i)
	{
		if (str[i] == '^')
		{
			if (str[i + 1] >= 0x30 && str[i + 1] <= 0x39)
			{
				str.erase(i, 2);
				len -= 2;
				continue;
			}
		}
	}

	return;
}

void PatchIW5_ConsoleTitle()
{
	HWND * console_window = (HWND*)0x0592C828;
	dvar_t * sv_hostname = (dvar_t*)0x00586159C;

	std::string hostname(sv_hostname->current.string);

	if (!*console_window)
	{
		return;
	}

	remove_colors(hostname);

	SetWindowTextA(*console_window, hostname.c_str());

	return;
}