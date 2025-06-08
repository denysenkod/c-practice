// testmatch: test pattern matching..

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#include "testutils.h"

#include "regex.h"
#include "parseregex.h"
#include "match.h"


#define NELEMS(arr)	(sizeof(arr)/sizeof(*arr))


// --- no match tests ---

typedef struct {
	char      *regexstr;
	char	  *targetstr;
} nomatchtest;

static nomatchtest nm_tests[] =
{
	{ "h", "" },
	{ "h", "eek" },
	{ ".", "" },
	{ "hello", "" },
	{ "hello", "hallo there" },
	{ "hello", "hell there" },
	{ "hello", "well hell there" },
	{ "h.llo", "hell" },
	{ "h.llo$", "hxllooo" },
	{ "he?llo", "well hll there" },
	{ "he?llo", "well heell there" },
	{ "he*llo", "well hll there" },
	{ "he*llo", "well hell there" },
	{ "he*llo", "well heeeell there" },
	{ "he*l*o", "well heeeell there" },
	{ "he*l*o", "well heekell there" },
	{ "he+llo", "well hll there" },
	{ "he+llo", "well hell there" },
	{ "he+llo", "well heeeell there" },
	{ "he+llo", "well hllo there" },
	{ "he+llo", "well helllo there" },
	{ "h[eua]+llo", "well helllo there" },
	{ "h[eua]+llo", "well hllo there" },
	{ "h[eua]+llo", "well hollo there" },
	{ "h[eua]*[lm]*o", "well hlko there" },
	{ "h[eua]*[lm]*o", "well heklo there" },
	{ "h[eua]*[lm]*o", "well hillo there" },
};

static void testnm( void )
{
	int nel = NELEMS(nm_tests);
	for( int i=0; i < nel; i++ )
	{
		nomatchtest *t = &nm_tests[i];
		//printf( "testnm(t=%d, re='%s', target='%s')",
		//	  t->t, t->restr, t->targetstr );

		struct REFlags flags;
		init_REFlags( &flags );
		RE re = parse_regex( &flags, t->regexstr );

		int startpos, endpos;
		bool matched = match( re, t->targetstr, &startpos, &endpos );
		if( matched )
		{
			printf( "debug: have matched RE /%s/ against target: '%s'"
				"at pos %d..%d\n",
				t->regexstr, t->targetstr,
				startpos, endpos );
		} else
		{
			printf( "debug: no match for RE /%s/ against target: %s\n",
				t->regexstr, t->targetstr );
		}

		char256 label;
		sprintf( label, "!match('%s','%s'))",
			 t->regexstr, t->targetstr );
		testcond( ! matched, label );

		free_RE( re );
	}
}


// --- match tests ---

typedef struct {
	char      *regexstr;
	char	  *targetstr;
	int	   startpos;
	int	   endpos;
} matchtest;

static matchtest m_tests[] =
{
	{ "h", "h", 0, 0 },
	{ "h", "he", 0, 0 },
	{ "h", "eh", 1, 1 },
	{ "h", "loopy is the best", 10, 10 },
	{ ".", "h", 0, 0 },
	{ ".", "i", 0, 0 },
	{ ".", "Z", 0, 0 },
	{ ".", "x", 0, 0 },
	{ "hi", "hi", 0, 1 },
	{ "hi", "this", 1, 2 },
	{ "hi", "thi$", 1, 2 },
	{ "h.", "hi", 0, 1 },
	{ "h.", "this", 1, 2 },
	{ "h.", "thi$", 1, 2 },
	{ "hello", "hello", 0, 4 },
	{ "hello", "hello there", 0, 4 },
	{ "hello", "well hello there", 5, 9 },
	{ "hello", "wellhellothere", 4, 8 },
	{ "hello", "hell hello there", 5, 9 },
	{ "h.llo", "hello", 0, 4 },
	{ "h.llo", "hxllo", 0, 4 },
	{ "h.llo$", "hxllo", 0, 4 },
	{ "h.?llo", "well hllo there", 5, 8 },
	{ "h.?llo", "well hello there", 5, 9 },
	{ "h.?llo", "well hxllo there", 5, 9 },
	{ "h.*llo", "well hllo there", 5, 8 },
	{ "h.*llo", "well hello there", 5, 9 },
	{ "h.*llo", "well hxllo there", 5, 9 },
	{ "h.*llo", "well hdfhsdhfxllo there", 5, 16 },
	{ "h.*llo", "well h1234yllo there", 5, 13 },
	{ "he*llo", "hllo there", 0, 3 },
	{ "he*llo", "hello there", 0, 4 },
	{ "he*llo", "heeeeello there", 0, 8 },
	{ "he*llo", "well hllo there", 5, 8 },
	{ "he*llo", "well hello there", 5, 9 },
	{ "he*llo", "well heeeeello there", 5, 13 },
	{ "he?llo", "well hllo there", 5, 8 },
	{ "he?llo", "well hello there", 5, 9 },
	{ "he*l*o", "well hoo there", 5, 6 },
	{ "he*l*o", "well hlo there", 5, 7 },
	{ "he*l*o", "well heeeeo there", 5, 10 },
	{ "he+llo", "well hello there", 5, 9 },
	{ "he+llo", "well heello there", 5, 10 },
	{ "h[eua]+llo", "well hello there", 5, 9 },
	{ "h[eua]+llo", "well hullo there", 5, 9 },
	{ "h[eua]+llo", "well heuauaueueuaeullo there", 5, 21 },
};


static void testm( void )
{
	int nel = NELEMS(m_tests);
	for( int i=0; i < nel; i++ )
	{
		matchtest *t = &m_tests[i];
		//printf( "testm(t=%d, re='%s', target='%s', sp=%d, ep=%d)",
		//	  t->t, t->restr, t->targetstr, t->startpos, t->endpos );

		struct REFlags flags;
		init_REFlags( &flags );
		RE re = parse_regex( &flags, t->regexstr );

		int startpos, endpos;
		bool matched = match( re, t->targetstr, &startpos, &endpos );
		if( matched )
		{
			printf( "debug: have matched RE /%s/ against target: '%s'"
				"at pos %d..%d\n",
				t->regexstr, t->targetstr,
				startpos, endpos );
		} else
		{
			printf( "debug: no match for RE /%s/ against target: %s\n",
				t->regexstr, t->targetstr );
		}

		char256 label;
		sprintf( label, "match('%s','%s')).matched",
			 t->regexstr, t->targetstr );
		testcond( matched, label );

		sprintf( label, "match('%s','%s')).startpos",
			 t->regexstr, t->targetstr );
		testint( startpos, t->startpos, label );

		sprintf( label, "match('%s','%s')).endpos",
			 t->regexstr, t->targetstr );
		testint( endpos, t->endpos, label );

		free_RE( re );
	}
}


int main( void )
{
	testnm();
	testm();
	return 0;
}
