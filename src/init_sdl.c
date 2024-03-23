/*! @author Felix Patschkowski
@file init_sdl.c Functions to load libraries used by the engine. */

#include "shared.h"

/*! @brief Initialize Simple DirectMedia Layer

@param[out] surface Applications window. */
void
initializeSdl( SDL_Window** const window, SDL_Surface** const surface, SDL_Renderer** const renderer, SDL_Texture** const texture )
{
  SDL_version runTime;
  SDL_version compileTime;

  /* A valid pointer is required. */
  if ( surface == NULL )
  {
    fprintf( stderr, "Invalid pointer.\n" );
    exit( EXIT_FAILURE );
  }

  /* Initialize SDL with video support. */
  if ( SDL_Init( SDL_INIT_VIDEO ) == -1 )
  {
    fprintf( stderr, "%s\n", SDL_GetError() );
    exit( EXIT_FAILURE );
  }

  /* Make our program call SDL_Quit() at exit. */
  if ( atexit( &SDL_Quit ) != 0 )
  {
    fprintf( stderr, "Failed to register SDL_Quit() with at exit stack.\n" );
    SDL_Quit();
    exit( EXIT_FAILURE );
  }

  /* Check integrity of compile and run time versions of SDL. */
  SDL_VERSION( &compileTime );
  SDL_GetVersion( &runTime );
  if ( compileTime.major == runTime.major )
  {
    if ( compileTime.minor != runTime.minor )
      fprintf( stderr, "SDL's compile (%u.%u.%u) and run "
        "time versions (%u.%u.%u) differ\n",
        compileTime.major, compileTime.minor, compileTime.patch,
        runTime.major, runTime.minor, runTime.patch
      );
  }
  else
  {
    fprintf( stderr, "SDL's compile (%u.%u.%u) and run time versions "
      "(%u.%u.%u) differ to much to guarantee a stable execution.\n",
      compileTime.major, compileTime.minor, compileTime.patch,
      runTime.major, runTime.minor, runTime.patch
    );
    exit( EXIT_FAILURE );
  }

  /* Style the window's appearance. */
  *window = SDL_CreateWindow(
    "IPPP732", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, VIEW_W, VIEW_H, SDL_WINDOW_SHOWN
  );
  if (*window == NULL)
  {
    fprintf(stderr, "%s\n", SDL_GetError());
    exit(EXIT_FAILURE);
  }

  /* Create the renderer. */
  *renderer = SDL_CreateRenderer(*window, -1, 0);
  if (*renderer == NULL)
  {
    fprintf(stderr, "%s\n", SDL_GetError());
    exit(EXIT_FAILURE);
  }

  /* Create and validate the main drawing area. */
  *surface = SDL_GetWindowSurface(*window);
  if ( *surface == NULL )
  {
    fprintf( stderr, "%s\n", SDL_GetError() );
    exit( EXIT_FAILURE );
  }

  /* Create the texture. */
  *texture = SDL_CreateTexture(*renderer, (*surface)->format->format, SDL_TEXTUREACCESS_STREAMING, VIEW_W, VIEW_H);
  if ( *texture == NULL )
  {
    fprintf(stderr, "%s\n", SDL_GetError());
    exit(EXIT_FAILURE);
  }

  /* Ignore certain events. */
  SDL_EventState( SDL_JOYAXISMOTION, SDL_IGNORE );
  SDL_EventState( SDL_JOYBALLMOTION, SDL_IGNORE );
  SDL_EventState( SDL_JOYHATMOTION,  SDL_IGNORE );
  SDL_EventState( SDL_JOYBUTTONDOWN, SDL_IGNORE );
  SDL_EventState( SDL_JOYBUTTONUP,   SDL_IGNORE );
}


/*! @brief Initialize SDL's truetype font library.

*/
void
initializeTtf()
{
  const SDL_version* runTime;
  SDL_version        compileTime;

  /* Initialize TTF. */
  if ( TTF_Init() != 0 )
  {
    fprintf( stderr, "%s\n", TTF_GetError() );
    exit( EXIT_FAILURE );
  }

  /* Make sure TTF is closed at the end of the program. */
  if ( atexit( &TTF_Quit ) != 0 )
  {
    TTF_Quit();
    fprintf( stderr, "Failed to register TTF_Quit() with at exit stack.\n" );
    exit( EXIT_FAILURE );
  }

  /* Check integrity of compile and run time versions of TTF. */
  TTF_VERSION( &compileTime );
  runTime = TTF_Linked_Version();
  if ( compileTime.major == runTime->major )
  {
    if ( compileTime.minor != runTime->minor )
      fprintf( stderr, "SDL_ttf's compile (%u.%u.%u) and run "
        "time versions (%u.%u.%u) differ\n",
        compileTime.major, compileTime.minor, compileTime.patch,
        runTime->major, runTime->minor, runTime->patch
      );
  }
  else
  {
    fprintf( stderr, "SDL_ttf's compile (%u.%u.%u) and run time versions "
      "(%u.%u.%u) differ to much to guarantee a stable execution.\n",
      compileTime.major, compileTime.minor, compileTime.patch,
      runTime->major, runTime->minor, runTime->patch
    );
    exit( EXIT_FAILURE );
  }
}


