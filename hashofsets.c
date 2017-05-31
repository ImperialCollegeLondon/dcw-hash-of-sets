/*
 * hashofsets.c: a hash from a string to a set of other strings.
 *
 *	Consider some form of "a contains b" information, specifically
 *	"parent a has child b" information, in which a named parent
 *	(eg "a") may contain any number of other named children (eg "b").
 *
 *	we store this as a parent -> set(child) collection.
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
	int nparents;
	hash h;
};


static void printV( FILE *out, hashkey parent, hashvalue v )
{
	set s = (set)v;
	fprintf( out, "%s: ", parent );
	bool empty = setIsEmpty( s );
	if( empty )
	{
		fprintf( out, "empty" );
	} else
	{
		//fprintf( out, "children: " );
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
	new->nparents = 0;
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
 * hosAddChild( m, parent, child );
 *	Add child to parent.
 */
void hosAddChild( hos h, char *parent, char *child )
{
	set s = (set)hashFind( h->h, parent );
	if( s==NULL )	/* parent not present in m yet */
	{
		s  = setCreate( &printStr );
		hashSet( h->h, parent, (hashvalue)s );
		h->nparents++;
	}
	/* add child to the set */
	setAdd( s, child );
}


/*
 * hosRemoveChild( m, parent, child );
 *	Remove Child from parent's set.
 *	Precondition: parent exists in m
 *	Postcondition: parent must be non-empty (i.e. you can't delete
 *		the last Child of a set)
 */
void hosRemoveChild( hos m, char *parent, char *child )
{
	set s = (set)hashFind( m->h, parent );
	assert( s!=NULL );	/* enforce precondition */

	/* remove child from the correct set of children */
	setRemove( s, child );

	/* enforce postcondition */
	assert( ! setIsEmpty( s ) );
}


/*
 * bool ischild = hosIsChild( m, parent, child );
 *	is child currently a child of parent?
 */
bool hosIsChild( hos m, char *parent, char *child )
{
	set s = (set)hashFind( m->h, parent );
	if( s==NULL )
	{
		return false;	/* parent not in m */
	}
	return setIn( s, child );
}


/*
 * hosDump( out, h );
 *	display h to out.
 */
void hosDump( FILE *out, hos h )
{
	hashDump( out, h->h );
	fprintf( out, "There are %d parents\n", h->nparents );
}


/*
 * set s = hosChildren( h, parent );
 *	Retrieve parent's set of Children, nb: not cloned, may be empty set
 *	Precondition: parent exists in h
 */
set hosChildren( hos h, char *parent )
{
	set s = (set)hashFind( h->h, parent );
	assert( s!=NULL );	/* enforce precondition */
	return s;
}


/*
 * int n = hosNParents( h );
 *	how many parents does h contain?
 */
int hosNParents( hos h )
{
	return h->nparents;
}


/* data passed as callback to foreachwrappercb */
typedef struct { void *extra; hos_foreachcbfunc f; } wrap_pair;

static void foreachwrappercb( hashkey parent, hashvalue v, void *extra )
{
	set s = (set)v;
	wrap_pair *g = (wrap_pair *)extra;
	(*g->f)( parent, s, g->extra );
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
