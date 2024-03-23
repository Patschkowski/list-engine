#include "shared.h"
#include "helpers.h"

bool
symbol( FILE* stream, char* sym )
{
  char   token[512];
  fpos_t pos;
  
  fgetpos( stream, &pos );
  readToken( stream, token );
  
  if ( strcmp( token, sym ) == 0 )
    return true;
    
  fsetpos( stream, &pos );
  return false;
}

bool
parseInt ( FILE* stream, int* result )
{
  char   token[512];
  /*char*  s;*/
  fpos_t pos;
  
  fgetpos( stream, &pos );
  readToken( stream, token );
  
  /*s = token;
  while ( *s )
  {
    if ( ! ( *s >= '0' && *s <= '9') )
    {
      result = 0;
      fsetpos( stream, &pos );
      return false;
    }
    s++;
  }*/

  if ( strspn( token, "0123456789" ) < strlen( token ) )
  {
    fsetpos( stream, &pos );
    return false;
  }
  
  *result = atoi( token );
  return true;
}

bool
parseVarName( FILE* stream, char* result )
{
  /*unsigned int i;
  char         z;*/
  fpos_t       pos;
  
  fgetpos( stream, &pos );
  readToken( stream, result );
  
  /*for ( i = 0; i < strlen( result ); i++ )
  {
    z = result[i];
    if ( ! ( ( z >= '0' && z <= '9') || ( z >= 'a' && z <= 'z') 
        || ( z >= 'A' && z <= 'Z') || z == '_' || z == '.' ) )
    {
      *result = 0;
      fsetpos( stream, &pos );
      return false;
    }
  }*/

  if ( strspn( result, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890_.:" ) < strlen( result ) )
  {
    *result = 0;
    fsetpos( stream, &pos );
    return false;
  }
  
  return true;
}

bool
parseString( FILE* stream, char* result )
{
  return readToken( stream, result );
}
