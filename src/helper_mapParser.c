#include "shared.h"
#include "helpers.h"

bool
parseImage( FILE* stream, Map* map )
{
  VarName     filename;
  char        filepath[128];
  int         x, y;
  VarName     imageId;
  SDL_Surface* img;
  SDL_Rect     src_rect;
  SDL_Surface* dest;
  
#ifndef NDEBUG
  printf( "Now parsing Image\n" );
#endif
  
  if ( !symbol( stream, "image" ) ) return false;
  if ( !parseVarName( stream, filename ) ) return false;
  if ( !parseInt( stream, &x ) ) return false;
  if ( !parseInt( stream, &y ) ) return false;
  if ( !parseVarName( stream, imageId ) ) return false;
  
  /*
   * Bild laden, Ausschnitt wählen und kopieren, in ImageList aufnehmen
   */
  
  /*strcat( filepath, filename );*/
  sprintf( filepath, "images%d/%s", TILESIZE, filename ); 

  img = SDL_LoadBMP ( filepath );
  
  src_rect.x = x * TILESIZE;
  src_rect.y = y * TILESIZE;
  src_rect.w = src_rect.h = TILESIZE;
  
  dest = SDL_CreateRGBSurface( 0, TILESIZE, TILESIZE, 32, 0, 0, 0, 0);
  
  SDL_BlitSurface( img, &src_rect, dest, NULL );
  
#ifndef NDEBUG
  printf( "address of the dest-surface: %p\n", (void*) dest );
#endif
  
  imgListPushBack( &map->images, imageId, dest );
  
  /*
   * BLOCK ENDE
   */
  
#ifndef NDEBUG
  printf( "IMAGE :: Image ID: %10s, Filename: %20s, X: %2d, Y: %2d\n", imageId, filename, x, y );
#endif
  
  return true;
}

bool
parseImages( FILE* stream, Map* map )
{
#ifndef NDEBUG
  printf( "Now parsing Images\n" );
#endif
  if ( parseImage( stream, map ) )
    return parseImages( stream, map );
  else
    return true;
}

bool
parseBgRegion( FILE* stream, Map* map, const SDL_PixelFormat* format )
{
  VarName      imageId;
  int          x, y, w, h, clip;
  SDL_Surface* region;
  int i, j;
  
  if ( !symbol( stream, "region" ) ) return false;
  if ( !parseVarName( stream, imageId ) ) return false;
  if ( !parseInt( stream, &x ) ) return false;
  if ( !parseInt( stream, &y ) ) return false;
  if ( !parseInt( stream, &w ) ) return false;
  if ( !parseInt( stream, &h ) ) return false;
  if ( !parseInt( stream, &clip ) ) return false;
  
#ifndef NDEBUG
  printf( "BG_REGION :: Image ID: %10s, X: %2d, Y: %2d, W: %2d, H: %2d, Clip: %d\n", imageId, x, y, w, h, clip );
#endif
  
  region = imgListFind( map->images, imageId );
#ifndef NDEBUG
  /*imgListPrint( map->images );*/
  if ( region ) printf ( "Image found, now blitting\n" );
#endif
  if ( region != NULL )
    blitRegion( region, x, y, w, h, map->bg, format );

  /* Set the clipping flag. */
  for ( j = y; j < y + h; ++j )
    for ( i = x; i < x + w; ++i )
      map->clipMap[j * map->w + i] = (Uint8)clip;

  return true;
}

bool
parseBgRegions( FILE* stream, Map* map, const SDL_PixelFormat* format )
{
  if ( parseBgRegion( stream, map, format ) )
    return parseBgRegions( stream, map, format );
  else
    return true;
}


bool
parseFgRegion( FILE* stream, Map* map, const SDL_PixelFormat* format )
{
  VarName      imageId;
  int          x, y, w, h;
  SDL_Surface* region;
  
  if ( !symbol( stream, "region" ) ) return false;
  if ( !parseVarName( stream, imageId ) ) return false;
  if ( !parseInt( stream, &x ) ) return false;
  if ( !parseInt( stream, &y ) ) return false;
  if ( !parseInt( stream, &w ) ) return false;
  if ( !parseInt( stream, &h ) ) return false;
  
#ifndef NDEBUG
  printf( "FG_REGION :: Image ID: %10s, X: %2d, Y: %2d, W: %2d, H: %2d\n", imageId, x, y, w, h );
#endif
  
  region = imgListFind( map->images, imageId );
#ifndef NDEBUG
  printf ( "Image found, now printing list\n" );
  imgListPrint( map->images );
  printf ( "now blitting\n" );
#endif
  if ( region != NULL )
    blitRegion( region, x, y, w, h, map->fg, format );
  
  return true;
}

bool
parseFgRegions( FILE* stream, Map* map, const SDL_PixelFormat* format )
{
#if 0
  if ( parseFgRegion( stream, map ) )
    return parseFgRegions( stream, map );
  else
    return true;
#endif
  return ( parseFgRegion( stream, map, format ) && parseFgRegions( stream, map, format ) ) || true;
}

bool
parseGate ( FILE* stream, Map* map )
{
  int     x, y, tx, ty;
  VarName targetMap;
  struct GateListNode* gate, * node, * prev;

  if ( !symbol( stream, "gate" ) ) return false;
  if ( !parseInt( stream, &x ) ) return false;
  if ( !parseInt( stream, &y ) ) return false;
  if ( !parseVarName( stream, targetMap ) ) return false;
  if ( !parseInt( stream, &tx ) ) return false;
  if ( !parseInt( stream, &ty ) ) return false;
  
  /* Append a new game to the list of gates. */
  gate = malloc(sizeof * gate);
  if ( !gate )
    exit( EXIT_FAILURE );
  gate->pos.x = x; gate->pos.y = y;
  gate->targetPos.x = tx; gate->targetPos.y = ty;
  strcpy( gate->targetMap, targetMap );
  gate->next = NULL;
  
  if ( !map->gates.root )
    map->gates.root = gate;
  else
  {
    node = map->gates.root;
    prev = NULL;
    while ( node )
    {
      prev = node;
      node = node->next;
    }
    prev->next = gate;
  }
  
#ifndef NDEBUG
  printf( "GATE :: X: %2d, Y: %2d, Target Map: %12s, X: %2d, Y: %2d\n", x, y, targetMap, tx, ty );
#endif

  return true;
}

bool
parseGates( FILE* stream, Map* map )
{
  if ( parseGate( stream, map ) )
    return parseGates( stream, map );
  else
    return true;
}

bool
parseMapHeader( FILE* stream, Map* map, const SDL_PixelFormat* format )
{
  if ( !parseString( stream, map->name ) ) return false;
  if ( !parseInt( stream, &map->w ) ) return false;
  if ( !parseInt( stream, &map->h ) ) return false;
  
#ifndef NDEBUG
  printf( "MAP :: Name: %s, W: %d, H: %d\n", map->name, map->w, map->h );
#endif
  
  /* Initialize the clip map. */
  map->clipMap = malloc(map->w * map->h * sizeof * map->clipMap);
  if ( !map->clipMap )
    exit( EXIT_FAILURE );
  memset( map->clipMap, 0, map->w * map->h * sizeof *map->clipMap );
  
  map->fg = SDL_CreateRGBSurface( 0, map->w * TILESIZE, map->h * TILESIZE, 32, 0, 0, 0, 0);
  SDL_FillRect( map->fg, NULL, SDL_MapRGB( format, 231, 255, 255 ) );
  map->bg = SDL_CreateRGBSurface( 0, map->w * TILESIZE, map->h * TILESIZE, 32, 0, 0, 0, 0);
  
  return true;
}

bool
parseMap( FILE* stream, Map* map, const SDL_PixelFormat* format )
{
  bool result;
#ifndef NDEBUG
  char token[512];
#endif
  
  result = 
    parseMapHeader( stream, map, format ) &&
    symbol( stream, "images" ) &&
    symbol( stream, "{" ) &&
    parseImages( stream, map ) &&
    symbol( stream, "}" ) &&
    symbol( stream, "background" ) &&
    symbol( stream, "{" ) &&
    parseBgRegions( stream, map, format ) &&
    symbol( stream, "}" ) &&
    symbol( stream, "foreground" ) &&
    symbol( stream, "{" ) &&
    parseFgRegions( stream, map, format ) &&
    symbol( stream, "}" ) &&
    symbol( stream, "gates" ) &&
    symbol( stream, "{" ) &&
    parseGates( stream, map ) &&
    symbol( stream, "}" ) /*&&
    symbol( stream, "bots" ) &&
    symbol( stream, "{" ) &&
    parseBots( stream, &map->bots ) &&
    symbol( stream, "}" )*/;
  
#ifndef NDEBUG
  if (!result)
  {
    if ( readToken( stream, token ) )
      printf( "Next token would be: %s\n", token );
  }
#endif

#ifndef NDEBUG
  {
    int i;
    for ( i = 0; i < map->w * map->h; ++i )
    {
      if ( i % map->w == 0 )
        printf( "\n" );
      printf( "%i ", map->clipMap[i] );
    }
    printf( "\n" );
  }

  if ( map->bots.root )
    printf( "First bot: %s\n", map->bots.root->bot.name );
#endif
  
  return result;
}

void
updateBotClipMap( GameState* state )
{
  struct BotListNode* node;

  node = state->map->bots.root;
  while ( node )
  {
    if (node->bot.state == 1)
      state->map->clipMap[node->bot.pos.y * state->map->w + node->bot.pos.x] = 2;
    node = node->next;
  }
}
