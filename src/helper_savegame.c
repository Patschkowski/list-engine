#include "shared.h"
#include "helpers.h"

/* Print relevant bot data to save file */
/*! @brief Save bot data to file.
 *
 * Save relevant data from all bots to save game file 
 * @param stream Filestream to store the data in
 * @param state Complete status of the game to read data from
 */
void
writeSaveGame ( FILE* stream, GameState* state )
{
  struct BotListNode*    node;    /* Pointer to an list element of the botlist */
  
  /* Assign first element of List */
  node = state->bots.root;
  
  /* Print first line of bot block to file */
  fprintf (stream, "bot_state {\n" );
  
  /* Do for every bot in the gamestate */
  while ( node )
  {
    /* Write bot data to one single file line: name, text, state */
    fprintf ( stream, "  %s %d %d\n", node->bot.name, node->bot.text, node->bot.state );
    
    /* Jump to next bot in List */ 
    node = node->next;
  }
  
  /* Write last line of bot block */
  fprintf (stream, "}\n");
}

bool
parseBotSave( FILE* stream, BotList* bots )
{
  bool    result;
  VarName botName;
  int     botText = 0;
  int     botState = 0;
  Bot*    bot = NULL;

  result = 
    parseVarName( stream, botName ) &&
    parseInt( stream, &botText ) &&
    parseInt( stream, &botState );

  if ( !result )
    return false;
  
  bot = botListFind( bots, botName );
  if ( !bot )
    /* We can't use the information, but parsing worked fine: */
    return true;

  bot->text = botText;
  bot->state = botState;

  return true;
}

bool
parseBotSaves( FILE* stream, BotList* bots )
{
  return ( parseBotSave( stream, bots ) && parseBotSaves( stream, bots ) ) || true;
}

bool
loadSaveGame( FILE* stream, GameState* state )
{
  return
    parseInt( stream, &state->player.pos.x ) &&
    parseInt( stream, &state->player.pos.y ) &&
    parseVarName( stream, state->player.map ) &&
    symbol( stream, "bot_state" ) &&
    symbol( stream, "{" ) &&
    parseBotSaves( stream, &state->bots ) &&
    symbol( stream, "}" );
}
