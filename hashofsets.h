/*
 * hashofsets.h: a hash from a string to a set of other strings.
 *
 *	Consider some form of "a contains b" information, specifically
 *	"parent a has child b" information, in which a named parent
 *	(eg "a") may contain any number of other named children (eg "b").
 *
 *	we store this as a parent -> set(child) collection.
 *
 * (C) Duncan C. White, May 2017
 */


typedef struct hos_s *hos;

typedef void (*hos_foreachcbfunc)( char *, set, void * );

extern hos hosCreate( void );
extern void hosFree( hos m );
extern void hosAddChild( hos h, char * parent, char * child );
extern void hosRemoveChild( hos m, char * parent, char * child );
extern bool hosIsChild( hos m, char * parent, char * child );
extern void hosDump( FILE * out, hos h );
extern set hosChildren( hos h, char * parent );
extern int hosNParents( hos h );
extern void hosForeach( hos h, hos_foreachcbfunc cb, void * extra );
