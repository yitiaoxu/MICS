/*
    C & CPP Utilities
    Copyright (C) 2023  Xiao Siyu

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef __ANSICOLORCONSOLE_H
#define __ANSICOLORCONSOLE_H

#if (defined(_WIN64) || defined(__linux__))
#define ANSICOLORCONSOLE_ENABLED
#endif


#ifdef ANSICOLORCONSOLE_ENABLED

#define ANSI_COLOR_FG_BLACK				"\x1b[30m"
#define ANSI_COLOR_FG_RED				"\x1b[31m"
#define ANSI_COLOR_FG_GREEN				"\x1b[32m"
#define ANSI_COLOR_FG_YELLOW			"\x1b[33m"
#define ANSI_COLOR_FG_BLUE				"\x1b[34m"
#define ANSI_COLOR_FG_MAGENTA			"\x1b[35m"
#define ANSI_COLOR_FG_CYAN				"\x1b[36m"
#define ANSI_COLOR_FG_WHITE				"\x1b[37m"

#define ANSI_COLOR_FG_BRIGHT_BLACK		"\x1b[90m"
#define ANSI_COLOR_FG_BRIGHT_RED		"\x1b[91m"
#define ANSI_COLOR_FG_BRIGHT_GREEN		"\x1b[92m"
#define ANSI_COLOR_FG_BRIGHT_YELLOW		"\x1b[93m"
#define ANSI_COLOR_FG_BRIGHT_BLUE		"\x1b[94m"
#define ANSI_COLOR_FG_BRIGHT_MAGENTA	"\x1b[95m"
#define ANSI_COLOR_FG_BRIGHT_CYAN		"\x1b[96m"
#define ANSI_COLOR_FG_BRIGHT_WHITE		"\x1b[97m"

#define ANSI_COLOR_BG_BLACK				"\x1b[40m"
#define ANSI_COLOR_BG_RED				"\x1b[41m"
#define ANSI_COLOR_BG_GREEN				"\x1b[42m"
#define ANSI_COLOR_BG_YELLOW			"\x1b[43m"
#define ANSI_COLOR_BG_BLUE				"\x1b[44m"
#define ANSI_COLOR_BG_MAGENTA			"\x1b[45m"
#define ANSI_COLOR_BG_CYAN				"\x1b[46m"
#define ANSI_COLOR_BG_WHITE				"\x1b[47m"

#define ANSI_COLOR_BG_BRIGHT_BLACK		"\x1b[100m"
#define ANSI_COLOR_BG_BRIGHT_RED		"\x1b[101m"
#define ANSI_COLOR_BG_BRIGHT_GREEN		"\x1b[102m"
#define ANSI_COLOR_BG_BRIGHT_YELLOW		"\x1b[103m"
#define ANSI_COLOR_BG_BRIGHT_BLUE		"\x1b[104m"
#define ANSI_COLOR_BG_BRIGHT_MAGENTA	"\x1b[105m"
#define ANSI_COLOR_BG_BRIGHT_CYAN		"\x1b[106m"
#define ANSI_COLOR_BG_BRIGHT_WHITE		"\x1b[107m"

#define ANSI_COLOR_FG_BRIGHT_GRAY 		ANSI_COLOR_FG_BRIGHT_BLACK

#define ANSI_COLOR_RESET				"\x1b[0m"

#else // ANSICOLORCONSOLE_ENABLED

#define ANSI_COLOR_FG_BLACK				""
#define ANSI_COLOR_FG_RED				""
#define ANSI_COLOR_FG_GREEN				""
#define ANSI_COLOR_FG_YELLOW			""
#define ANSI_COLOR_FG_BLUE				""
#define ANSI_COLOR_FG_MAGENTA			""
#define ANSI_COLOR_FG_CYAN				""
#define ANSI_COLOR_FG_WHITE				""

#define ANSI_COLOR_FG_BRIGHT_BLACK		""
#define ANSI_COLOR_FG_BRIGHT_RED		""
#define ANSI_COLOR_FG_BRIGHT_GREEN		""
#define ANSI_COLOR_FG_BRIGHT_YELLOW		""
#define ANSI_COLOR_FG_BRIGHT_BLUE		""
#define ANSI_COLOR_FG_BRIGHT_MAGENTA	""
#define ANSI_COLOR_FG_BRIGHT_CYAN		""
#define ANSI_COLOR_FG_BRIGHT_WHITE		""

#define ANSI_COLOR_BG_BLACK				""
#define ANSI_COLOR_BG_RED				""
#define ANSI_COLOR_BG_GREEN				""
#define ANSI_COLOR_BG_YELLOW			""
#define ANSI_COLOR_BG_BLUE				""
#define ANSI_COLOR_BG_MAGENTA			""
#define ANSI_COLOR_BG_CYAN				""
#define ANSI_COLOR_BG_WHITE				""

#define ANSI_COLOR_BG_BRIGHT_BLACK		""
#define ANSI_COLOR_BG_BRIGHT_RED		""
#define ANSI_COLOR_BG_BRIGHT_GREEN		""
#define ANSI_COLOR_BG_BRIGHT_YELLOW		""
#define ANSI_COLOR_BG_BRIGHT_BLUE		""
#define ANSI_COLOR_BG_BRIGHT_MAGENTA	""
#define ANSI_COLOR_BG_BRIGHT_CYAN		""
#define ANSI_COLOR_BG_BRIGHT_WHITE		""

#define ANSI_COLOR_FG_BRIGHT_GRAY 		ANSI_COLOR_FG_BRIGHT_BLACK

#define ANSI_COLOR_RESET				""

#endif // ANSICOLORCONSOLE_ENABLED

#define ANSI_COLOR(color, str) ANSI_COLOR_FG_##color str ANSI_COLOR_RESET

#endif //__ANSICOLORCONSOLE_H
