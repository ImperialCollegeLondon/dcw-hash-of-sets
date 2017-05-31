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


typedef struct hos_s *hos;

typedef void (*hos_foreachcbfunc)( char *, set, void * );

extern hos hosCreate( void );
extern void hosFree( hos m );
extern void hosAddMember( hos m, char * name, char * member );
extern void hosRemoveMember( hos m, char * name, char * member );
extern bool hosIsMember( hos m, char * name, char * member );
extern void hosDump( FILE * out, hos m );
extern set hosMembers( hos m, char * name );
extern int hosNentries( hos m );
extern void hosForeach( hos m, hos_foreachcbfunc cb, void * extra );
