
/*
	common.c

	Utility functions for dpmaster

	Copyright (C) 2008-2009  Mathieu Olivier

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


#include "common.h"
#include "system.h"
#include "servers.h"


// ---------- Private variables ---------- //

// The log file
static FILE* log_file = NULL;

// Log file path
static char log_filepath [MAX_PATH] = DEFAULT_LOG_FILE;

// The info file
static FILE* info_file = NULL;

// Info file path
static char info_filepath [MAX_PATH] = DEFAULT_INFO_FILE;

// Should we (re)open the log file?
static volatile sig_atomic_t must_open_log = false;

// Should we close the log file?
static volatile sig_atomic_t must_close_log = false;


// ---------- Public variables ---------- //

// The current time (updated every time we receive a packet)
time_t crt_time;

// Maximum level for a message to be printed
#if !_DEBUG
msg_level_t max_msg_level = MSG_NORMAL;
#else
msg_level_t max_msg_level = MSG_DEBUG;
#endif

// Peer address. We rebuild it every time we receive a new packet
char peer_address [128];

// Should we print the date before any new console message?
qboolean print_date = false;


// ---------- Private functions ---------- //

/*
====================
BuildDateString

Return a string containing the current date and time
====================
*/
static const char* BuildDateString (void)
{
	static char datestring [80];

	size_t date_len = strftime (datestring, sizeof(datestring),
								"%Y-%m-%d %H:%M:%S %Z", localtime(&crt_time));

	// If the datestring buffer was too small, its contents
	// is now "indeterminate", so we need to clear it
	if (date_len == 0)
		datestring[0] = '\0';

	return datestring;
}


/*
====================
CloseLogFile

Close the log file
====================
*/
static void CloseLogFile (const char* datestring)
{
	if (log_file != NULL)
	{
		if (datestring == NULL)
			datestring = BuildDateString();

		fprintf (log_file, "\n> Closing log file (time: %s)\n", datestring);
		fclose (log_file);
		log_file = NULL;
	}
}


// ---------- Public functions (logging) ---------- //

/*
====================
Com_EnableLog

Enable the logging
====================
*/
void Com_EnableLog (void)
{
	must_open_log = true;
}


/*
====================
Com_FlushLog

Enable the logging
====================
*/
void Com_FlushLog (void)
{
	fflush (log_file);
}


/*
====================
Com_IsLogEnabled

Test if the logging is enabled
====================
*/
qboolean Com_IsLogEnabled (void)
{
	return (log_file != NULL);
}


/*
====================
Com_SetLogFilePath

Change the log file path
====================
*/
qboolean Com_SetLogFilePath (const char* filepath)
{
	if (filepath == NULL || filepath[0] == '\0')
		return false;

	strncpy(log_filepath, filepath, sizeof(log_filepath) - 1);
	log_filepath[sizeof(log_filepath) - 1] = '\0';

	return true;
}

/*
====================
Com_SetInfoFilePath

Change the info file path
====================
*/
qboolean Com_SetInfoFilePath (const char* filepath)
{
	if (filepath == NULL || filepath[0] == '\0')
		return false;

	strncpy(info_filepath, filepath, sizeof(info_filepath) - 1);
	info_filepath[sizeof(info_filepath) - 1] = '\0';

	return true;
}


/*
====================
Com_UpdateLogStatus

Update the logging status, opening or closing the log file when necessary
====================
*/
qboolean Com_UpdateLogStatus (qboolean init)
{
	// If we need to (re)open the log file
	if (must_open_log)
	{
		const char* datestring;

		must_open_log = false;

		datestring = BuildDateString ();
		CloseLogFile (datestring);

		log_file = fopen (log_filepath, "a");
		if (log_file == NULL)
		{
			Com_Printf (MSG_ERROR, "> ERROR: can't open log file \"%s\"\n",
						log_filepath);
			return false;
		}

		// Make the log stream fully buffered (instead of line buffered)
		setvbuf (log_file, NULL, _IOFBF, SETVBUF_DEFAULT_SIZE);

		fprintf (log_file, "> Opening log file (time: %s)\n", datestring);

		// if we're opening the log after the initialization, print the list of servers
		if (! init)
			Sv_PrintServerList (MSG_WARNING);

	}

	// If we need to close the log file
	if (must_close_log)
	{
		must_close_log = false;
		CloseLogFile (NULL);
	}

	return true;
}


// ---------- Public functions (misc) ---------- //

/*
====================
Com_OpenInfo
====================
*/

void Com_OpenInfo()
{
	info_file = fopen(info_filepath, "w");
}

/*
====================
Com_CloseInfo
====================
*/

void Com_CloseInfo()
{
	fflush(info_file);
	fclose(info_file);

	info_file = NULL;
}

/*
====================
Com_PrintInfo
====================
*/

void Com_PrintInfo(const char* format, ...)
{
	va_list args;

	if (!info_file) return;

	va_start (args, format);
	vfprintf (info_file, format, args);
	va_end (args);
}

/*
====================
Com_Printf

Print a message to screen, depending on its verbose level
====================
*/
void Com_Printf (msg_level_t msg_level, const char* format, ...)
{
	// If the message level is above the maximum level, or if we output
	// neither to the console nor to a log file, there nothing to do
	if (msg_level > max_msg_level ||
		(log_file == NULL && daemon_state == DAEMON_STATE_EFFECTIVE))
		return;

	// Print a time stamp if necessary
	if (print_date)
	{
		const char* datestring = BuildDateString();

		if (daemon_state < DAEMON_STATE_EFFECTIVE)
			printf ("\n* %s\n", datestring);
		if (log_file != NULL)
			fprintf (log_file, "\n* %s\n", datestring);

		print_date = false;
	}

	if (daemon_state < DAEMON_STATE_EFFECTIVE)
	{
		va_list args;

		va_start (args, format);
		vprintf (format, args);
		va_end (args);
	}
	if (log_file != NULL)
	{
		va_list args;

		va_start (args, format);
		vfprintf (log_file, format, args);
		va_end (args);
	}
}


/*
====================
Com_SignalHandler

Handling of the signals sent to this process
====================
*/
void Com_SignalHandler (int Signal)
{
	switch (Signal)
	{
#ifdef SIGUSR1
		case SIGUSR1:
			must_open_log = true;
			break;
#endif
#ifdef SIGUSR2
		case SIGUSR2:
			must_close_log = true;
			break;
#endif
		default:
			// We aren't suppose to be here...
			assert(false);
			break;
	}
}
