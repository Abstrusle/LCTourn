
#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>

#include <stdio.h>
#include <string.h>

#include "m_config.h"

#include "g_header.h"
#include "m_globvars.h"

#include "g_misc.h"
#include "i_console.h"
#include "t_template.h"

#include "e_log.h"
#include "e_editor.h"
#include "g_game.h"

#include "f_save.h"
#include "f_load.h"

// This file contains functions to load and save gamefiles
// it uses some of the functions in f_save.c and f_load.c

extern ALLEGRO_DISPLAY* display; // used to display the native file dialog

extern struct save_statestruct save_state; // in f_save.c
extern struct world_init_struct w_init; // this is the world_init generated by world setup menus - declared in s_menu.c
//extern struct templstruct templ [TEMPLATES]; // see t_template.c
extern struct game_struct game;

int save_gamefile_to_file(void);




// Call this function to save the contents of w_init and the system file template to disk
// uses various save functions in f_save.c
// if an error occurs, it writes to the mlog then returns 0
int save_gamefile(void)
{

 if (!open_save_file("Save gamefile", "*.gam"))
  return 0; // no file opened, so don't goto save_gamefile_fail

 save_state.bp = 0;
 save_state.error = 0;

 if (!save_gamefile_to_file())
  goto save_gamefile_fail;

 if (save_state.bp != 0) // means there is probably something left in the buffer
  write_save_buffer();

 flush_game_event_queues(); // may not be necessary

 close_save_file();

 if (save_state.error == 1)
  goto save_gamefile_fail;

 write_line_to_log("Gamefile saved.", MLOG_COL_FILE);
 return 1; // success!

save_gamefile_fail:
 write_line_to_log("Gamefile not saved.", MLOG_COL_ERROR); // A relevant error message should already have been written.
 close_save_file();
 return 0;

}

// This function saves the contents of world_init and the system file template to a gamefile
int save_gamefile_to_file(void)
{
/*
 int i;
 int j;

// First save contents of game_struct (although not all values need to be saved, which is why we don't use save_game_struct_to_file() in f_save.c)
// save_int(game.phase);
// save_int(game.turns);
// save_int(game.minutes_each_turn);

// Now save contents of w_init (which should have been set up by a call to derive_world_init_from_menu() in s_menu.c)
 save_int(w_init.players);
 save_int(w_init.game_turns);
 save_int(w_init.game_minutes_each_turn);
 save_int(w_init.procs_per_player);
// save_int(w_init.gen_limit);
 save_int(w_init.packets_per_player);
 save_int(w_init.max_clouds);
 save_int(w_init.w_block);
 save_int(w_init.h_block);

 for (i = 0; i < w_init.players; i ++)
 {
//  save_int(w_init.team_colour [i]);
  save_int(w_init.may_change_client_template [i]);
  save_int(w_init.may_change_proc_templates [i]);
  for (j = 0; j < PLAYER_NAME_LENGTH; j++)
  {
   save_char(w_init.player_name [i] [j]);
  }
 }
 save_int(w_init.player_clients);
 save_int(w_init.allow_observer);
 save_int(w_init.player_operator);
 save_int(w_init.system_program);

 if (save_state.error == 1)
  return 0;

// now save contents of system file template:
 save_template(0, 0); // can assume system file template is template 0

 if (save_state.error == 1)
  return 0;
*/
 return 1;

}



// Loading:

extern struct load_statestruct load_state;

int load_gamefile_from_file(void);


// call this when load game item selected from game system menu
// opens native file dialogue and loads file
// writes to mlog on failure or error
int load_gamefile(void)
{

 if (!open_load_file("Load gamefile", "*.*"))
  return 0;

 load_state.bp = -1; // means first entry read will be 0
 load_state.error = 0;
 load_state.current_buffer_size = 0;

// load first buffer:
 if (!read_load_buffer())
  goto load_gamefile_fail;

 if (!load_gamefile_from_file())
  goto load_gamefile_fail;

 flush_game_event_queues(); // Probably not necessary

 close_load_file();
 write_line_to_log("Gamefile loaded.", MLOG_COL_FILE);
 return 1; // success!!


load_gamefile_fail:

  close_load_file();
//  reset_templates();
  write_line_to_log("Gamefile not loaded.", MLOG_COL_ERROR);
  return 0;


}


int load_gamefile_from_file(void)
{
/*
 int i;

// other game_struct values are filled in by start_game
// load_int(&game.phase, 0, GAME_PHASES - 1, "game phase");
// load_int(&game.turns, 0, MAX_LIMITED_GAME_TURNS, "game turns");
// load_int(&game.minutes_each_turn, 0, MAX_TURN_LENGTH_MINUTES, "turn length");

 load_int(&w_init.players, MIN_PLAYERS, MAX_PLAYERS, "players");
 load_int(&w_init.game_turns, 0, MAX_LIMITED_GAME_TURNS, "game turns");
 load_int(&w_init.game_minutes_each_turn, 0, MAX_TURN_LENGTH_MINUTES, "turn length");

 if (load_state.error == 1)
  return 0;

// set up some basic game values from the initstruct:
 start_game(GAME_PHASE_PREGAME, w_init.game_turns, w_init.game_minutes_each_turn);

 load_int(&w_init.procs_per_player, MIN_procs_per_player, MAX_procs_per_player, "processes per team");
// load_int(&w_init.gen_limit, MIN_GEN_LIMIT, MAX_GEN_LIMIT, "gen limit");
 load_int(&w_init.packets_per_player, MIN_PACKETS, MAX_PACKETS, "packets");
 load_int(&w_init.max_clouds, CLOUDS, CLOUDS, "clouds");
 load_int(&w_init.w_block, MIN_BLOCKS, MAX_BLOCKS, "w_blocks");
 load_int(&w_init.h_block, MIN_BLOCKS, MAX_BLOCKS, "h_blocks");
 for (i = 0; i < w_init.players; i ++)
 {
//  load_int(&w_init.team_colour [i], 0, TEAM_COLOUR_CHOICES - 1, "team colours");
  load_int(&w_init.may_change_client_template [i], 0, 1, "change client template");
  load_int(&w_init.may_change_proc_templates [i], 0, 1, "change process templates");
  load_string(w_init.player_name [i], PLAYER_NAME_LENGTH, 0, "player name");
//  fprintf(stdout, "\nplayer %i name (%s)", i, w_init.player_name[i]);
 }
 load_int(&w_init.player_clients, 0, 1, "player clients");
 load_int(&w_init.allow_observer, 0, 1, "allow observer");
 load_int(&w_init.player_operator, -1, w_init.players - 1, "operator")
 ;
 load_int(&w_init.system_program, 0, 1, "system program");

 if (load_state.error == 1)
  return 0;

 setup_system_template(); // this resets templates and leaves us with just a system template
// w.players = w_init.players; // just need to set this so load_template doesn't reject a player number
 load_template(0, 0); // 2nd 0 means don't load basic template properties - setup_system_template will hav set up the system template
 set_opened_template(0, TEMPL_TYPE_SYSTEM, 1);

 if (load_state.error == 1)
  return 0;

*/

 return 1;
}


