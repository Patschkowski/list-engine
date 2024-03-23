#include "shared.h"
#include "helpers.h"

void
execBotAction( Action* action, GameState* state, char* self )
{
  struct BotListNode* node;
  char*               affect;

  affect = action->affect;
  if ( self && strcmp( affect, ":self:" ) == 0 )
    affect = self;

  node = state->bots.root;

  while ( node )
  {
    /* Apply actions */
    if ( strcmp( node->bot.name, affect ) == 0 )
    {
      if ( strcmp( action->action, "changetext" ) == 0 )
      {
        node->bot.text = action->newValue;
#ifndef NDEBUG
        printf( "Changed text of %s to %d\n", affect, action->newValue );
#endif
      }
      if ( strcmp( action->action, "changestate" ) == 0 )
      {
        state->map->clipMap[node->bot.pos.y * state->map->w + node->bot.pos.x] = 0;
        node->bot.state = action->newValue;
#ifndef NDEBUG
        printf( "Changed state of %s to %d\n", affect, action->newValue );
#endif
      }
    }
    node = node->next;
  }

  return;
}

void
execAllBotActions( Condition* cond, GameState* state, char* self )
{
  Action* action;

  action = cond->actions;
  while ( action )
  {
    execBotAction( action, state, self );
    action = action->next;
  }

  return;
}

void
checkCondition( Condition* cond, GameState* state )
{
  struct BotListNode* node;
  char*               required;
  char                number[10];
  bool                class = false;
  bool                match;
  int                 matchesLeft = 1;
  
  node = state->bots.root;
  required = cond->requiredItem;
  
  if ( strcmp( required, ":none:" ) == 0 )
  {
    execAllBotActions( cond, state, NULL );
    return;
  }

  if ( strncmp( required, ":any:", 5 ) == 0 )
  {
    class = true;
    required += 5 * sizeof( char );
  }
  if ( strncmp( required, ":many:", 6) == 0 )
  {
    class = true;
    required += 6 * sizeof( char );
    
    strncpy( number, required, strcspn( required, ":" ) );
    required += ( strlen( number ) + 1 ) * sizeof( char );
    matchesLeft = atoi( number );
  }

#ifndef NDEBUG
  printf( "now checking condition\n" );
#endif
  while ( node && matchesLeft > 0 )
  {
    if( class )
      match = strcmp( node->bot.className, required ) == 0;
    else
      match = strcmp( node->bot.name, required ) == 0;

    if ( match && node->bot.state == 2 )
      matchesLeft --;

    node = node->next;
  }
  
  if ( matchesLeft == 0 )
    execAllBotActions( cond, state, NULL );
  
  return;
}
