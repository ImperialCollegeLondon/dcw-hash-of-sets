/*
 * hashofsets.h: a hash from a name to a set of members.
 *
 *	consider some form of "a contains b" information, where each
 *	named entity (eg "a") may contain any number of other named
 *	entities (eg "b").
 *
 *	we store this as a name -> set(members) collection.
 *
 * (C) Duncan C. White, May 2017
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#include "hash.h"
#include "set.h"
#include "hashofsets.h"


struct hos_s		/* an hos is simply a hash of sets with some meta-data */
{
	int nentries;
	hash h;
};


static void printV( FILE *out, hashkey name, hashvalue v )
{
	set s = (set)v;
	fprintf( out, "%s: ", name );
	bool empty = setIsEmpty( s );
	if( empty )
	{
		fprintf( out, "empty" );
	} else
	{
		//fprintf( out, "members: " );
		setDump( out, s );
	}
	fprintf( out, "\n" );
}


static hashvalue copyV( hashvalue v )
{
	set s = (set)v;
	return (hashvalue) setCopy(s);
}


static void freeV( hashvalue v )
{
	set s = (set)v;
	setFree( s );
}


/*
 * hos m = hosCreate();
 *	Create a hash of sets.
 */
hos hosCreate( void )
{
	hos new = (hos) malloc( sizeof(struct hos_s));
	assert( new != NULL );
	new->h = hashCreate( &printV, &freeV, &copyV );
	new->nentries = 0;
	return new;
}


/*
 * hosFree( m );
 *	Free a hos
 */
void hosFree( hos m )
{
	hashFree( m->h );
	free( (void *)m );
}


static void printStr( FILE *out, setkey s )
{
	fprintf( out, "%s,", s );
}


/*
 * hosAddMember( m, name, member );
 *	Add member to name.
 */
void hosAddMember( hos h, char *name, char *member )
{
	set s = (set)hashFind( h->h, name );
	if( s==NULL )	/* name not present in m yet */
	{
		s  = setCreate( &printStr );
		hashSet( h->h, name, (hashvalue)s );
		h->nentries++;
	}
	/* add member to the set */
	setAdd( s, member );
}


/*
 * hosRemoveMember( m, name, member );
 *	Remove member from name.
 *	Precondition: name exists in m
 *	Postcondition: name must be non-empty (i.e. you can't delete
 *		the last member of a list)
 */
void hosRemoveMember( hos m, char *name, char *member )
{
	set s = (set)hashFind( m->h, name );
	assert( s!=NULL );	/* enforce precondition */

	/* remove member from the correct membership set */
	setRemove( s, member );

	/* enforce postcondition */
	assert( ! setIsEmpty( s ) );
}


/*
 * bool ismember = hosIsMember( m, name, member );
 *	is member currently a member of name?
 */
bool hosIsMember( hos m, char *name, char *member )
{
	set s = (set)hashFind( m->h, name );
	if( s==NULL )
	{
		return false;	/* name not in m */
	}
	return setIn( s, member );
}


/*
 * hosDump( out, m );
 *	display m to out.
 */
void hosDump( FILE *out, hos h )
{
	hashDump( out, h->h );
	fprintf( out, "There are %d entries\n", h->nentries );
}


/*
 * set s = hosMembers( h, name );
 *	Retrieve name's set of members, nb: not cloned, may be empty set
 *	Precondition: name exists in h
 */
set hosMembers( hos h, char *name )
{
	set s = (set)hashFind( h->h, name );
	assert( s!=NULL );	/* enforce precondition */
	return s;
}


/*
 * int n = hosNentries( m );
 *	how many entries does m contain?
 */
int hosNentries( hos h )
{
	return h->nentries;
}


/* data passed as callback to foreachwrappercb */
typedef struct { void *extra; hos_foreachcbfunc f; } wrap_pair;

static void foreachwrappercb( hashkey name, hashvalue v, void *extra )
{
	set s = (set)v;
	wrap_pair *g = (wrap_pair *)extra;
	(*g->f)( name, s, g->extra );
}


/*
 * hosForeach( h, cb, extra );
 *	foreach entry, call the callback.
 */
void hosForeach( hos h, hos_foreachcbfunc cb, void *extra )
{
	wrap_pair wp;
	wp.extra = extra;
	wp.f = cb;
	hashForeach( h->h, &foreachwrappercb, (void *)&wp );
}
