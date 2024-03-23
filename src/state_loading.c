#include "shared.h"
#include "helpers.h"

extern void drawWaitingScreen ( const SDL_Surface*, char* );
extern void loadBots( GameState* state, const SDL_PixelFormat* format );

/* Set mark to skip the first run of the loadingUpdate functon */
static int skipMark = 1;


/*! @brief Handle input while loading.
 *
 * Capture closing the window to end the programm right. 
 * @param state Complete status of the game to store data in
 */
void
loadingHandleInput( GameState* state )
{
  /* In loading we don't handle events but the exit event. */
  SDL_Event event;

  while ( SDL_PollEvent( &event ) )
  {
    switch ( event.type )
    {
    case SDL_QUIT :
      state->isRunning = 0;
      break;
    default :
      break;
    }
  }
}



/*! @brief Load the map.
 *
 * Parse the map and load it. 
 * @param state Complete status of the game to store data in
 */
void
loadingUpdate( GameState* state )
{
  FILE* stream;
  char nextMapPath[FILENAME_MAX] = "maps/";

  /* Skip this function once to draw loading screen first */
  if (skipMark == 1) 
  {
#ifndef NDEBUG
    printf("Skipped one run of loadingUpdate\n");
#endif
    
    /* Set skipMark to zero to enter loadingUpdate next time */
    skipMark = 0;
    return;
  }
  
  /* Set skipMark to skip loadingUpdate for next time (the state is entered) */
  skipMark = 1;

  /* This should only the case when starting a new game. Loading the savegame */
  if ( state->nextMap == NULL )
  {    
    /* TRICKY: Trace back whether the user clicked 'starting' or 'loading'. */
    /* Zero is 'Start Game'; One is 'Load' */
    switch ( state->menus->data[0].highlightedItem )
    {
    case 1 :
      if ( (stream = fopen( "savegame.txt", "rb" )) != NULL )
        /* Ok, we got our savegame, now we load it */
        break;

      /* Fallthrough if the savegame could not be loaded and start a new game */
      fprintf( stderr, "Unable to open savegame.txt, starting a new game.\n" );

    case 0 :  /* Start a new game from scratch */
    {
      stream = fopen("maps/default_savegame.txt", "r");
      if ( !stream )
      {
        /* We don't have a default savegame? Crash! */
        fprintf( stderr, "Can't start new game, default savegame not found.\n" );
        exit( EXIT_FAILURE );
      }

      break;
    }

    default :
      /* Outch, That's terribly bad. */
      fprintf( stderr, "I do not know how to proceed. Exiting ...\n" );
      exit( EXIT_FAILURE );
      break;
    }
    
    if ( state->map )
    {
      joinBotLists( &state->bots, &state->map->bots );
      freeMap( state->map );
      state->map = NULL;
    }

    if ( state->bots.root != NULL )
    {
      botListFree( state->bots );
      state->bots.root = NULL;
    }
    loadBots( state, state->surface->format );

    loadSaveGame( stream, state );
    
    fclose( stream );
    stream = NULL;
  }
  else
  {
    if ( strcmp( state->map->file, state->nextMap->targetMap ) == 0 )
      /* if we do not reload map, we need to clean up our old position */
      state->map->clipMap[state->player.pos.y * state->map->w + state->player.pos.x] = 0;

    strcpy( state->player.map, state->nextMap->targetMap );
    state->player.pos.x = state->nextMap->targetPos.x;
    state->player.pos.y = state->nextMap->targetPos.y;
    state->nextMap = NULL;
  }

  /* We just need to load the map if either we have no map jet,
     or the filename of the current map differs from where the player wants to go */
  if (
      !state->map ||
      strcmp( state->map->file, state->player.map ) != 0
     )
  {
    /* Free map if already allocated. */  
    if ( state->map )
    {
      /* Save map bots first and move them to global bot list */
      joinBotLists( &state->bots, &state->map->bots );
      /* Now map may be freed */
      freeMap( state->map );
      state->map = NULL;
    }

    /* Compute the whole path to the next map. */
    strcat( nextMapPath, state->player.map );

    /* Load the map. */
    state->map = malloc(sizeof * state->map);
    if ( !state->map )
      exit( EXIT_FAILURE );
    memset( state->map, 0, sizeof *state->map );

#ifndef NDEBUG
    printf( "Loading map: '%s' @ x = %i, y = %i\n",
      state->player.map, state->player.pos.x, state->player.pos.y
    );
#endif

    stream = fopen( nextMapPath, "rb" );
    if ( !stream )
    {
      fprintf( stderr, "Failed to open file '%s'\n", nextMapPath );
      exit( EXIT_FAILURE );
    }

    if ( !parseMap( stream, state->map, state->surface->format ) )
    {
      fprintf( stderr, "Error parsing map '%s'\n", nextMapPath );
      exit( EXIT_FAILURE );
    }

    fclose( stream );
    stream = 0;

    strcpy( state->map->file, state->player.map );

    /* Move bots from global list to map list */
    moveBotDepMap( &state->map->bots, &state->bots, state->map->file );
    /* Now we need to update the clipMap with the new bots */
    updateBotClipMap( state );

#ifndef NDEBUG
    SDL_SaveBMP( state->map->bg, "map_bg.bmp" );
    SDL_SaveBMP( state->map->fg, "map_fg.bmp" );
#endif
  }

  /* Update the clip map */
  state->map->clipMap[state->player.pos.y * state->map->w + state->player.pos.x] = 3;

  /* Now we are ready to enter the game. */
  state->nextState = NS_PLAYING;

#ifndef NDEBUG
  printf( "Loading completed.\n" );
#endif

  /* Make loading take a little longer,
     so we can see our nice loading box */
  SDL_Delay( 1000 );
}



/*! @brief Draw the loading screen.
 *
 * Draw the loading screen and change mark to enter loadingUpdate next time.
 * @param state Complete status of the game to store data in
 */
void loadingDraw ( GameState* state )
{
  drawWaitingScreen ( state->surface, "Loading..." );

#ifndef NDEBUG
  printf ("Draw: skipMark is set to 0\n");
#endif
}
