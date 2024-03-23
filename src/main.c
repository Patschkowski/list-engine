#include "shared.h"

/* Declare the used functions. */
extern void loadingHandleInput( GameState* );
extern void loadingUpdate( GameState* );
extern void loadingDraw( const GameState* );
extern void savingHandleInput( GameState* );
extern void savingUpdate( GameState* );
extern void savingDraw( const GameState* );
extern void playingHandleInput( GameState* );
extern void playingUpdate( GameState* );
extern void playingDraw( const GameState* );
extern void menuHandleInput( GameState* );
extern void menuUpdate( GameState* );
extern void menuDraw( const GameState* );



void
changeState( GameState* state )
{
  /* Change to the next game state if necessary. */
  switch ( state->nextState )
  {
  case NS_NONE :
    return;

  case NS_QUIT :
    state->isRunning = 0;
    break;
    
  case NS_LOADING :
    state->handleInput = &loadingHandleInput;
    state->update      = &loadingUpdate;
    state->draw        = &loadingDraw;
    break;

  case NS_SAVING :
    state->handleInput = &savingHandleInput;
    state->update      = &savingUpdate;
    state->draw        = &savingDraw;
    break;

  case NS_PLAYING :
    state->nextMoveTick = SDL_GetTicks();
    state->handleInput  = &playingHandleInput;
    state->update       = &playingUpdate;
    state->draw         = &playingDraw;
    break;

  case NS_MAIN_MENU :
    #ifndef NDEBUG
      printf ("Mark at NS_MAIN_MENU\n");
    #endif
    state->menus->current = 0;
    state->menus->data[state->menus->current].highlightedItem = 0;
    state->handleInput = &menuHandleInput;
    #ifndef NDEBUG
      printf ("Second mark at NS_MAIN_MENU\n");
    #endif
    state->update      = &menuUpdate;
    state->draw        = &menuDraw;
    break;

  case NS_GAME_MENU :
    #ifndef NDEBUG
      printf ("Mark at NS_GAME_MENU\n");
    #endif
    state->menus->current = 1;
    state->menus->data[state->menus->current].highlightedItem = 0;
    state->handleInput = &menuHandleInput;
    state->update      = &menuUpdate;
    state->draw        = &menuDraw;
    break;

  default :
    fprintf( stderr, "An unsupported game state lerks around.\n" );
    break;
  }

  state->nextState = NS_NONE;
}



int
main( int nArguments, char** arguments )
{
  unsigned int nFrameskip;
  Uint32       nextGameTick;
  GameState*   state;
  size_t       i;

  /* Shut up compiler. */
  if ( nArguments || arguments ){}

  state = initGame();

  /* Run the main game loop. */
  nextGameTick = SDL_GetTicks();
  while ( state->isRunning )
  {
    nFrameskip = 0u;
    while (
      SDL_GetTicks() > nextGameTick &&
      nFrameskip < MAX_FRAMESKIP
      && state->nextState == NS_NONE
    )
    {
      if ( state->handleInput )
        (*state->handleInput)( state );

      if ( state->update )
        (*state->update)( state );

      nextGameTick += GAME_TICK;
      ++nFrameskip;
    }

    /* Draw the game. */
    if ( state->draw )
      (*state->draw)( state );

    if (0 != SDL_UpdateTexture(state->texture, NULL, state->surface->pixels, state->surface->pitch))
    {
      fprintf(stderr, "%s\n", SDL_GetError());
      state->isRunning = 0;
      continue;
    }

    if (0 != SDL_RenderClear(state->renderer))
    {
      fprintf(stderr, "%s\n", SDL_GetError());
      state->isRunning = 0;
      continue;
    }

    if (0 != SDL_RenderCopy(state->renderer, state->texture, NULL, NULL))
    {
      fprintf(stderr, "%s\n", SDL_GetError());
      state->isRunning = 0;
      continue;
    }

    /* Swap front and back buffer. */
    SDL_RenderPresent(state->renderer);

    changeState( state );
    SDL_Delay( 20 );
  }
  
  if ( state->map )
  {
    freeMap( state->map );
    state->map = NULL;
  }

  if ( state->player.image )
  {
    SDL_FreeSurface( state->player.image );
    state->player.image = NULL;
  }
    
  if ( state->menus )
  {
    SDL_FreeSurface( state->menus->data[0].bg );
    SDL_FreeSurface( state->menus->data[1].bg );
    SDL_FreeSurface( state->menus->data[0].hItem[0] );
    SDL_FreeSurface( state->menus->data[0].hItem[1] );
    SDL_FreeSurface( state->menus->data[0].hItem[2] );
    SDL_FreeSurface( state->menus->data[1].hItem[0] );
    SDL_FreeSurface( state->menus->data[1].hItem[1] );
    SDL_FreeSurface( state->menus->data[1].hItem[2] );
  }
  
  for ( i = 0u; i < TEXT_BASE_SIZE; ++i )
    free( state->textBase[i] );
  free( state->textBase );

  /* Cleanup. */
  free( state );
  state = NULL;

  /* Return to the operating system successfully. */
  return EXIT_SUCCESS;
}



/* TODO: Move this to another file? */
void
blitRegion( SDL_Surface* region, int x, int y, int w, int h, SDL_Surface* dst, const SDL_PixelFormat* format )
{
  SDL_Rect rect;
  int i, j;
  int blit;
  const char* error;

  rect.w = rect.h = TILESIZE;
  rect.x = TILESIZE * x;
  rect.y = TILESIZE * y;

  for ( i = 0; i < w; ++i )
  {
    rect.y = TILESIZE * y;
    for ( j = 0; j < h; ++j )
    {
      SDL_SetColorKey( region, SDL_TRUE, SDL_MapRGB( format, 231, 255, 255 ) );
      blit = SDL_BlitSurface( region, NULL, dst, &rect );
      if (0 != blit) {
        error = SDL_GetError();
        fprintf(stderr, "Region blit failed: %s\n", error);
      }

      rect.y += TILESIZE;
    }
    rect.x += TILESIZE;
  }
}



void
freeMap( Map* map )
{
  struct ImgListNode* img, * nextImg;
  struct BotListNode* bot, * nextBot;

  if ( map->fg )
    SDL_FreeSurface( map->fg );
  
  if ( map->bg )
    SDL_FreeSurface( map->bg );
  
  for ( img = map->images.root; img; img = nextImg )
  {
    nextImg = img->next;
    
    if ( img->image )
      SDL_FreeSurface( img->image );
    
    free( img );
  }
  
  for ( bot = map->bots.root; bot; bot = nextBot )
  {
    nextBot = bot->next;
    
    if ( bot->bot.image )
      SDL_FreeSurface( bot->bot.image );
    
    free( bot );
  }
  
  free( map->clipMap );
}

