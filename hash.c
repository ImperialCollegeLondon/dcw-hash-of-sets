/*
 * hash.c: hash storage for C.. new version of assoc.c with cooler
 * 	   name and more features (copying, depth metrics etc)
 * 	   we store a hash as a hash table, hashing each key into
 * 	   a dynamic array of binary search trees, and then
 * 	   search/include/exclude the key,value pair in/from the
 * 	   corresponding search tree.  The hash also stores 3 func
 * 	   ptrs: a (file,key,value) print function, a value free function,
 * 	   and a value copy function.  These enable you to use values
 * 	   that are themselves complex data structures.
 *
 * (C) Duncan C. White, 1996-2013 although it seems longer:-)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <mem.h>

#include "hash.h"


#define	NHASH	32533


typedef struct tree_s *tree;


struct hash_s {
	tree *		data;			/* dynamic array of trees */
	hash_printfunc	p;			/* how to print (k,v) pair */
	hash_freefunc	f;			/* how to free a value  */
	hash_copyfunc	c;			/* how to copy a value  */
};

struct tree_s {
	hash_key	k;			/* Key */
	hash_value   	v;			/* Value */
	tree		left;			/* Left... */
	tree		right;			/* ... and Right trees */
};


/*
 * operation
 */
typedef enum { Search, Define } tree_operation;


/* Private functions */

static void foreach_tree( tree, hash_foreachcbfunc, void * );
static void dump_cb( hash_key, hash_value, void * );
static void free_tree( tree, hash_freefunc );
static void freevalue( hash_freefunc, hash_value );
static tree copy_tree( tree, hash_copyfunc );
static int depth_tree( tree );
static tree tree_op( hash, hash_key, hash_value, tree_operation );
static tree talloc( hash_key, hash_value );
static int shash( char * );


/*
 * Create an empty hash
 */
hash hashCreate( hash_printfunc p, hash_freefunc f, hash_copyfunc c )
{
	int  i;
	hash h;

	h = (hash) malloc( sizeof(struct hash_s) );

	h->data = (tree *) malloc( NHASH*sizeof(tree) );

	h->f = f;
	h->p = p;
	h->c = c;

	for( i = 0; i < NHASH; i++ )
	{
		h->data[i] = NULL;
	}

	return h;
}


/*
 * Empty an existing hash - ie. retain only the skeleton..
 */
void hashEmpty( hash h )
{
	int   i;

	for( i = 0; i < NHASH; i++ )
	{
		if( h->data[i] != NULL )
		{
			free_tree( h->data[i], h->f );
			h->data[i] = NULL;
		}
	}
}


/*
 * Copy an existing hash, including copying the values
 */
hash hashCopy( hash h )
{
	int   i;
	hash   result;

	result = (hash) malloc( sizeof(struct hash_s) );
	result->data = (tree *) malloc( NHASH*sizeof(tree) );
	result->p = h->p;
	result->f = h->f;
	result->c = h->c;

	for( i = 0; i < NHASH; i++ )
	{
		result->data[i] = NULL;
		if( h->data[i] != NULL )
		{
			result->data[i] = copy_tree( h->data[i], h->c );
		}
	}

	return result;
}


/*
 * Free the given hash - clean it up and delete it's skeleton too..
 */
void hashFree( hash h )
{
	int   i;

	for( i = 0; i < NHASH; i++ )
	{
		if( h->data[i] != NULL )
		{
			free_tree( h->data[i], h->f );
		}
	}

	free( (hash_value) h->data );
	free( (hash_value) h );
}


/*
 * Add k->v to the hash array a
 */
void hashSet( hash h, hash_key k, hash_value v )
{
	(void) tree_op( h, k, v, Define);
}


/*
 * Is the key present in the hash a?  if so, write
 * it's value into *value (this is like hashFind()
 * except that this can distinguish between value NULL
 * and value 0 (present)!
 */
int hashPresent( hash h, hash_key k, hash_value *v )
{
	tree x = tree_op(h, k, 0, Search);
	if( x == NULL )
	{
		*v = (hash_value)-1;
		return 0;
	}
	*v = x->v;
	return 1;
}


/*
 * Look for something in the hash h
 * return NULL if it's not present
 */
hash_value hashFind( hash h, hash_key k )
{
	tree x = tree_op(h, k, 0, Search);

	return ( x == NULL ) ? (hash_value) NULL : x->v;
}


/*
 * perform a foreach operation over a given hash array
 * call a given callback for each (name, value) pair.
 */
void hashForeach( hash h, hash_foreachcbfunc cb, void * arg )
{
	int	i;

	for( i = 0; i < NHASH; i++ ) {
		if( h->data[i] != NULL )
		{
			foreach_tree( h->data[i], cb, arg );
		}
	}
}


/* ----------- Higher level operations using setForeach -------------- */
/* - each using it's own callback, sometimes with a custom structure - */


/*
 * hashDump: Display a given hash - print each name and value
 *  by calling the hash's printfunc (or a default if NULL)
 *  Here, we need to know where (FILE *out) and how (hash_printfunc p) to
 *  display each item of the set.
 */
typedef struct { FILE *out; hash_printfunc p; } dumparg;
static void dump_cb( hash_key k, hash_value v, void * arg )
{
	dumparg *dd = (dumparg *)arg;
	if( dd->p != NULL )
	{
		(*(dd->p))( dd->out, k, v );
	} else
	{
		fprintf( dd->out, "%20s -> %08lx\n", k, (long) v );
	}
}
void hashDump( FILE *out, hash h )
{
	dumparg arg;
	arg.p = h->p;
	arg.out = out;

	fputc('\n',out);
	hashForeach( h, &dump_cb, (void *)&arg );
	fputc('\n',out);
}


/*
 * Allocate a new node in the tree
 */
static tree talloc( hash_key k, hash_value v )
{
	tree   p = (tree) malloc(sizeof(struct tree_s));

	if( p == NULL )
	{
		fprintf( stderr, "talloc: No space left\n" );
		exit(1);
	}
	p->left = p->right = NULL;
	p->k = strdup(k);		/* Save key */
	p->v = v;			/* value */
	return p;
}


/*
 * Hash metrics:
 *  calculate the min, max and average depth of all non-empty trees
 *  sadly can't do this with a hashForeach unless the depth is magically
 *  passed into the callback..
 */
void hashMetrics( hash h, int *min, int *max, double *avg )
{
	int	i;
	int	nonempty = 0;
	int	total    = 0;

	*min =  100000000;
	*max = -100000000;
	for( i = 0; i < NHASH; i++ ) {
		if( h->data[i] != NULL )
		{
			int d = depth_tree( h->data[i] );
			if( d < *min ) *min = d;
			if( d > *max ) *max = d;
			total += d;
			nonempty++;
		}
	}
	*avg = ((double)total)/(double)nonempty;
}


/*
 * Operate on the binary search tree
 * Search, Define.
 */
static tree tree_op( hash h, hash_key k, hash_value v, tree_operation op )
{
	tree	ptr;
	tree *	aptr = h->data + shash(k);

	while( (ptr = *aptr) != NULL )
	{
		int rc = strcmp(ptr->k, k);
		if( rc == 0 )
		{
			if (op == Define)
			{
				/* free old value */
				freevalue( h->f, ptr->v );
				/* set new value */
				ptr->v = v;
			}
			return ptr;
		}
		if (rc < 0)
		{
			/* less - left */
			aptr = &(ptr->left);
		} else
		{
			/* more - right */
			aptr = &(ptr->right);
		}
	}

	if (op == Define)
	{
		return *aptr = talloc(k,v);	/* Alloc new node */
	}

	return NULL;				/* not found */
}


/*
 * Copy one tree
 */
static tree copy_tree( tree t, hash_copyfunc c )
{
	tree result = NULL;
	if( t )
	{
		hash_value v = c != NULL ? (*c)(t->v) : t->v;
		result = talloc( t->k, v );
		result->left  = copy_tree( t->left, c );
		result->right = copy_tree( t->right, c );
	}
	return result;
}


/*
 * foreach one tree
 */
static void foreach_tree( tree t, hash_foreachcbfunc f, void * arg )
{
	assert( f != NULL );

	if( t )
	{
		if( t->left ) foreach_tree( t->left, f, arg );
		(*f)( t->k, t->v, arg );
		if( t->right ) foreach_tree( t->right, f, arg );
	}
}


/*
 * Free one tree
 */
static void free_tree( tree t, hash_freefunc f )
{
	if( t )
	{
		if( t->left ) free_tree( t->left, f );
		if( t->right ) free_tree( t->right, f );
		freevalue( f, t->v );
		free( (hash_value) t->k );
		free( (hash_value) t );
	}
}


static void freevalue( hash_freefunc f, hash_value v )
{
	if( f != NULL )
	{
		(*f)( v );
	} else
	{
		free( v );
	}
}


/*
 * Hash members: how many members in the hash?
 */
static void count_cb( hash_key k, hash_value v, void *arg )
{
	int *n = (int *)arg;
	(*n)++;
}
int hashMembers( hash h )
{
	int n = 0;
	hashForeach( h, &count_cb, (void *)&n );
	return n;
}

/*
 * Hash isEmpty: is the hash empty?
 */
int hashIsEmpty( hash h )
{
	int n = hashMembers(h);
	return n==0;
}

/*
 * Compute the depth of a given tree
 */
#define max(a,b)  ((a)>(b)?(a):(b))
static int depth_tree( tree t )
{
	if( t )
	{
		int d2 = depth_tree( t->left );
		int d3 = depth_tree( t->right );
		return 1 + max(d2,d3);
	}
	return 0;
}


/*
 * Calculate hash on a string
 */
static int shash( char *str )
{
	unsigned char	ch;
	unsigned int	hh;
	for (hh = 0; (ch = *str++) != '\0'; hh = hh * 65599 + ch );
	return hh % NHASH;
}
