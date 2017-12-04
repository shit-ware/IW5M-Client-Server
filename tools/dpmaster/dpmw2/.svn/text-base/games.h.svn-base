/*
	games.h

	Games management for dpmaster

	Copyright (C) 2009  Mathieu Olivier

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


#ifndef _GAMES_H_
#define _GAMES_H_


// ---------- Public functions ---------- //

// Declare the server policy regarding which games are allowed on this master
cmdline_status_t Game_DeclarePolicy (const char* policy, const char** games, unsigned int nb_games);

// Return true if the game is allowed on this master
qboolean Game_IsAccepted (const char* game_name);


#endif  // #ifndef _GAMES_H_
