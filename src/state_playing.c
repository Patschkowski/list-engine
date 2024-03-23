/*! @author Robert Oehlmann Felix Patschkowski
@file state_playing.c
Functions to draw and update the game, handle the input while playing. */

#include "shared.h"
#include "helpers.h"

/* Expose the drawDialogue interface to the compiler. */
extern char* drawDialogue( SDL_Surface*, char* );


/* Wraps to deal with drawDialogue. */
static char* nextSentence_ = NULL;
static char* sentence_     = NULL;

/*! @brief Handle input while playing.

@param state Handle to the global game state. */
void
playingHandleInput( GameState* state )
{
  SDL_Event event;
  Point*    pos;
  Map*      map;

  /* Establish shortcuts. */
  pos = &state->player.pos;
  map = state->map;

  /* Deal with all events on the event queue. */
  while ( SDL_PollEvent( &event ) )
  {
    switch ( event.type )
    {
    /* Handle key press. */
    case SDL_KEYDOWN :
      if ( state->player.isTalking )
      {
        /* Advance text display. If talking is over reset the isTalking and lockedBot flags. */
        if ( event.key.keysym.sym == SDLK_SPACE )
        {
          if ( nextSentence_ == NULL ) break;
          /* Has the bot said everything? */
          if ( *nextSentence_ == '\0' )
          {
            sentence_     = NULL;
            nextSentence_ = NULL;
            state->player.isTalking = false;
            state->map->lockedBot = NULL;
          }
          /* Or is there still text to say? */
          else
            sentence_ = nextSentence_;
        }

        break;
      }

      switch ( event.key.keysym.sym )
      {
      case SDLK_ESCAPE :
        state->nextState = NS_GAME_MENU;
        break;

      /* Move the player with collision detection.
      Zeros are reachable areas in the collision map,
      twos are bots and ones are unreachable areas */
      case SDLK_k :
      case SDLK_UP :
        if ( pos->y != 0 &&
          map->clipMap[(pos->y - 1) * map->w + pos->x] == 0
        )
        {
          /* Update the clip map. */
          map->clipMap[pos->y * map->w + pos->x] = 0;
          pos->y--;
          map->clipMap[pos->y * map->w + pos->x] = 3;
        }
        state->player.lrud = V_UP;
        break;

      case SDLK_j :
      case SDLK_DOWN :
        if ( pos->y != map->h - 1 &&
          map->clipMap[(pos->y + 1) * map->w + pos->x] == 0
        )
        {
          map->clipMap[pos->y * map->w + pos->x] = 0;
          pos->y++;
          map->clipMap[pos->y * map->w + pos->x] = 3;
        }
        state->player.lrud = V_DOWN;
        break;

      case SDLK_l :
      case SDLK_RIGHT :
        if ( pos->x != map->w - 1 &&
          map->clipMap[pos->y * map->w + pos->x + 1] == 0
        )
        {
          map->clipMap[pos->y * map->w + pos->x] = 0;
          pos->x++;
          map->clipMap[pos->y * map->w + pos->x] = 3;
        }
        state->player.lrud = V_RIGHT;
        break;

      case SDLK_h :
      case SDLK_LEFT :
        if ( pos->x != 0 &&
          map->clipMap[pos->y * map->w + pos->x - 1] == 0
        )
        {
          map->clipMap[pos->y * map->w + pos->x] = 0;
          pos->x--;
          map->clipMap[pos->y * map->w + pos->x] = 3;
        }
        state->player.lrud = V_LEFT;
        break;

      case SDLK_SPACE :
        /* Check whether the field next to the player is a bot. */
        switch ( state->player.lrud )
        {
        case V_LEFT :
          if ( pos->x != 0 &&
            *(map->lockedBot = &map->clipMap[pos->y * map->w + pos->x - 1]) == 2
          )
          {
            state->player.isTalking = true;
#ifndef NDEBUG
            printf( "Player is now talking.\n" );
#endif
          }
          break;

        case V_RIGHT :
          if ( pos->x != map->w - 1 &&
            *(map->lockedBot = &map->clipMap[pos->y * map->w + pos->x + 1]) == 2
          )
          {
            state->player.isTalking = true;
#ifndef NDEBUG
            printf( "Player is now talking.\n" );
#endif
          }
          break;

        case V_UP :
          if ( pos->y != 0 &&
            *(map->lockedBot = &map->clipMap[(pos->y - 1) * map->w + pos->x]) == 2
          )
          {
            state->player.isTalking = true;
#ifndef NDEBUG
            printf( "Player is now talking.\n" );
#endif
          }
          break;

        case V_DOWN :
          if ( pos->y != map->h - 1 &&
            *(map->lockedBot = &map->clipMap[(pos->y + 1) * map->w + pos->x]) == 2
          )
          {
            state->player.isTalking = true;
#ifndef NDEBUG
            printf( "Player is now talking.\n" );
#endif
          }
          break;

        default :
          fprintf( stderr, "Unknown view detected.\n" );
          break;
        }
        if ( *map->lockedBot != 2 )
          map->lockedBot = NULL;
        break;

      /* Do not react on other pressed keys. */
      default :
        break;
      }
      break;

    case SDL_QUIT :
      state->isRunning = 0;
      break;

    /* Ignore all other events. */
    default :
      break;
    }
  }
}



/*! @brief Update the game.

@param state Handle to the global game state. */
void
playingUpdate( GameState* state )
{
  char                 r;
  struct BotListNode*  bot;
  int                  hasMoved;
  Map*                 map;
  struct GateListNode* gate;

  /* Get a handle to the current map. */
  map = state->map;

  /* Only update bots, if enough time elapsed. */
  if ( SDL_GetTicks() > state->nextMoveTick )
  {
    /* Loop through all the bots and update their position. */
    for ( bot = map->bots.root; bot; bot = bot->next )
    {
      /* Skip non-active bots. */
      if ( bot->bot.state != 1 ) continue;

      if ( &map->clipMap[bot->bot.pos.y * map->w + bot->bot.pos.x] == map->lockedBot )
      {
        if ( sentence_ == NULL )
        {
          joinBotLists( &state->bots, &map->bots );
          checkCondition( bot->bot.condition, state );
          moveBotDepMap( &map->bots, &state->bots, map->file );

          /* Make the bot face the player. */
          switch ( state->player.lrud )
          {
          case V_UP : bot->bot.lrud = V_DOWN; break;
          case V_DOWN : bot->bot.lrud = V_UP; break;
          case V_LEFT : bot->bot.lrud = V_RIGHT; break;
          case V_RIGHT : bot->bot.lrud = V_LEFT; break;
          default : fprintf( stderr, "Unknown view.\n" ); break;
          }
          sentence_     = state->textBase[bot->bot.text];
#ifndef NDEBUG
          printf( "%s: '%s'\n", bot->bot.name, sentence_ );
#endif
          nextSentence_ = NULL;
        }
        continue;
      }

      /* Get the next movement. */
      r        = bot->bot.route[bot->bot.iRoute];
      hasMoved = 0;

      /* w = wait, l = left, r = right, u = up, d = down */
      switch ( r )
      {
      case '\0' :
        break;
      case 'w' :
        hasMoved = 1;
        break;

      case 'l' :
        if ( bot->bot.pos.x > 0 &&
          map->clipMap[bot->bot.pos.y * map->w + bot->bot.pos.x - 1] == 0
        )
        {
          map->clipMap[bot->bot.pos.y * map->w + bot->bot.pos.x] = 0;
          bot->bot.pos.x--;
          map->clipMap[bot->bot.pos.y * map->w + bot->bot.pos.x] = 2;

          bot->bot.lrud = V_LEFT;
          hasMoved = 1;
        }
        break;

      case 'r' :
        if ( bot->bot.pos.x < map->w &&
          map->clipMap[bot->bot.pos.y * map->w + bot->bot.pos.x + 1] == 0
        )
        {
          map->clipMap[bot->bot.pos.y * map->w + bot->bot.pos.x] = 0;
          bot->bot.pos.x++;
          map->clipMap[bot->bot.pos.y * map->w + bot->bot.pos.x] = 2;

          bot->bot.lrud = V_RIGHT;
          hasMoved = 1;
        }
        break;

      case 'u' :
        if ( bot->bot.pos.y > 0 &&
          map->clipMap[(bot->bot.pos.y - 1) * map->w + bot->bot.pos.x] == 0
        )
        {
          map->clipMap[bot->bot.pos.y * map->w + bot->bot.pos.x] = 0;
          bot->bot.pos.y--;
          map->clipMap[bot->bot.pos.y * map->w + bot->bot.pos.x] = 2;

          bot->bot.lrud = V_UP;
          hasMoved = 1;
        }
        break;

      case 'd' :
        if ( bot->bot.pos.y < map->h && 
          map->clipMap[(bot->bot.pos.y + 1) * map->w + bot->bot.pos.x] == 0
        )
        {
          map->clipMap[bot->bot.pos.y * map->w + bot->bot.pos.x] = 0;
          bot->bot.pos.y++;
          map->clipMap[bot->bot.pos.y * map->w + bot->bot.pos.x] = 2;

          bot->bot.lrud = V_DOWN;
          hasMoved = 1;
        }
        break;

      default :
        fprintf( stderr, "Check the route definition of "
          "bot %s. It is not valid!\n", bot->bot.name
        );
        break;
      }

      if ( hasMoved )
      {
        if ( bot->bot.iRoute < strlen( bot->bot.route )-1 )
          bot->bot.iRoute++;
        else
          bot->bot.iRoute = 0u;
      }
    }

    state->nextMoveTick += 500;
  }

  /* Loop through all gates and check whether
  it was activated by the player. */
  for ( gate = map->gates.root; gate; gate = gate->next )
  {
    if ( state->player.pos.x == gate->pos.x &&
      state->player.pos.y == gate->pos.y
    )
    {
      /* Ok, we moved onto a gate. */
#ifndef NDEBUG
      printf( "Activated gate to: %s @ %i,%i\n",
        gate->targetMap, gate->targetPos.x, gate->targetPos.y
      );
#endif
      state->nextMap   = gate;
      state->nextState = NS_LOADING;
    }
  }
}



/*! @brief Draw the game.

@param state Constant handle to the game state,
as it will not be changed during drawing. */
void
playingDraw( const GameState* state )
{
  SDL_Rect            view, botDst, botSrc;
  struct BotListNode* bot;

  /* Compute the current drawing area. */
  view.x = state->player.pos.x * TILESIZE - (VIEW_W - TILESIZE) / 2;
  view.y = state->player.pos.y * TILESIZE - (VIEW_H - TILESIZE) / 2;
  view.w = VIEW_W;
  view.h = VIEW_H;

  if ( !state->map )
  {
    fprintf( stderr, "Error: no map loaded, quit drawing.\n" );
    return;
  }

  /* At first the background is blitted into the window. */
  if ( state->map->bg )
    SDL_BlitSurface( state->map->bg, &view, state->surface, NULL );
#ifndef NDEBUG
  else
    fprintf( stderr, "Error: no background loaded for this map.\n" );
#endif

  /* Now all dynamic objects are blitted into the screen. */
  /* Draw the bots. */
  botDst.w = botDst.h = botSrc.w = botSrc.h = TILESIZE;
  botSrc.y = 0;
  for ( bot = state->map->bots.root; bot; bot = bot->next )
  {
    /* If there is not image, do not proceed. */
    if ( bot->bot.state != 1 || !bot->bot.image )
      continue;

    /* Compute the bot's frame. */
    botSrc.x = TILESIZE * bot->bot.lrud;

    /* Compute the position relative to the view. */
    botDst.x = -(state->player.pos.x - bot->bot.pos.x) * TILESIZE + (VIEW_W - TILESIZE) / 2;
    botDst.y = -(state->player.pos.y - bot->bot.pos.y) * TILESIZE + (VIEW_H - TILESIZE) / 2;

    /* Blit the bot into the final image */
    SDL_BlitSurface( bot->bot.image, &botSrc, state->surface, &botDst );
  }

  /* Draw the player. */
  if ( state->player.image )
  {
    /* Center the player to the view. */
    botDst.x = (VIEW_W - TILESIZE) / 2;
    botDst.y = (VIEW_H - TILESIZE) / 2;

    /* Get the player's frame. */
    botSrc.x = TILESIZE * state->player.lrud;
    botSrc.y = 0;
    botSrc.w = botSrc.h = TILESIZE;

    SDL_BlitSurface( state->player.image, &botSrc, state->surface, &botDst );
  }

    /* At last the foreground is blitted. */
  if ( state->map->fg )
  {
    SDL_SetColorKey( state->map->fg, SDL_TRUE,
      SDL_MapRGB( state->surface->format, 231, 255, 255 )
    );
    SDL_BlitSurface( state->map->fg, &view, state->surface, NULL );
  }
#ifndef NDEBUG
  else
    printf( "This map has no foreground to draw.\n" );
#endif
  /* Draw the text bubble. */
  if ( state->player.isTalking && sentence_ != NULL && *sentence_ != '\0' )
    nextSentence_ = drawDialogue( state->surface, sentence_ );
}
