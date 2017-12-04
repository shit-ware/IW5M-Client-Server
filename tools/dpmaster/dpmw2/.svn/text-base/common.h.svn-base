/*
	common.h

	Common header file for dpmaster

	Copyright (C) 2004-2009  Mathieu Olivier

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


#ifndef _COMMON_H_
#define _COMMON_H_


#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>


// ---------- Constants ---------- //

// Maximum and minimum sizes for a valid incoming packet
#define MAX_PACKET_SIZE_IN 2048
#define MIN_PACKET_SIZE_IN 5


// ---------- Types ---------- //

// A few basic types
typedef enum {false, true} qboolean;
typedef unsigned char qbyte;

// The various messages levels
typedef enum
{
	MSG_NOPRINT,	// used by "max_msg_level" (= no printings)
	MSG_ERROR,		// errors
	MSG_WARNING,	// warnings
	MSG_NORMAL,		// standard messages
	MSG_DEBUG		// for debugging purpose
} msg_level_t;

// Command line option
typedef struct
{
	const char* long_name;		// if NULL, this is the end of the list
	const char* help_syntax;	// help string printed by PrintHelp (syntax)
	const char* help_desc;		// help string printed by PrintHelp (description)
	int	help_param [2];			// optional parameters for the "help_desc" string
	char short_name;			// may be '\0' if it has no short name
	unsigned int min_params;	// minimum number of parameters for this option
	unsigned int max_params;	// maximum number of parameters for this option
}  cmdlineopt_t;

// Command line status
typedef enum
{
	CMDLINE_STATUS_OK,
	CMDLINE_STATUS_SHOW_HELP,

	// Errors
	CMDLINE_STATUS_INVALID_OPT,
	CMDLINE_STATUS_NOT_ENOUGH_OPT_PARAMS,
	CMDLINE_STATUS_TOO_MUCH_OPT_PARAMS,
	CMDLINE_STATUS_INVALID_OPT_PARAMS,
	CMDLINE_STATUS_NOT_ENOUGH_MEMORY,
} cmdline_status_t;


// ---------- Public variables ---------- //

// The current time (updated every time we receive a packet)
extern time_t crt_time;

// Maximum level for a message to be printed
extern msg_level_t max_msg_level;

// Peer address. We rebuild it every time we receive a new packet
extern char peer_address [128];

// Should we print the date before any new console message?
extern qboolean print_date;


// ---------- Public functions (logging) ---------- //

// Enable the logging
void Com_EnableLog (void);

// Flush the buffer of the log file
void Com_FlushLog (void);

// Test if the logging is enabled
qboolean Com_IsLogEnabled (void);

// Set the log file path (taken into account the next time it's opened)
qboolean Com_SetLogFilePath (const char* filepath);

// Set the info file path (taken into account the next time it's opened)
qboolean Com_SetInfoFilePath (const char* filepath);

// Update the logging status, opening or closing the log file when necessary
qboolean Com_UpdateLogStatus (qboolean init);


// ---------- Public functions (misc) ---------- //

// Open the info file
void Com_OpenInfo (void);

// Close the info file
void Com_CloseInfo (void);

// Output a line to the info file
void Com_PrintInfo (const char* format, ...);

// Print a text to the screen and/or to the log file
void Com_Printf (msg_level_t msg_level, const char* format, ...);

// Handling of the signals sent to this process
void Com_SignalHandler (int Signal);


#endif  // #ifndef _COMMON_H_
