/*! @file shared.h Project's main include file. */

#ifndef SHARED_H
#define SHARED_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL.h>
#include <SDL_ttf.h>

#define TILESIZE 64
#define GAME_LANGUAGE "de"

#define VIEW_W (TILESIZE * 10)
#define VIEW_H (TILESIZE * 9)

#define TEXT_BASE_SIZE 128
#define TEXT_BASE_TEXT_LENGTH 512

#define TICKS_PER_SECOND 25
#define MAX_FRAMESKIP 10
#define GAME_TICK (1000 / TICKS_PER_SECOND)

/*! @def IS_WHITE Check whether an ascii character is drawable or not. */
#define IS_WHITE( c ) ((c) < 0x21 || (c) == 0x7f)

enum NextState
{
  NS_NONE,
  NS_QUIT,
  NS_LOADING,
  NS_SAVING,
  NS_PLAYING,
  NS_MAIN_MENU,
  NS_GAME_MENU
};



typedef enum bool
{
  false = 0,
  true = 1
} bool;



typedef struct Point
{
  Sint32 x, y;
} Point;

typedef struct Action
{
  char           action[64];
  char           affect[64];
  int            newValue;
  struct Action* next;
} Action;

typedef struct Condition
{
  char    requiredItem[64];
  Action* actions;
} Condition;

typedef struct Bot
{
  char         name[64];
  char         className[64];
  Point        pos;
  char         map[64];
  int          text;
  int          state;
  char         route[128];
  size_t       iRoute;
  Condition*   condition;
  SDL_Surface* image;
  int          lrud; /* left, right, up, down */
} Bot;



struct BotListNode
{
  Bot                 bot;
  struct BotListNode* next;
};

typedef struct BotList
{
  struct BotListNode* root;
} BotList;

Bot*
botListFind( BotList* botList, char* botName );

void
botListFree( BotList bots );

struct ImgListNode
{
  char                name[64];
  SDL_Surface*        image;
  struct ImgListNode* next;
};

typedef struct ImgList
{
  struct ImgListNode* root;
} ImgList;


struct GateListNode
{
  char  targetMap[32];
  Point pos;
  Point targetPos;
  struct GateListNode* next;
};

typedef struct GateList
{
  struct GateListNode* root;
} GateList;

void
imgListPushBack( ImgList* list, const char* name, SDL_Surface* image );

SDL_Surface*
imgListFind( ImgList list, const char* name );

void
imgListFree( ImgList list );

#ifndef NDEBUG
void
imgListPrint( ImgList list );
#endif


typedef struct Map
{
  int          w;
  int          h;
  SDL_Surface* fg;
  SDL_Surface* bg;
  ImgList      images;
  BotList      bots;
  Uint8*       clipMap;
  char         name[128]; /* title */
  GateList     gates;
  Uint8*       lockedBot;
  char         file[128]; /* filename */
} Map;


enum View
{
  V_DOWN = 0, V_UP = 1, V_RIGHT = 2, V_LEFT = 3
};


typedef struct Player
{
  SDL_Surface* image;
  Point        pos;
  char         map[128]; /* filename */
  bool         isTalking;
  enum View    lrud; /* left, right, up, down */
} Player;



typedef struct MenuData
{
  unsigned int highlightedItem;
  SDL_Surface* bg;
  SDL_Surface* hItem[3];
  int          nextState[3];
} MenuData;


typedef struct MenuState
{
  unsigned int  current;
  MenuData      data[2];
} MenuState;


typedef struct GameState
{
  Map*          map;
  BotList       bots;
  char**        textBase;
  int           isRunning;
  MenuState*    menus;
  void        (*draw)( const struct GameState* );
  void        (*update)( struct GameState* );
  void        (*handleInput)( struct GameState* );
  int           nextState;
  struct GateListNode* nextMap;
  SDL_Surface*  surface;
  SDL_Window*   window;
  SDL_Renderer* renderer;
  SDL_Texture*  texture;
  Player        player;
  Uint32        nextMoveTick;
} GameState;

/* TODO: Move this declarations to another file? */
extern void initializeSdl( SDL_Window** const window, SDL_Surface** const surface, SDL_Renderer** const renderer, SDL_Texture** const texture );
extern void initializeTtf();
extern GameState* initGame( void );
void changeState( GameState* );
extern void blitRegion( SDL_Surface* region, int x, int y, int w, int h, SDL_Surface* dst, const SDL_PixelFormat* format );
extern void freeMap( Map* );
extern void moveBotDepMap ( BotList* targetList, BotList* inputList, char* mapName );
extern void botListPrint( BotList* botList );
extern void joinBotLists ( BotList* targetList, BotList* inputList );
extern void writeSaveGame ( FILE* stream, GameState* state );

#endif
