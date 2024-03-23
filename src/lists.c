#include "shared.h"

SDL_Surface*
imgListFind( ImgList list, const char* name )
{
  struct ImgListNode* node;
  
  node = list.root;
  while ( node )
  {
    if ( strcmp( node->name, name ) == 0 )
      return node->image;
    
    node = node->next;
  }
  
  return NULL;
}



void
imgListPushBack( ImgList* list, const char* name, SDL_Surface* img )
{
  struct ImgListNode** node;
  struct ImgListNode** prev;
  
  node = &list->root;
  prev = NULL;
  
  while ( *node )
  {
    prev = node;
    node = &(*node)->next;
  }
  
  *node = malloc(sizeof * *node);
  if ( !*node )
    exit( EXIT_FAILURE );
  
  (*node)->next  = NULL;
  (*node)->image = img;
  strcpy( (*node)->name, name );
  
  if ( prev )
    (*prev)->next = (*node);
}



void
imgListFree( ImgList list )
{
  struct ImgListNode* node, * next;
  
  /* Iterate over the list and free all memory. */
  node = list.root;
  while ( node )
  {
    /* Save the pointer to the next item. */
    next = node->next;
    
    /* Free the node. */
    SDL_FreeSurface( node->image );
    node->image = NULL;
    free( node );
    
    /* Advance to the next node. */
    node = next;
  }
  
  list.root = NULL;
}



#ifndef NDEBUG
void
imgListPrint( ImgList images )
{
  struct ImgListNode* node;
  
  node = images.root;
  while ( node )
  {
    printf( "name = %s, img = %p, this =%p, next = %p\n",
      node->name, (void*) node->image, (void*) node, (void*) node->next
    );
    node = node->next;
  }
}

void
botListPrint( BotList* botList )
{
  struct BotListNode* node;
  
  node = botList->root;
  while ( node )
  {
    printf( "name = %s, map = %s, this = %p, next = %p\n",
      node->bot.name, node->bot.map, (void*) node, (void*) node->next
    );
    node = node->next;
  }
}
#endif

Bot*
botListFind( BotList* botList, char* botName )
{
  struct BotListNode* node;

  node = botList->root;
  while ( node )
  {
    if ( strcmp( node->bot.name, botName ) == 0 )
      return &node->bot;
    node = node->next;
  }
  return NULL;
}

/*! @see imgListFree() */
void
botListFree( BotList bots )
{
  struct BotListNode* node, * next;
  
  node = bots.root;
  while ( node )
  {
    next = node->next;
    
    free( node );
    
    node = next;
  }
}



void moveBotDepMap ( BotList* targetList, BotList* inputList, char* mapName )
{
  struct BotListNode*    inputNode       =   inputList->root;
  struct BotListNode*    inputNodePrev   =   NULL;
    
  struct BotListNode*    outputNode      =   NULL;
  
  while ( inputNode )
  {
    if ( strcmp ( inputNode->bot.map, mapName ) == 0 )
    {
      
      if ( !targetList->root )
      {
        targetList->root = inputNode;
        outputNode = targetList->root;
      }
      else
      {
        outputNode->next = inputNode;
        outputNode = outputNode->next;
      }
      
      
      if ( inputNodePrev )
        inputNodePrev->next = inputNode->next;
      else
        inputList->root = inputNode->next;
      
      inputNode = inputNode->next;
      outputNode->next = NULL;
    }
    else
    {
      inputNodePrev = inputNode;
      inputNode = inputNode->next;
    }
  }
}

void joinBotLists ( BotList* targetList, BotList* inputList )
{
  struct BotListNode* node;
  
  if ( !targetList->root )
    targetList->root = inputList->root;
  else
  {
    node = targetList->root;
    /* Find last element of targetList */
    while ( node->next )
      node = node->next;
    node->next = inputList->root;
  }
  inputList->root = NULL;
}
