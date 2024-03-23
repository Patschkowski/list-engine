/*! @author Johannes Bornholdt
 * @file state_menu.c
 * file contains als functions for the menus
 */



#include "shared.h"

/* Two calculation helpers to compute the current position in the menu and do the "wrap around" */
#define INC_LIMIT( x, limit ) ( x = ( x < limit ) ? x + 1 : 0 )
#define DEC_LIMIT( x, limit ) ( x = ( x > 0 ) ? x - 1 : limit )


/*! @brief Capture the input from the user and do the actions.
 *
 * Capture the input from keyboard an make changes the menu data in the game state 
 * @param state Complete status of the game to store data in
 */
void
menuHandleInput( GameState* state )
{
  /* Store the event queue */
  SDL_Event event;
  
  /* Short access to struct value */
  int currentMenu = state->menus->current;
  
  /* Fetch every event from the queue */
  while ( SDL_PollEvent( &event ) )
  {
    /* Do actions depending on the type of the event */
    switch ( event.type )
    {
      /* Stop main loop on exit */
      case SDL_QUIT :
        state->isRunning = 0;
        break;
      
      /* Capute key press */
      case SDL_KEYDOWN :
        /* Detect the different keys */
        switch ( event.key.keysym.sym )
        {
          /* Arrow up ist pressed */
          case SDLK_k :
          case SDLK_UP :
            /* Calculate new item to highlight */
            state->menus->data[ currentMenu ].highlightedItem = DEC_LIMIT(state->menus->data[ currentMenu ].highlightedItem, 2 );
          #ifndef NDEBUG
            printf ("Main menu: New highlighted item: %i\n", state->menus->data[currentMenu].highlightedItem);
          #endif
          break;
        
          /* Arrow down is pressed */
          case SDLK_j :
          case SDLK_DOWN :
            /* Calculate new item to highlight */
            state->menus->data[ currentMenu ].highlightedItem = INC_LIMIT(state->menus->data[ currentMenu ].highlightedItem, 2 );
           #ifndef NDEBUG
              printf ("Main menu: New highlighted item: %i\n", state->menus->data[ currentMenu ].highlightedItem);
           #endif
            break;
            
          /* Space ist pressed */
          case SDLK_SPACE :
          case SDLK_RETURN :
            #ifndef NDEBUG
              printf ("Main menu: Selected item: %i\n", state->menus->data[ currentMenu ].highlightedItem);
            #endif
            
            /* Save next state to game state */
            /* Items of nextState are linked to the plain text names of the game states */
            state->nextState = state->menus->data[ currentMenu ].nextState[  state->menus->data[ currentMenu ].highlightedItem  ];
            break;
        
          default :
            break;;
        }
      default:
        break;
    }
  }
  return;
}



/*! @brief Update the menu data
 *
 * Normally the menu data is updated here, but there is nothing to do here
 * @param state Complete status of the game to store data in
 */
void menuUpdate ( GameState* state )
{
  /* Shut up compiler */
  if ( state ) {}
  return;
}



/*! @brief Draw the menu
 *
 * Draw the current menu (saved in MenuState)
 * @param state Complete status of the game to store data in
 */
void menuDraw ( GameState* state )
{
  /* Short access to struct value */
  int currentMenu = state->menus->current;
  
  /* Draw the Background */
  SDL_BlitSurface(state->menus->data[ currentMenu ].bg, NULL, state->surface, NULL );
  
  /* Set the transparent color for the highlight graphics */
  SDL_SetColorKey(state->menus->data[ currentMenu ].hItem[state->menus->data[currentMenu].highlightedItem], SDL_TRUE, SDL_MapRGB( state->surface->format, 231, 255, 255 ) );
  
  /* Draw the foreground to highlight a menu item */
  SDL_BlitSurface(state->menus->data[ currentMenu ].hItem[state->menus->data[currentMenu].highlightedItem], NULL, state->surface, NULL );
  return;
}
