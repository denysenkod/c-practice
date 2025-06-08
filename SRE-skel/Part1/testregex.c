// testregex: test the regex adt module..

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#include "testutils.h"

#include "regex.h"


// --- SimplePat tests ---


typedef struct {
	SimpleType t;
	optstr	   s;
	char256    output;	// result of stringify(make_SimplePat(t,s))
} SPtest;


static SPtest msp_tests[] =
{
	{ SimpleType_is_Dot,     NULL,  "." },
	{ SimpleType_is_One,     "a",   "a" },
	{ SimpleType_is_One,     "abc", "[abc]" },
	{ SimpleType_is_OptDot,  NULL,  ".?" },
	{ SimpleType_is_Opt,     "a",   "a?" },
	{ SimpleType_is_Opt,     "abc", "[abc]?" },
	{ SimpleType_is_DotStar, NULL,  ".*" },
	{ SimpleType_is_Star,    "a",   "a*" },
	{ SimpleType_is_Star,    "abc", "[abc]*" },
	{ SimpleType_is_DotPlus, NULL,  ".+" },
	{ SimpleType_is_Plus,    "a",   "a+" },
	{ SimpleType_is_Plus,    "abc", "[abc]+" },
	{ 0,                     NULL,  "" },
};


static void testmsp( SPtest *tests )
{
	for( int i=0; tests[i].output[0] != '\0'; i++ )
	{
		SPtest *t = &tests[i];
		//printf( "testmsp(t=%d, s='%s')",
		//	  t->t, t->s==NULL?"null":t->s );

		char256 output;
		*output = '\0';

		char *sdup = NULL;
		if( t->s != NULL )
		{
			sdup = strdup(t->s);
		}
		SimplePat sp = make_SimplePat( t->t, sdup );
		stringify_SimplePat( output, sp );

		char256 label;
		sprintf( label, "stringify(msp(%d,'%s'))",
			 t->t, t->s==NULL?"null":t->s );
		teststring( output, t->output, label );

		free_SimplePat( sp );
	}
}


// --- SPList tests --- reuses the SPtests..

static void testspl( SPtest *tests )
{
	int ntests = 0;
	for( int i=0; tests[i].output[0] != '\0'; i++ ) ntests++;

	SPList l = make_SPList( ntests+5 );
	char256 lstr = "";

	for( int i=0; tests[i].output[0] != '\0'; i++ )
	{
		SPtest *t = &tests[i];
		//printf( "testspl(t=%d, s='%s')",
		//	  t->t, t->s==NULL?"null":t->s );

		char *sdup = NULL;
		if( t->s != NULL )
		{
			sdup = strdup(t->s);
		}
		SimplePat sp = make_SimplePat( t->t, sdup );
		push_SPList( l, sp );		// ok, append sp to list

		strcat( lstr, t->output );	// lstr should be the result of
						// stringify(push(sp))

		char256 output;
		stringify_SPList( output, l );	// stringify list

		char256 label;
		sprintf( label, "stringify(list+=sp(%d,'%s'))",
			 t->t, t->s==NULL?"null":t->s );
		teststring( output, lstr, label );
	}

	free_SPList( l );
}


// --- RE tests --- reuses the SPtests..

// check_re_str( sa, ea, l, a, b, c );
//	  Perform a check on the RE given that startanchor==sa, endanchor==ea,
//	  SPList is l, and the stringification of that RE is a++b++c
//
static void check_re_str( bool sa, bool ea, SPList l, char *a, char *b, char *c )
{
	struct REFlags flags;
	init_REFlags( &flags );
	flags.startanchor = sa;
	flags.endanchor   = ea;

	RE re = make_RE( &flags, l );

	char256 output;
	stringify_RE( output, re );

	char256 corrstr;
	sprintf( corrstr, "%s%s%s", a, b, c );

	char256 label;
	sprintf( label, "stringify(re(%d,%d,l))", sa, ea );
	teststring( output, corrstr, label );

	free( re );		// deliberately not freeing re->l, i.e. l
}


static void testre( SPtest *tests )
{
	int ntests = 0;
	for( int i=0; tests[i].output[0] != '\0'; i++ ) ntests++;

	SPList l = make_SPList( ntests+5 );

	for( int i=0; tests[i].output[0] != '\0'; i++ )
	{
		SPtest *t = &tests[i];
		//printf( "makelist(t=%d, s='%s')",
		//	  t->t, t->s==NULL?"null":t->s );

		char *sdup = NULL;
		if( t->s != NULL )
		{
			sdup = strdup(t->s);
		}
		SimplePat sp = make_SimplePat( t->t, sdup );
		push_SPList( l, sp );		// ok, append sp to list
	}

	char256 output;
	stringify_SPList( output, l );	// stringify list

	//printf( "debug: testre: l is <%s>\n", output );

	check_re_str( false, false, l, "",  output, "" );
	check_re_str( true,  false, l, "^", output, "" );
	check_re_str( false, true, l, "", output, "$" );
	check_re_str( true,  true, l, "^", output, "$" );

	free_SPList( l );
}


int main( void )
{
	testmsp( msp_tests );
	testspl( msp_tests );
	testre( msp_tests );
	return 0;
}
