        /*if ( GetComment( InstructionWithComment, Comment ) )
        {
            printf("%sh\n", Comment );
        }*/


        int GetComment( char InstructionWithComment[], char Comment[] )
{
    char *p, str[256];

    strcpy ( str, InstructionWithComment );

    p = strchr( str, '/' );
    if( *(p) == '/' &&  *(p + 1 )  == '/' )
    {
        //printf("%d", strlen ( p ));
       // strcpy( str, p );
       if( strlen( p ) >= 2 )
       {
            *( p + 2 ) = '\0';
       }

        strcpy( Comment, p );
        return( 1 );
    }
    else
    {
        return( 0 );
    }
}
