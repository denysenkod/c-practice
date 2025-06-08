// testparseregex: test the regex parsing..

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#include "testutils.h"

#include "regex.h"
#include "parseregex.h"


// --- Parse regex tests ---


typedef struct {
	char128 re;		// re string to parse
	bool    startanch;	// is it anchored at the start?
	bool    endanch;	// is it anchored at the end?
	int     npat;		// how many simple patterns?
	char128 regen;		// the regenerated "SPlist as src" string
} retest;


static retest parsere_tests[] =
{
	{ "^.*",        true,  false, 1, "^.*", },
	{ "^.*a",       true,  false, 2, "^.*a", },
	{ "^.*$",       true,  true,  1, "^.*$", },
	{ "b.*$",       false, true,  2, "b.*$", },
	{ "^.+$",       true,  true,  1, "^.+$" },
	{ "^.+hi",      true,  false, 3, "^.+hi" },
	{ "ho.+$",      false, true,  3, "ho.+$" },
	{ "^.*hello",   true,  false, 6, "^.*hello" },
	{ "^a*",        true,  false, 1, "^a*" },
	{ "a+",         false, false, 1, "a+" },
	{ "^a$",        true,  true,  1, "^a$" },
	{ "[a-fp-zq]+", false, false, 1, "[abcdefpqrstuvwxyz]+" },
	{ "a*b[ch]",    false, false, 3, "a*b[ch]" },
	{ "^a*b[ch]",   true,  false, 3, "^a*b[ch]" },
	{ "^a+b[ch]",   true,  false, 3, "^a+b[ch]" },
	{ "a+b[ch]",    false, false, 3, "a+b[ch]" },
	{ "a+$",        false, true,  1, "a+$" },
	{ "hello",      false, false, 5, "hello" },
	{ "a?b",        false, false, 2, "a?b" },
	{ "[a-f]?g",    false, false, 2, "[abcdef]?g" },
	{ ".?[bc]",     false, false, 2, ".?[bc]" },
	{ "r.+",        false, false, 2, "r.+" },
	{ "",           false, false, 0, "" },
};


static void testparseregex( retest *tests )
{
	struct REFlags reflags;

	for( int i=0; tests[i].re[0] != '\0'; i++ )
	{
		retest *t = &tests[i];
		printf( "testparseregex: %s ->\n", t->re );
		init_REFlags( &reflags );
		RE re = parse_regex( &reflags, t->re );
		//printf( "parsed RE /%s/: ", t->re );
		//print_RE( stdout, re );
		char256 regen;
		stringify_RE( regen, re );

		char256 label;
		sprintf( label, "parse(/%s/).startanchor", t->re );
		testint( (int)re->flags->startanchor, (int)t->startanch, label );
		sprintf( label, "parse(/%s/).endanchor", t->re );
		testint( (int)re->flags->endanchor, (int)t->endanch, label );

		sprintf( label, "parse(/%s/).npat", t->re );
		testint( re->l->nel, t->npat, label );

		sprintf( label, "parse(/%s/).regen", t->re );
		teststring( regen, t->regen, label );

		free_RE( re );
	}
}


int main( void )
{
	testparseregex( parsere_tests );
	return 0;
}
