/*
 * mlistdefns.c: mailing list definitions
 *
 *	the primary data structure in our flattening problem
 *	is a collection of mailing list definitions.  Each one
 *	comprises a mailing listname, and a set of members,
 *	divided for efficiency into basic members and non-basic
 *	members (i.e. the names of other lists):
 *	We represent this information as:
 *		a hash from listname to a pair of member sets
 *
 * (C) Duncan C. White, 2015-2017
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#include "hash.h"
#include "set.h"
#include "mlistdefns.h"


typedef struct pair_s *pair;
struct pair_s
{
	set basicmembers;
	set nonbasicmembers;
};

struct mld_s		/* an mld is basically a hash of pairs of sets. */
{
	hash h;
};

static set basicinfo;	/* stored basic mailnames set, for categorization */


/*
 * mldStoreBasicInfo( basicinfoset );
 *	squirrel away the set of basic mailnames, in order that
 *	we can categorize mailing list members as basic|nonbasic
 */
void mldStoreBasicInfo( set bi )
{
	basicinfo = bi;
}


static void printV( FILE *out, hashkey l, hashvalue v )
{
	pair p = (pair)v;
	fprintf( out, "%s: ", l );
	bool basicnotempty = ! setIsEmpty( p->basicmembers );
	if( basicnotempty )
	{
		fprintf( out, "basic: " );
		setDump( out, p->basicmembers );
	}
	if( ! setIsEmpty( p->nonbasicmembers ) )
	{
		if( basicnotempty ) fprintf( out, ", " );
		fprintf( out, "non-basic: " );
		setDump( out, p->nonbasicmembers );
	}
	fprintf( out, "\n" );
}


static pair newpair( set b, set nb )
{
	pair new = (pair) malloc( sizeof( struct pair_s ) );
	assert( new != NULL );
	new->basicmembers = b;
	new->nonbasicmembers = nb;
	return new;
}


static hashvalue copyV( hashvalue v )
{
	pair p = (pair)v;
	return (hashvalue)
	       newpair( setCopy(p->basicmembers),
		        setCopy(p->nonbasicmembers) );
}


static void freeV( hashvalue v )
{
	pair p = (pair)v;
	setFree( p->basicmembers );
	setFree( p->nonbasicmembers );
	free( v );
}


/*
 * mld m = mldCreate();
 *	Create a hash of mailing list definitions
 */
mld mldCreate( void )
{
	mld new = (mld) malloc( sizeof(struct mld_s));
	assert( new != NULL );
	new->h = hashCreate( &printV, &freeV, &copyV );
	return new;
}


/*
 * mldFree( m );
 *	Free an mld
 */
void mldFree( mld m )
{
	hashFree( m->h );
	free( (void *)m );
}


static void printStr( FILE *out, setkey s )
{
	fprintf( out, "%s,", s );
}


/*
 * mldAddMember( m, listname, member );
 *	Add member to listname, categorising member as basic|nonbasic.
 */
void mldAddMember( mld m, char *listname, char *member )
{
	pair p = (pair)hashFind( m->h, listname );
	if( p==NULL )	/* listname not present in m yet */
	{
		set b  = setCreate( &printStr );
		set nb = setCreate( &printStr );
		p = newpair( b, nb );
		hashSet( m->h, listname, (hashvalue)p );
	}
	/* add member to the correct membership set */
	set s = setIn( basicinfo, member ) ?
			p->basicmembers :
			p->nonbasicmembers;
	setAdd( s, member );
}


/*
 * mldRemoveMember( m, listname, member );
 *	Remove member from listname, categorising member as basic|nonbasic.
 *	Precondition: listname exists in m
 *	Postcondition: listname must be non-empty (i.e. you can't delete
 *		the last member of a list)
 */
void mldRemoveMember( mld m, char *listname, char *member )
{
	pair p = (pair)hashFind( m->h, listname );
	assert( p!=NULL );	/* enforce precondition */
	/* remove member from the correct membership set */
	set s = setIn( basicinfo, member ) ?
		p->basicmembers :
		p->nonbasicmembers;
	setRemove( s, member );
	/* enforce postcondition */
	assert( ! setIsEmpty( p->basicmembers ) ||
		! setIsEmpty( p->nonbasicmembers ) );
}


/*
 * bool ismember = mldIsMember( m, listname, member );
 *	is member currently a member (basic or non-basic) of listname?
 */
bool mldIsMember( mld m, char * listname, char *member )
{
	pair p = (pair)hashFind( m->h, listname );
	if( p==NULL )
	{
		return false;	/* listname not in m */
	}
	/* search for member in the correct membership set */
	set s = setIn( basicinfo, member ) ?
		p->basicmembers :
		p->nonbasicmembers;
	return setIn( s, member );
}


/*
 * mldDump( out, m );
 *	display m to out.
 */
void mldDump( FILE *out, mld m )
{
	hashDump( out, m->h );
}


/*
 * set s = mldBasicMembers( m, listname );
 *	Retrieve listname's set of basic members 
 *	Preconditon: listname exists in m
 */
set mldBasicMembers( mld m, char *listname )
{
	pair p = (pair)hashFind( m->h, listname );
	assert( p!=NULL );	/* enforce precondition */
	return p->basicmembers;
}


/*
 * set s = mldNonBasicMembers( m, listname );
 *	Retrieve listname's set of Nonbasic members 
 *	Preconditon: listname exists in m
 */
set mldNonbasicMembers( mld m, char *listname )
{
	pair p = (pair)hashFind( m->h, listname );
	assert( p!=NULL );	/* enforce precondition */
	return p->nonbasicmembers;
}


/*
 * int n = mldNlists( m );
 *	how many lists does m contain?
 */
int mldNlists( mld m )
{
	return hashMembers( m->h );
}


/*
 * bool allbasic = mldAllBasicMembers( m, listname );
 *	return true iff listname only contains basic members
 *	Preconditon: listname exists in m
 */
bool mldAllBasicMembers( mld m, char *listname )
{
	pair p = (pair)hashFind( m->h, listname );
	assert( p!=NULL );	/* enforce precondition */
	return setIsEmpty( p->nonbasicmembers );
}


/* data passed as callback to foreachwrappercb */
typedef struct { void *extra; mld_foreachcbfunc f; } wrap_pair;

static void foreachwrappercb( hashkey listname, hashvalue v, void *extra )
{
	pair p = (pair)v;
	wrap_pair *g = (wrap_pair *)extra;
	(*g->f)( listname, p->basicmembers, p->nonbasicmembers, g->extra );
}


/*
 * mldForeach( m, cb, extra );
 *	foreach mailing list, call the callback.
 */
void mldForeach( mld m, mld_foreachcbfunc cb, void *extra )
{
	hashForeach( m->h, &foreachwrappercb,
		(void *)&(wrap_pair){.extra = extra, .f = cb} );
		/* nb: this is a Compound Literal Operator. */
		/*     never heard of it?  C99 cleverness.. */
}
