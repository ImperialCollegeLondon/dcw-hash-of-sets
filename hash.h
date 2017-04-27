/*
 * hash.h: hash storage for C..
 *  an hash is a hash table of (key, value) trees..
 *
 * (C) Duncan C. White, 1996-2008 although it seems longer:-)
 */


typedef struct hash_s *hash;
typedef void *hash_value;
typedef char *hash_key;

typedef void (*hash_printfunc)( FILE *, hash_key, hash_value );
typedef void (*hash_foreachcbfunc)( hash_key, hash_value, void * );
typedef void (*hash_freefunc)( hash_value );
typedef hash_value (*hash_copyfunc)( hash_value );

extern hash hashCreate( hash_printfunc p, hash_freefunc f, hash_copyfunc c );
extern void hashEmpty( hash h );
extern hash hashCopy( hash h );
extern void hashFree( hash h );
extern void hashSet( hash h, hash_key k, hash_value v );
extern int hashPresent( hash h, hash_key k, hash_value * v );
extern hash_value hashFind( hash h, hash_key k );
extern void hashForeach( hash h, hash_foreachcbfunc cb, void * arg );
extern void hashDump( FILE * out, hash h );
extern void hashMetrics( hash h, int * min, int * max, double * avg );
extern int hashMembers( hash h );
extern int hashIsEmpty( hash h );
