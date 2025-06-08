// regex.h: abstract regex data type


// ---- Basic useful types ----

typedef char char128[128];
typedef char char256[256];
typedef char *optstr;


// ---- Type SimpleType ----

typedef enum {
	SimpleType_is_Dot,
	SimpleType_is_One,
	SimpleType_is_OptDot,
	SimpleType_is_Opt,
	SimpleType_is_DotStar,
	SimpleType_is_Star,
	SimpleType_is_DotPlus,
	SimpleType_is_Plus,
} SimpleType;


// ---- Type SimplePat ----

struct SimplePat {
	SimpleType t;
	optstr	   s;
};

typedef struct SimplePat *SimplePat;


// ---- Type SPList ----

struct SPList {
	SimplePat *l;		// l[maxsize] malloced chunk
	int        maxsize;	// max size of list (doesn't need to grow)
	int        nel;		// number elements in list
};

typedef struct SPList *SPList;


// ---- Type REFlags ----

struct REFlags {
	bool	startanchor;	// ^
	bool	endanchor;	// $
};

typedef struct REFlags *REFlags;


// ---- Type RE ----

struct RE {
	REFlags	flags;
	SPList	l;
};

typedef struct RE *RE;


// -- Prototypes, grouped by type

extern void print_optstr( FILE *f, optstr s );
extern void print_SimpleType( FILE *, SimpleType );

extern SimplePat make_SimplePat( SimpleType , optstr );
extern void print_SimplePat( FILE *, SimplePat );
extern void stringify_SimplePat( char *s, SimplePat sp );
extern void free_SimplePat( SimplePat sp );

extern SPList make_SPList( int maxsize );
extern void print_SPList( FILE *f, SPList l );
extern void push_SPList( SPList l, SimplePat sp );
extern void stringify_SPList( char *s, SPList l );
extern void free_SPList( SPList l );

extern void init_REFlags( REFlags flags );
extern void print_REFlags( FILE *f, REFlags refs );

extern RE make_RE( REFlags , SPList );
extern void print_RE( FILE *, RE );
extern void stringify_RE( char *s, RE re );
extern void free_RE( RE l );
