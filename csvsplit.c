/*
 * csvsplit.c: simple CSV splitting (csvForeach)
 *
 * (C) Duncan C. White, May 2017
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#include "csvsplit.h"


/*
 * csvForeach( csvstring, &foreach_callback, (void *)extravalue );
 *	Split csvstring into each comma-separated field, calling the
 *	foreach calback for each comma-separated field, passing the
 *	value and extravalue as parameters to it.
 *	Potentially: a generally useful utility function?
 */
void csvForeach( char *csvstring, csvforeachcb cb, void *extra )
{
	// strtok modifies it's string, so make a mutable copy..
	char *copy = strdup( csvstring );

	char *first = strtok( copy, "," );
	assert( first != NULL );
	(*cb)( first, extra );
	printf( "debug: csvForeach: found %s\n", first );

	char *next;
	while( (next = strtok(NULL,",")) != NULL )
	{
		printf( "debug: csvForeach: found %s\n", next );
		(*cb)( next, extra );
	}
	// don't forget to..
	free( copy );
}
