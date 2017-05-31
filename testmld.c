/*
 * testmld.c: test program for the mlistdefns module.
 *
 * (C) Duncan C. White, 2015
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#include "set.h"
#include "hash.h"
#include "mlistdefns.h"



static mld m;


static void myPrint( FILE *out, setkey k )
{
        fprintf( out, "%s,", k );
}


static char *bool2str[] = { "false", "true" };

static void testbool( bool v, bool expected, char *msg )
{
	if( v != expected )
	{
		printf( "T %s: should be %s, was %s: bad\n",
			msg, bool2str[v], bool2str[expected] );
	} else
	{
		printf( "T %s: is %s: ok\n", msg, bool2str[v] );
	}
}


/* data passed as callback to foreach_addto */
typedef struct { char *listname; mld  m; } grumble;

/*
 * foreach_addto( member, &grumble_struct );
 *	given a void * cast pointer to a grumble (containing
 *	a listname and a mld (collection of mailing list defns),
 *	and a member to add, add that member to listname in the mld
 */
static void foreach_addto( setkey member, void *extra )
{
	grumble *g = (grumble *)extra;
	mldAddMember( g->m, g->listname, member );
}


/*
 * expandmembers( m, fromlist, tolist );
 *	given an mld m, an all-basic list fromlist, and a list tolist
 *	which contains fromlist as a non-basic member, expand that
 *	use of fromlist in tolist - replace fromlist (as a member of tolist)
 *	with all fromlist's basic members.
 *
 *	Preconditions: fromlist is the name of an allbasic list in m.
 *		       tolist contains fromlist as a (nonbasic) member.
 */
static void expandmembers( mld m, char *fromlist, char *tolist )
{
	assert( mldAllBasicMembers( m, fromlist ) ); /* enforce precondition */
	assert( mldIsMember( m, tolist, fromlist ) ); /* enforce precondition */

	/* remove fromlist from tolist */
	mldRemoveMember( m,tolist,fromlist );
	assert( ! mldIsMember( m, tolist, fromlist ) ); /* check Remove works! */

	/* add fromlist's basic members to tolist */
	set b = mldBasicMembers( m, fromlist );
	setForeach( b, &foreach_addto,
		(void *)&(grumble){.listname = tolist, .m = m} );
		/* nb: this is a Compound Literal Operator. */
		/*     never heard of it?  C99 cleverness.. */
}


/* extra data in foreach_concat callback is char *label */

/*
 * foreach_concat( member, (void *)label );
 *	given a set member, append that member to label
 */
static void foreach_concat( setkey member, void *extra )
{
	char *label = (char *)extra;
	strcat( label, member );
	strcat( label, "," );
}


/*
 *	makelabel( label, "testname", "listname" );
 *		write (into label, a very long string) a test label
 *		the form testname( listname: members_of_list )
 *	Unchecked Precondition: assume that label is long enough..
 */
static void makelabel( char *label, char *testname, char *listname )
{
	sprintf( label, "%s( %s: ", testname, listname );

	/* add listname's basic members to label */
	set b = mldBasicMembers( m, listname );
	set nb = mldNonbasicMembers( m, listname );
	if( ! setIsEmpty(b) )
	{
		setForeach( b, &foreach_concat, (void *)label );
	}
	/* add listname's nonbasic members to label */
	if( ! setIsEmpty(nb) )
	{
		strcat( label, " nonbasic " );
		setForeach( nb, &foreach_concat, (void *)label );
	}
	strcat( label, " )" );
}


int main( int argc, char **argv )
{
	set basic = setCreate( &myPrint );
	setModify( basic, "+a+b+c+d+e+dcw+ldk+gnb" );
	printf( "basic members: " );
	setDump( stdout, basic );
	printf( "\n" );

	mldStoreBasicInfo( basic );

	m = mldCreate();
	mldAddMember( m, "one", "two" );
	mldAddMember( m, "one", "b" );
	mldAddMember( m, "one", "c" );
	mldAddMember( m, "two", "a" );
	mldAddMember( m, "two", "d" );
	mldAddMember( m, "two", "dcw" );
	mldAddMember( m, "two", "ldk" );
	mldAddMember( m, "three", "one" );
	mldAddMember( m, "three", "two" );
	mldAddMember( m, "three", "e" );

	printf( "lists initially:\n" );
	mldDump( stdout, m );

	if( argc > 1 )
	{
		malloc(strlen(argv[1]));	/* test memory leaks */
	}

	char label[1024];
	makelabel( label, "allbasic", "one" );
	testbool( mldAllBasicMembers( m, "one" ), false, label );

	makelabel( label, "allbasic", "two" );
	testbool( mldAllBasicMembers( m, "two" ), true, label );

	makelabel( label, "allbasic", "three" );
	testbool( mldAllBasicMembers( m, "three" ), false, label );

	/* now expand basic members of all-basic two into one */
	expandmembers( m, "two", "one" );

	printf( "\nafter expansion of two into one, lists are:\n" );

	mldDump( stdout, m );

	makelabel( label, "allbasic", "one" );
	testbool( mldAllBasicMembers( m, "one" ), true, label );
	makelabel( label, "allbasic", "two" );
	testbool( mldAllBasicMembers( m, "two" ), true, label );
	makelabel( label, "allbasic", "three" );
	testbool( mldAllBasicMembers( m, "three" ), false, label );

	/* now expand basic members of all-basic two into three */
	expandmembers( m, "two", "three" );

	printf( "\nafter expansion of two into three, lists are:\n" );

	mldDump( stdout, m );

	makelabel( label, "allbasic", "one" );
	testbool( mldAllBasicMembers( m, "one" ), true, label );
	makelabel( label, "allbasic", "two" );
	testbool( mldAllBasicMembers( m, "two" ), true, label );
	makelabel( label, "allbasic", "three" );
	testbool( mldAllBasicMembers( m, "three" ), false, label );

	/* now expand basic members of all-basic one into three */
	expandmembers( m, "one", "three" );

	printf( "\nafter expansion of one into three, lists are:\n" );

	mldDump( stdout, m );

	makelabel( label, "allbasic", "one" );
	testbool( mldAllBasicMembers( m, "one" ), true, label );
	makelabel( label, "allbasic", "two" );
	testbool( mldAllBasicMembers( m, "two" ), true, label );
	makelabel( label, "allbasic", "three" );
	testbool( mldAllBasicMembers( m, "three" ), true, label );

	mldFree( m );
	setFree( basic );

	exit(0);
	return 0;
}
