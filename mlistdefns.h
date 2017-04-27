/*
 * mlistdefns.h: mailing list definitions
 *
 *	the primary data structure in our flattening problem
 *	is a collection of mailing list definitions.  Each one
 *	comprises a mailing listname, and a set of members,
 *	divided for efficiency into basic members and non-basic
 *	members (i.e. the names of other lists):
 *	We represent this information as:
 *		a hash from listname to a pair of member sets
 *
 * (C) Duncan C. White, 2015
 */


typedef struct mld_s *mld;

typedef void (*mld_foreachcbfunc)( char *, set, set, void * );

extern void mldStoreBasicInfo( set bi );
extern mld mldCreate( void );
extern void mldFree( mld m );
extern void mldAddMember( mld m, char * listname, char * member );
extern void mldRemoveMember( mld m, char * listname, char * member );
extern bool mldIsMember( mld m, char * listnamea, char *member );
extern void mldDump( FILE * out, mld m );
extern set mldBasicMembers( mld m, char * listname );
extern set mldNonbasicMembers( mld m, char * listname );
extern int mldNlists( mld m );
extern bool mldAllBasicMembers( mld m, char * listname );
extern void mldForeach( mld m, mld_foreachcbfunc cb, void * extra );
