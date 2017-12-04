/*
	games.c

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


#include "common.h"
#include "system.h"
#include "games.h"


// ---------- Private variables ---------- //

static const char** game_names = NULL;
static unsigned int nb_game_names = 0;
static qboolean reject_when_known = true;


// ---------- Private functions ---------- //

/*
====================
Game_Find
 
Find a game name in the list of game names
After the call, *index_ptr will contain the index where the game is stored in game_names (or should be stored, if it is not present)
====================
*/
static qboolean Game_Find (const char* game_name, unsigned int* index_ptr)
{
	int left = 0;

	if (game_names != NULL)
	{
		int right = nb_game_names - 1;

		while (left <= right)
		{
			int middle, diff;

			middle = (left + right) / 2;
			diff = strcmp(game_names[middle], game_name);

			if (diff == 0)
			{
				if (index_ptr != NULL)
					*index_ptr = middle;
				return true;
			}

			if (diff > 0)
				right = middle - 1;
			else
				left = middle + 1;
		}
	}
		
	if (index_ptr != NULL)
		*index_ptr = left;
	return false;
}


// ---------- Public functions ---------- //

/*
====================
Game_DeclarePolicy

Declare the server policy regarding which games are allowed on this master
====================
*/
cmdline_status_t Game_DeclarePolicy (const char* policy, const char** games, unsigned int nb_games)
{
	qboolean new_reject_when_known;
	unsigned int i;

	if (strcmp (policy, "accept") == 0)
		new_reject_when_known = false;
	else if (strcmp (policy, "reject") == 0)
		new_reject_when_known = true;
	else
		return CMDLINE_STATUS_INVALID_OPT_PARAMS;

	// If this is the first game policy option we parse, assign the default game policy
	if (game_names == NULL)
		reject_when_known = new_reject_when_known;

	// Else, this list must be compatible with the previous one(s)
	else if (new_reject_when_known != reject_when_known)
		return CMDLINE_STATUS_INVALID_OPT_PARAMS;

	for (i = 0; i < nb_games; i++)
	{
		unsigned int index;
		const char* game = games[i];
		
		// If we don't already have this game in the list, add it
		if (! Game_Find (game, &index))
		{
			const char** new_game_names;
			
			new_game_names = realloc ((void*)game_names, (nb_game_names + 1) * sizeof (game_names[0]));
			if (new_game_names == NULL)
				return CMDLINE_STATUS_NOT_ENOUGH_MEMORY;
			
			memmove((void*)&new_game_names[index + 1], &new_game_names[index], (nb_game_names - index) * sizeof (new_game_names[0]));
			new_game_names[index] = game;

			game_names = new_game_names;
			nb_game_names++;
		}
	}

	return CMDLINE_STATUS_OK;
}


/*
====================
Game_IsAccepted

Return true if the game is allowed on this master
====================
*/
qboolean Game_IsAccepted (const char* game_name)
{
	return (Game_Find (game_name, NULL) ^ reject_when_known);
}
