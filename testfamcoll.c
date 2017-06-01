/*
 * testfamcoll.c: test program for the famcoll module.
 *
 * (C) Duncan C. White, May 2017
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#include <set.h>
#include <hash.h>

#include "csvsplit.h"
#include "famcoll.h"


//#define TESTSET


static famcoll f;


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
 * testcontains( f, parent, csvchildren );
 *	test that famcoll f contains an entry for parent, containing
 *	precisely the comma-separated csvchildren
 *	(and no other children).  assumes that child names contain
 *	no commas themselves:-)
 */
void testcontains( famcoll f, char *parent, char *csvchildren )
{
	set s = famcollChildren( f, parent );

	grumble g;
	g.s = s;
	g.nfound = 0;
	g.nincsv = 0;
	sprintf( g.msg, "f(%s) contains %s", parent, csvchildren );

	csvForeach( csvchildren, &testcontains_foundone, (void *)&g );

	sprintf( g.msg, "f(%s) has %d children(s)", parent, g.nincsv );
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

	f = famcollCreate();
	famcollAddChild( f, "one", "a" );
	printf( "added <a> to <one> in f\n" );
	testint( famcollNFamilies(f), 1, "f has 1 family" );

	set s2 = famcollChildren( f, "one" );
	testint( setNMembers(s2), 1, "f[one] has 1 child" );

	testbool( setIn(s2, "a" ), true, "a in f[one]" );
	testbool( setIn(s2, "b" ), false, "b in f[one]" );

	printf( "initial families:\n" );
	famcollDump( stdout, f );

	testcontains( f, "one", "a" );	// test that "one" contains only "a"

	famcollAddChild( f, "one", "b" );
	printf( "added <b> to <one> in f\n" );
	testcontains( f, "one", "a,b" );	// should contain a and b

	famcollAddChild( f, "one", "c" );
	printf( "added <c> to <one> in f\n" );
	testcontains( f, "one", "a,b,c" );	// should contain a,b and c

	famcollAddChild( f, "two", "a" );
	printf( "added <a> to <two> in f\n" );
	testcontains( f, "one", "a,b,c" );
	testcontains( f, "two", "a" );

	famcollAddChild( f, "three", "z" );
	printf( "added <z> to <three> in f\n" );
	testcontains( f, "one", "a,b,c" );
	testcontains( f, "two", "a" );
	testcontains( f, "three", "z" );

	famcollAddChild( f, "two", "d" );
	printf( "added <d> to <two> in f\n" );
	testcontains( f, "one", "a,b,c" );
	testcontains( f, "two", "d,a" );	// order irrelevent
	testcontains( f, "three", "z" );

	printf( "final families:\n" );
	famcollDump( stdout, f );

	famcollFree( f );

	return 0;
}