#ifndef HELPERS_H
#define HELPERS_H

/* Types */
/* TODO: move to shared.h and use there */
typedef char VarName[64];

/* Tokenizer */
extern bool readToken( FILE* const stream, char* const token );

/* Parser */
extern bool symbol( FILE* stream, char* sym );
extern bool parseInt ( FILE* stream, int* result );
extern bool parseVarName( FILE* stream, char* result );
extern bool parseString( FILE* stream, char* result );

/* botParser */
extern bool parseBots( FILE* stream, BotList* bots, const SDL_PixelFormat* format );
extern bool parseBotFile( FILE* stream, BotList* bots, const SDL_PixelFormat* format );

extern bool loadSaveGame( FILE* stream, GameState* state );

/* mapParser */
extern bool parseMap( FILE* stream, Map* map, const SDL_PixelFormat* format );

extern void menuHandleInput( GameState* state );
extern void menuDraw ( GameState* state );

extern void updateBotClipMap( GameState* state );

/* botActions */
extern void checkCondition( Condition* cond, GameState* state );
#endif
