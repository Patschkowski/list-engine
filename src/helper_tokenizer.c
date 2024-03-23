#include "shared.h"
#include "helpers.h"
#include <ctype.h>

/*! @brief Read a squence of characters from a @c file into a @c token.
 *
 * @param stream Handle to a file to read from.
 * @param token Reference to a user allocated buffer to read to.
 * @param One on success, zero on failure.
 */
bool
readToken( FILE* const stream, char* const token )
{
  int   c;
  char* s;
  int   isQuoted;
  
  /* Check the arguments. */
  if ( !stream || !token )
    return 0;
  
  /* Skip leading white space characters. */
  do
  {
    c = fgetc( stream );
    if ( feof( stream ) || ferror( stream ) )
      return 0;
  }
  while ( IS_WHITE( c ) );
  
  /* Read all non white space characters into the token. */
  s         = token;
  isQuoted  = c == '\"';
  /* If c is " skip it and read another char */
  if ( isQuoted )
  {
    c = fgetc( stream );
    if ( feof( stream ) || ferror( stream ) )
      return 0;
  }
  
  do
  { 
    if ( !isQuoted ) c = tolower( c );
    *s++ = (char)c;
    
    c    = fgetc( stream );
    if ( c == '\"' )
    {
      isQuoted = 0;
      /* We don't want the " so read another char */
      c = fgetc( stream );
    }
  }
  while (
    (isQuoted || !IS_WHITE( c )) && !feof( stream ) && !ferror( stream )
  );
  
  /* Check whether any character has been read. */
  *s = '\0';
  return *token;
}
