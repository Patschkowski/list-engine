#include "shared.h"

void drawWaitingScreen ( const SDL_Surface*, char* );

/* Set mark to skip the first run of the loadingUpdate functon */
static int skipMark = 1;

/*! @brief Handle input while saving.
 *
 * Avoid closing application while saving to prevent corrupted data. 
 * @param state Complete status of the game to store data in
 */
void
savingHandleInput( GameState* state )
{
  SDL_Event event;
  
  /* Shut up compiler */
  if ( state ) { }

  /* In saving we don't handle events, but we clear the event queue. */
  while ( SDL_PollEvent( &event ) )
  {
    switch ( event.type )
    {
      case SDL_QUIT :
        break;
      default :
        break;
    }
  }
}



/*! @brief Save the game.
 *
 * Save the gamestate at 2nd run of this function. 
 * @param state Complete status of the game to store data in
 */
void
savingUpdate( GameState* state )
{
  FILE* stream;
  
  /* Skip this function once to draw saving screen first */
  if (skipMark == 1) 
  {
#ifndef NDEBUG
    printf("Skip one run of loadingUpdate\n");
#endif
    return;
  }

  stream = fopen("savegame.txt", "wb");
  if ( !stream )
  {
    fprintf( stderr, "Failed to open savegame.txt\n" );
    exit( EXIT_FAILURE );
  }
  
  /* For now the player's position and the current map is saved. */
  fprintf( stream, "%i %i %s\n", state->player.pos.x, state->player.pos.y,
    state->map->file
  );

  joinBotLists( &state->bots, &state->map->bots );
  writeSaveGame( stream, state );
  moveBotDepMap( &state->map->bots, &state->bots, state->map->file );
  
  fclose ( stream );
  /* Let us have a look on our saving screen */
  SDL_Delay ( 1000 );
  /* Forward to game menu. */
  state->nextState = NS_GAME_MENU;
}



/*! @brief Draw the saving screen.
 *
 * Draw the saving screen and enable run of savingUpdate next. 
 * @param state Complete status of the game to store data in
 */
void
savingDraw( const GameState* state )
{
  drawWaitingScreen ( state->surface, "Saving..." );
  
  /* Set skipMark to zero to enter savingUpdate next time */
  skipMark = 0;
#ifndef NDEBUG
  printf ("Draw: skipMark is set to 0\n");
#endif
}
