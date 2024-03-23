/*! @author Johannes Bornholdt
 * @file init_game.c
 * file contains the functions to initialize all data for the game
 */

#include "shared.h"
#include "helpers.h"

/*! @brief Load the text base.

@note The text base is not allowed to store strings greater than 511 characters. */
static int
loadTextBase( const char* file, char*** textBase )
{
  FILE* stream;
  size_t i;
  size_t j;
  
  *textBase = malloc(sizeof * (*textBase) * TEXT_BASE_SIZE);
  if ( !*textBase )
    exit( EXIT_FAILURE );
  
  for ( i = 0; i < TEXT_BASE_SIZE; ++i )
  {
    (*textBase)[i] = malloc(sizeof * (*textBase)[i] * TEXT_BASE_TEXT_LENGTH);
    if ( !((*textBase)[i]) )
      exit( EXIT_FAILURE );
    
    memset( (*textBase)[i], 0, TEXT_BASE_TEXT_LENGTH * sizeof *(*textBase)[i] );
  }
  
  stream = fopen(file, "rb");
  if ( !stream )
  {
    fprintf( stderr, "Failed to load textbase: '%s'.\n", file );
    return 0;
  }
  
  
  i = 0u;
  while ( NULL != fgets( (*textBase)[i], TEXT_BASE_TEXT_LENGTH, stream ) )
  {
    /* Replace the '\n' with '\0' at the string's end. */
    j = strlen( (*textBase)[i] );
    (*textBase)[i][j] = '\0';
    
    ++i;
  }
  
#ifndef NDEBUG
  do {
    --i;
    printf( "textBase[%i]: %s\n", (int)i, (*textBase)[i] );
  } while ( i > 0 );
#endif
  
  
  fclose( stream );
  stream = NULL;
  
  return 1;
}

/*! @brief Load complete botlist (all maps)
 *
 * @param state Complete status of the game to store date in
 */
void
loadBots( GameState* state, const SDL_PixelFormat* format )
{
  FILE* stream;

  stream = fopen("maps/bots.txt", "rb");
  if( !stream )
  {
    fprintf( stderr, "Failed to open file 'maps/bots.txt'\n" );
    exit( EXIT_FAILURE );
  }

  if ( !parseBotFile( stream, &state->bots, format ) )
  {
    fprintf( stderr, "Error parsing bot file" );
    exit( EXIT_FAILURE );
  }

  fclose( stream );
  stream = 0;
}


/*! @brief Initialize the menus.
 *
 * Initialize the Surfaces and state identifiers for both menus. 
 * @param state Complete status of the game to store data in
 */
void
initMenus( GameState* state )
{
  /* Indices: i for type of menu, j for item or next state */
  int i, j;
  char* imgPath;
  

  
  /* Set filenames for background graphics of the menus */
  char* bgFileNames[2] = { "bgmainmenu.bmp", "bggamemenu.bmp" };
  
  /* Set filenames for layer graphics to highlight the current menu item */
  char* itemFileNames[2][3] = { 
    { "mainmenuitem0.bmp", "mainmenuitem1.bmp", "mainmenuitem2.bmp" }, 
    { "gamemenuitem0.bmp", "gamemenuitem1.bmp", "gamemenuitem2.bmp" }
  };
  
  /* Link to the game states */
  int stateNames[2][3] = {
    { NS_LOADING, NS_LOADING, NS_QUIT }, 
    { NS_PLAYING, NS_SAVING, NS_MAIN_MENU }
  };
  
  imgPath = malloc(128 * sizeof(char));
  if ( !imgPath )
    exit( EXIT_FAILURE );
  
#ifndef NDEBUG
  printf ("initMenus called\n");
#endif
  /* Allocate the space for the menu data */
  state->menus = malloc(sizeof * state->menus);
  if ( !state->menus )
    exit( EXIT_FAILURE );
  memset( state->menus, 0, sizeof *state->menus );
  
  /* Do loading jobs for all data items of the menus */
  /* First: for both menus */
  for ( i = 0; i < 2; i++ )
  {
    /* Load background graphic */
    sprintf( imgPath, "images%d/%s", TILESIZE, bgFileNames[i] );
    state->menus->data[i].bg = SDL_LoadBMP(imgPath);
    if ( !state->menus->data[i].bg )
    {
      fprintf( stderr, "Failed to load menu image(s).\n" );
      exit( EXIT_FAILURE );
    }
    
    /* Set highlighted item */
    state->menus->data[i].highlightedItem = 0;
    
    /* Second: for all sub items */
    for ( j = 0; j < 3; j++ )
    {
      sprintf( imgPath, "images%d/%s", TILESIZE, itemFileNames[i][j] );
      
      /* Load graphics for highlighting the current item */
      state->menus->data[i].hItem[j] = SDL_LoadBMP(imgPath);
      if ( !state->menus->data[i].hItem[j] )
      {
        fprintf( stderr, "Failed to load menu image: '%s'.\n", imgPath );
        exit( EXIT_FAILURE );
      }
      
      /* Assign the states from the list above */
      state->menus->data[i].nextState[j] = stateNames[i][j];
    }
  }
}


GameState*
initGame( void )
{
  GameState* state;
  char*  filePath;
    
  filePath = malloc(128 * sizeof(char));
  if ( !filePath )
    exit( EXIT_FAILURE );
  
  
  /* Allocate mememory for state */
  state = malloc(sizeof * state);
  if ( !state )
    exit( EXIT_FAILURE );
  memset( state, 0, sizeof *state );

  /* Initialize the game state. */
  state->isRunning = 1;
  state->nextState = NS_MAIN_MENU;

  /* Initialize SDL and TTF */
  initializeSdl( &state->window, &state->surface, &state->renderer, &state->texture );
  initializeTtf();

  /* Initialize Menus */
  initMenus( state );

  /* Load all the bots */
 /* loadBots( state );*/

  /* Load the player's data. */
  state->player.pos.x = 0;
  state->player.pos.y = 0;
  state->player.isTalking = false;
  state->player.lrud = V_DOWN;
  sprintf( filePath, "images%d/%s", TILESIZE, "player2.bmp" );
  state->player.image = SDL_LoadBMP(filePath);
  if ( !state->player.image )
  {
    fprintf( stderr, "Failed to load player's image.\n" );
    exit( EXIT_FAILURE );
  }
  SDL_SetColorKey( state->player.image, SDL_TRUE,
    SDL_MapRGB( state->surface->format, 231, 255, 255 )
  );
  
  /*! @todo Load the text base here. */
  /*strcpy( state->textBase[0], "Hallo, Welt! Ich bin ein Bot" );
  strcpy( state->textBase[1], "Du pinkelst gegen den Baum. Wie frech!" );*/
  
  sprintf( filePath, "maps/%s.lng", GAME_LANGUAGE );
  if ( !loadTextBase( filePath, &state->textBase ) )
    exit( EXIT_FAILURE );

  changeState( state );

  return state;
}
