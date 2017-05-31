/*
 * testhos.c: test program for the hashofsets module.
 *
 * (C) Duncan C. White, May 2017
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#include "set.h"
#include "hash.h"
#include "hashofsets.h"


//#define TESTSET


static hos h;


#if 1
static char *bool2str[] = { "false", "true" };

static void testbool( bool v, bool expected, char *msg )
{
	if( v != expected )
	{
		printf( "T %s: should be %s, was %s: bad\n",
			msg, bool2str[expected], bool2str[v] );
	} else
	{
		printf( "T %s: is %s: ok\n", msg, bool2str[v] );
	}
}
#endif


static void testint( int v, int expected, char *msg )
{
	if( v != expected )
	{
		printf( "T %s: should be %d, was %d: bad\n",
			msg, expected, v );
	} else
	{
		printf( "T %s: is %d: ok\n", msg, v );
	}
}


// a csv foreach func takes a char * (a csv value) and a void *
// (an extra value), and returns nothing

typedef void (*csvforeachfunc)( char *, void * );


/*
 * csvForeach( csvstring, &foreach_callback, (void *)extravalue );
 *	Split csvstring into each comma-separated field, calling the
 *	foreach calback for each comma-separated field, passing the
 *	value and extravalue as parameters to it.
 *	Potentially: a generally useful utility function?
 */
void csvForeach( char *csvstring, csvforeachfunc cb, void *extra )
{
	// strdup modifies it's string..
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
	free( copy );
}


// testcontains called csvForeach and needs to squeeze 2 pieces of
// info into the void *extra parameter: so we use a pointer to this
// shortlived but elegantly named struct:-):
typedef struct { set s; char msg[1024]; int nincsv; int nfound; } grumble;

/*
 * testcontains_foundone( element, &a_grumble_struct );
 *	ok, found element in our csvstring.  we expect
 *	that it IS a child of the grumble struct's set.
 *	let's check.
 */
void testcontains_foundone( char *element, void *extra )
{
	grumble *g = (grumble *)extra;		// extra is really a grumble *
	g->nincsv++;
	if( setIn( g->s, element ) )
	{
		// good: expected element to be in, and it is.
		printf( "T %s: %s is in set: ok\n", g->msg, element );
		g->nfound++;
	} else
	{
		printf( "T %s: %s should be in set but isn't\n",
			g->msg, element );
	}
}


/*
 * testcontains( h, parent, csvchildren );
 *	test that hos h contains an entry for parent, containing
 *	precisely the comma-separated csvchildren
 *	(and no other children).  assumes that child names contain
 *	no commas themselves:-)
 */
void testcontains( hos h, char *parent, char *csvchildren )
{
	set s = hosChildren( h, parent );

	grumble g;
	g.s = s;
	g.nfound = 0;
	g.nincsv = 0;
	sprintf( g.msg, "h(%s) contains %s", parent, csvchildren );

	csvForeach( csvchildren, &testcontains_foundone, (void *)&g );

	sprintf( g.msg, "h(%s) has %d children(s)", parent, g.nincsv );
	testint( g.nfound, g.nincsv, g.msg );
}

#ifdef TESTSET
static void myPrint( FILE *out, char *s )
{
	fprintf( out, "%s,", s );
}
#endif

int main( int argc, char **argv )
{
	if( argc > 1 )
	{
		malloc(strlen(argv[1]));	/* test memory leaks */
	}

	#ifdef TESTSET
	set s = setCreate( &myPrint );
	setAdd( s, "one" );
	printf( "set s = " );
	setDump( stdout, s );
	printf( "\n" );
	testbool( setIn( s, "one" ), true, "one in s" );
	testint( setNMembers(s), 1, "s has 1 entry" );
	#endif

	h = hosCreate();
	hosAddChild( h, "one", "a" );
	printf( "added <a> to <one> in h\n" );
	testint( hosNFamilies(h), 1, "h has 1 family" );

	set s2 = hosChildren( h, "one" );
	testint( setNMembers(s2), 1, "h[one] has 1 child" );

	testbool( setIn(s2, "a" ), true, "a in h[one]" );
	testbool( setIn(s2, "b" ), false, "b in h[one]" );

	printf( "initial families:\n" );
	hosDump( stdout, h );

	testcontains( h, "one", "a" );	// test that "one" contains only "a"

	hosAddChild( h, "one", "b" );
	printf( "added <b> to <one> in h\n" );
	testcontains( h, "one", "a,b" );	// should contain a and b

	hosAddChild( h, "one", "c" );
	printf( "added <c> to <one> in h\n" );
	testcontains( h, "one", "a,b,c" );	// should contain a,b and c

	hosAddChild( h, "two", "a" );
	printf( "added <a> to <two> in h\n" );
	testcontains( h, "one", "a,b,c" );
	testcontains( h, "two", "a" );

	hosAddChild( h, "three", "z" );
	printf( "added <z> to <three> in h\n" );
	testcontains( h, "one", "a,b,c" );
	testcontains( h, "two", "a" );
	testcontains( h, "three", "z" );

	hosAddChild( h, "two", "d" );
	printf( "added <d> to <two> in h\n" );
	testcontains( h, "one", "a,b,c" );
	testcontains( h, "two", "d,a" );	// order irrelevent
	testcontains( h, "three", "z" );

	printf( "final families:\n" );
	hosDump( stdout, h );

	hosFree( h );

	return 0;
}
