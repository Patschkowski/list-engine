#include "shared.h"
#include "helpers.h"

bool
parseAction( FILE* stream, Condition* cond )
{
  VarName action;
  VarName bot;
  int     newValue;
  Action* newAction;
  Action* node;
  Action* prev;
  
  if ( !parseVarName( stream, action ) ) return false;
  if ( !parseVarName( stream, bot ) ) return false;
  if ( !parseInt( stream, &newValue ) ) return false;

  /* Allocate mem for action and append to condition */
  newAction = malloc(sizeof * newAction);
  if ( !newAction )
    exit( EXIT_FAILURE );
  memset( newAction, 0, sizeof *newAction );

  strcpy( newAction->action, action );
  strcpy( newAction->affect, bot );
  newAction->newValue = newValue;

  if ( !cond->actions)
    cond->actions = newAction;
  else
  {
    node = cond->actions;
    prev = NULL;
    while ( node )
    {
      prev = node;
      node = node->next;
    }
    prev->next = newAction;
  }

  return true;
}

bool
parseActions( FILE* stream, Condition* cond )
{
  if ( parseAction( stream, cond ) )
    return parseActions( stream, cond );
  else
    return true;
}

bool
parseCondition( FILE* stream, Bot* bot )
{
  bool       result;
  Condition* cond;
  
  /* Basic check, so we don't allocate mem all for nothing */
  if ( !symbol( stream, "condition" ) ) return false;
  
  /* Now we know it's a condition, alloc mem */
  cond = malloc(sizeof * cond);
  if ( !cond )
    exit( EXIT_FAILURE );
  memset( cond, 0, sizeof *cond );
  
  result = 
    parseVarName( stream, cond->requiredItem ) &&
    symbol( stream, "{" ) &&
    parseActions( stream, cond ) &&
    symbol( stream, "}" );

  if ( !result )
    return false;

  /* Attach to bot instead of freeing */
  bot->condition = cond;

  return true;
}

bool
parseBotPosition( FILE* stream, Bot* bot )
{
  return
    symbol( stream, "position" ) &&
    parseVarName( stream, bot->map ) &&
    parseInt( stream, &bot->pos.x ) &&
    parseInt( stream, &bot->pos.y );
}

bool
parseBotText( FILE* stream, Bot* bot )
{
  return
    symbol( stream, "text" ) &&
    parseInt( stream, &bot->text );
}

bool
parseBotRoute( FILE* stream, Bot* bot )
{
  return
    symbol( stream, "route" ) &&
    parseVarName( stream, (char*) &bot->route );
}

bool
parseBotImage( FILE* stream, Bot* bot, const SDL_PixelFormat* format )
{
  bool    result;
  VarName filename;
  char    filePath[128];
  SDL_Surface* img;
  SDL_Rect     src_rect;
  
  result =
    symbol( stream, "image" ) &&
    parseVarName( stream, filename );

  if ( !result )
    return false;

  /* Bild laden und beim Bot ablegen */
  sprintf ( filePath, "images%d/%s", TILESIZE, filename );

#ifndef NDEBUG
  printf( "BOT:IMAGE :: Image: %s\n", filePath );
#endif

  img = SDL_LoadBMP ( filePath );
  
  src_rect.x = src_rect.y = 0;
  src_rect.w = TILESIZE * 4;
  src_rect.h = TILESIZE;
  
  bot->image = SDL_CreateRGBSurface( 0, TILESIZE * 4, TILESIZE, 32, 0, 0, 0, 0);

  SDL_BlitSurface( img, &src_rect, bot->image, NULL );

  SDL_SetColorKey( bot->image, SDL_TRUE,
      SDL_MapRGB( format, 231, 255, 255 )
  );

  return true;
}

bool
parseBotProperty( FILE* stream, Bot* bot, const SDL_PixelFormat* format )
{
  return
    parseBotPosition( stream, bot ) ||
    parseBotText    ( stream, bot ) ||
    parseBotRoute   ( stream, bot ) ||
    parseBotImage   ( stream, bot, format ) ||
    parseCondition  ( stream, bot );
}

bool
parseBotProperties( FILE* stream, Bot* bot, const SDL_PixelFormat* format )
{
  if ( parseBotProperty ( stream, bot, format ) )
    return parseBotProperties( stream, bot, format );
  else
    return true;
}

bool
parseBot( FILE* stream, BotList* bots, const SDL_PixelFormat* format )
{
  bool    result;
  VarName botName;
  Bot*    bot;
  struct BotListNode* newNode;
  struct BotListNode* node, * prev;
#ifndef NDEBUG
  Action* action;
#endif

  /* Basic check, so we don't allocate mem unnecessarily */
  if ( !parseVarName( stream, botName ) ) return false;

  /* Allcote memory for the new bot */
  newNode = malloc(sizeof * newNode);
  if ( !newNode )
    exit( EXIT_FAILURE );
  memset( newNode, 0, sizeof *newNode );
  bot = &newNode->bot;

  /* Default state of a bot is active (=1) */
  bot->state = 1;

  /* Do the actual parsing */
  result = 
    symbol( stream, "{" ) &&
    parseBotProperties( stream, bot, format ) &&
    symbol( stream, "}" );

  if ( !result )
  {
    /* Uh, something is wrong in the bot definition,
       lets cancle this, but we need to free memory first */
    free( newNode);
    return false;
  }

  /* Copy the name from temporary variable to bot */
  strcpy( bot->name, botName );

#ifndef NDEBUG
  printf( "BOT :: Name: %s, X: %2d, Y: %2d, Text: %d", bot->name, bot->pos.x, bot->pos.y, bot->text );
  if ( strlen( bot->route ) > 0 )
    printf( ", Route: %s", bot->route );
  printf( "\n" );
  if ( bot->condition )
  {
    printf( "BOT:CONDITION :: Required: %s\n", bot->condition->requiredItem );
    action = bot->condition->actions;
    while( action )
    {
      printf( "BOT:CONDITION:ACTION :: Do: %s, to: %s, new value: %d\n", action->action, action->affect, action->newValue );
      action = action->next;
    }
  }
#endif

  /* Now we can append our new bot to the bot list */
  if ( !bots->root )
    bots->root = newNode;
  else
  {
    node = bots->root;
    prev = NULL;
    while ( node )
    {
      prev = node;
      node = node->next;
    }
    prev->next = newNode;
  }
  
  return true;
}

bool
parseBots( FILE* stream, BotList* bots, const SDL_PixelFormat* format )
{
  return ( parseBot( stream, bots, format ) && parseBots( stream, bots, format ) ) || true;
}

bool
parseBotFile( FILE* stream, BotList* bots, const SDL_PixelFormat* format )
{
  return 
    symbol( stream, "bots" ) && symbol( stream, "{" ) 
    && parseBots( stream, bots, format ) 
    && symbol( stream, "}" );
}
