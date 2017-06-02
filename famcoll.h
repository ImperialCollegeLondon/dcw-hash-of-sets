/*
 * famcoll.h:
 *
 *	Consider a collection of family information about parents
 *	and their children.
 *
 *	Specifically, we want to associate a non-empty set of children S
 *	with a named parent P.  We will be told (parent P, child C) pairs,
 *	and want to gather together, for each parent, all their children
 *	together.
 *
 *	We store this as a parent -> set(child) collection;
 *      actually, a hash from a string (the parent name) to a set of
 *	other strings (the names of the children of that parent).
 *
 * (C) Duncan C. White, May 2017
 */


typedef struct famcoll_s *famcoll;

typedef void (*famcollforeachcb)( char *, set, void * );

extern famcoll famcollCreate( void );
extern void famcollFree( famcoll m );
extern void famcollAddChild( famcoll h, char * parent, char * child );
extern void famcollRemoveChild( famcoll m, char * parent, char * child );
extern bool famcollIsChild( famcoll m, char * parent, char * child );
extern void famcollDump( FILE * out, famcoll h );
extern set famcollChildren( famcoll h, char * parent );
extern int famcollNFamilies( famcoll h );
extern void famcollForeach( famcoll h, famcollforeachcb cb, void * extra );
