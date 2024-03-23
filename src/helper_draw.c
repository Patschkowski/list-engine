#include "shared.h"

char* drawDialogue ( SDL_Surface* screen, char* textString );
char* fillTextLine ( char* textString, char* targetString, int maxLineW, TTF_Font* font );

/*! @brief Draw a waiting screen.
 *
 * Draw a waiting screen with the given text to the given surface 
 * @param screen Surface to blit the loading screen on.
 * @param textString Text to write on the screen.
 */
void drawWaitingScreen( SDL_Surface* screen, char* textString )
{
  TTF_Font*     font;         /* Space for loaded ttf */
  SDL_Color     textColor;    /* Text color */
  SDL_Color     bgColor;      /* Text background color */
  SDL_Surface*  renderedText;
  SDL_Surface*  text;         /* Text surface */
  SDL_Rect      position;     /* Text position */
  int           textW;        /* Text dimension: Width */
  int           blit;
  const char*   error = NULL;
  
  /* Set color of text (old: 255, 128, 0 )*/
  textColor.r = 195;
  textColor.g = 195;
  textColor.b = 195;
  
  /* Set background color (old: 0, 128, 192)*/
  bgColor.r = 0;
  bgColor.g = 0;
  bgColor.b = 0;
  
  /* Fill the screen with the background color */
  SDL_FillRect( screen, NULL, SDL_MapRGB( screen->format, bgColor.r, bgColor.g, bgColor.b ) );
  
  /* Load font */
  font = TTF_OpenFont ( "fonts/waiting_font.ttf", 2 * TILESIZE );
  if ( font == NULL )
  {
    fprintf( stderr, "%s\n", TTF_GetError() );
    exit ( EXIT_FAILURE );
  }
  
  /* Compute dimensions of rendered text */
  TTF_SizeText(font, textString, &textW, NULL );
  
  /* Center text in screen horizontally */
  position.x = ( VIEW_W - textW ) / 2;
  
  /* Center text in screen vertically (baseline oriented) */
  position.y = ( VIEW_H - TTF_FontAscent( font ) ) / 2;
 
  /* Render text to surface */
  renderedText = TTF_RenderText_Shaded (font, textString, textColor, bgColor);
  text = SDL_ConvertSurface(renderedText, screen->format, 0);
  SDL_FreeSurface(renderedText);
  renderedText = NULL;
  
  /* Draw surface */
  blit = SDL_BlitSurface (text, NULL, screen, &position );
  if (0 != blit) {
    error = SDL_GetError();
    fprintf(stderr, "Error blitting text: %s", error);
  }
  
  /* Free surface and close font */
  SDL_FreeSurface (text);
  text = NULL;
  TTF_CloseFont(font); 
  
 #ifndef NDEBUG
  SDL_SaveBMP (screen, "waitingscreen.bmp");
#endif
}



/*! @brief Draw a dialogue box.
 *
 * Draw a dialogue box, wrap the text to the line and draw it.
 * @param screen Surface to blit the loading screen on.
 * @param inputString Text to write on the screen.
 */
char* drawDialogue ( SDL_Surface* screen, char* inputString )
{
  int             canvasColorRGB;           /* SDL compatible color of canvas */
  int             areaColorRGB;             /* SDL compatible color of canvas */
  int             textLineHeight;           /* Height of one text line */
  
  SDL_Color       canvasColor;              /* Color of the canvas */
  SDL_Color       areaColor;                /* Color of the canvas */
  
  char            firstTextLine[256]="";    /* First text line of the box */
  char            secondTextLine[256]="";   /* Second text line of the box */
  
  SDL_Rect        boxCanvasRect;            /* Dimension of the canvas */
  SDL_Rect        boxAreaRect;              /* Dimension of the text area */
  SDL_Rect        textRect1;                /* Dimension of the first text line */
  SDL_Rect        textRect2;                /* Dimension of the second text line */
  
  TTF_Font*       font;                     /* TrueTypeFont for the text */
  
  SDL_Surface*    renderedText1;            /* Surface for the first text line */
  SDL_Surface*    renderedText2;            /* Surface for the second text line */
  SDL_Surface*    text1;                    /* Surface for the first text line, pixel format corrected */
  SDL_Surface*    text2;                    /* Surface for the second text line, pixel format corrected */

  int             blit;                     /* Result of blit operation. */
  const char*     error = NULL;
  
  
  /* Color of canvas */
  canvasColor.r = 0;
  canvasColor.g = 0;
  canvasColor.b = 0;
   
  /* Color of the inner area */
  areaColor.r = 255;
  areaColor.g = 255;
  areaColor.b = 255; 
  
  /* Translate the color of canvas for SDL */
  canvasColorRGB = SDL_MapRGB ( screen->format, canvasColor.r, canvasColor.g, canvasColor.b );
  
  /* Translate the color of the inner area for SDL */
  areaColorRGB = SDL_MapRGB ( screen->format, areaColor.r, areaColor.g, areaColor.b );
  
  /* Load font and end programm on error */
  font = TTF_OpenFont ( "fonts/dialogue_font.ttf", TILESIZE );
  if ( font == NULL )
  {
    fprintf( stderr, "%s\n", TTF_GetError() );
    printf ("Error: Cannot load font file (Line: %i)\n", __LINE__);
    exit ( EXIT_FAILURE );
  }
  
  /* Compute the Height of one text line */
  textLineHeight = TTF_FontLineSkip ( font );
  
  /* Calculate the dimension and position of the canvas */
  boxCanvasRect.w = VIEW_W;
  boxCanvasRect.h = TILESIZE + 2 * textLineHeight;
  boxCanvasRect.x = 0;
  boxCanvasRect.y = VIEW_H - boxCanvasRect.h;
  
  /* Calculate the dimension of the inner area for the text */
  boxAreaRect.w = boxCanvasRect.w - TILESIZE / 2 ;
  boxAreaRect.h = boxCanvasRect.h - TILESIZE / 2;
  boxAreaRect.x = boxCanvasRect.x + TILESIZE / 4;
  boxAreaRect.y = boxCanvasRect.y + TILESIZE / 4;
  
  /* Determine the space for the first text line */
  textRect1.w = boxCanvasRect.w - TILESIZE;
  textRect1.h = textLineHeight;
  textRect1.x = boxCanvasRect.x + TILESIZE / 2;
  textRect1.y = boxCanvasRect.y + TILESIZE / 2;
  
  /* Determine the space for the second text line */
  textRect2.w = boxCanvasRect.w - TILESIZE;
  textRect2.h = textLineHeight;
  textRect2.x = boxCanvasRect.x + TILESIZE / 2;
  textRect2.y = boxCanvasRect.y + TILESIZE / 2 + textLineHeight;
  
  /* Fetch text for first ext line */
  inputString = fillTextLine( inputString, firstTextLine, textRect1.w, font );
 
  /* Fetch text for second text line only if there is a text left */
  if ( strlen( inputString ) > 1)
    inputString = fillTextLine( inputString, secondTextLine, textRect2.w, font );
  
  /* Render both text lines to surfaces */
  renderedText1 = TTF_RenderText_Shaded ( font, firstTextLine, canvasColor, areaColor );
  text1 = SDL_ConvertSurface( renderedText1, screen->format, 0 );
  SDL_FreeSurface( renderedText1 );
  renderedText1 = NULL;

  renderedText2 = TTF_RenderText_Shaded ( font, secondTextLine, canvasColor, areaColor );
  text2 = SDL_ConvertSurface( renderedText2, screen->format, 0 );
  SDL_FreeSurface( renderedText2 );
  renderedText2 = NULL;
  
  /* Close font */
  TTF_CloseFont( font );
  font = NULL;

  /* Draw Canvas for dialogue box */
  SDL_FillRect( screen, &boxCanvasRect, canvasColorRGB );
 
  /* Draw inner area of dialogue box */
  SDL_FillRect( screen, &boxAreaRect, areaColorRGB );
  
  /* Blit both texts to the screen */
  blit = SDL_BlitSurface ( text1, NULL, screen, &textRect1 );
  if (0 != blit) {
    error = SDL_GetError();
    fprintf(stderr, "Blitting font failed (1st line): %s\n", error);
  }

  if (text2) {
    blit = SDL_BlitSurface(text2, NULL, screen, &textRect2);
    if (0 != blit) {
      error = SDL_GetError();
      fprintf(stderr, "Blitting font failed (2nd line): %s\n", error);
    }
  }

  /* Free the surfaces */
  SDL_FreeSurface( text1 );
  text1 = NULL;
  SDL_FreeSurface( text2 );
  text2 = NULL;
  
  return inputString;
}



/*! @brief Fit a text to a given width.
 *
 * Fit the given text word by word to the given width.
 * @param inputString Input string to handle.
 * @param targetString The handled text.
 * @param maxLineW Width of the line to fit the text to.
 * @param font Font to calculate the width of the text with.
 */
char* fillTextLine ( char* inputString, char* targetString, int maxLineW, TTF_Font* font )
{
  char*       nextWord;                 /* Cache for next word */
  char*       s;                        /* Pointer to nextWord */
  int         nextWordW, textLineW;
  
  /* Allocate memory for caching of next word */
  nextWord = malloc(255 * sizeof(char));
  if ( !nextWord )
    exit( EXIT_FAILURE );
  
  /* Fit as many words as possible to one text line */
  do
  {
    /* Clear memory for nextWord to avoid problems */
    memset ( nextWord, 0, 255 * sizeof ( *nextWord ) );
    
    /* Save pointer to next word */
    s = nextWord;
    
    /* Save following part of the input string until a non-printable char appears */
    do {
      *s++ = *inputString++;
    } while ( ! IS_WHITE(*inputString) && *inputString );
    
    /* Calculate width of the already handled text rendered with the given font */
    TTF_SizeText (font, targetString, &textLineW, NULL);
    
    /* Calculate width of the new next word rendered with the given font */
    TTF_SizeText (font, nextWord, &nextWordW, NULL);
    
    /* If both strings fits to the line concatenate them and add a space */
    if ( textLineW + nextWordW <= maxLineW )
    {
      strcat ( targetString, nextWord );
      strcat ( targetString, " ");
    }
    /* If text would be to long move the pointer back to the position before the last handled word */
    else
      inputString -= ( 1 + strlen( nextWord ) ) * sizeof( char );
    
    /* Cancel this function here if there is no more text to handle */
    if ( *inputString == 0 ) break;
    
    /* Skip the space after the word */
    inputString++;
  } 
  while ( textLineW + nextWordW <= maxLineW );
  
  /* Free allocated memory */
  free( nextWord );

  /* Return the shifted pointer */
  return inputString;
}
