// searchstr: grep on a string

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#include "regex.h"
#include "parseregex.h"
#include "match.h"
#include "emphasize.h"

#define DEBUG
#undef DEBUG


int main( int argc, char **argv )
{
	if( argc != 3 )
	{
		fprintf( stderr, "Usage: searchstr regex string\n" );
		exit(1);
	}

	char *regstr = strdup(argv[1]);	// make a mutable string buffer
	assert( regstr != NULL );
	struct REFlags flags;
	init_REFlags( &flags );

	init_emphasize();

	RE re = parse_regex( &flags, regstr );

	report_match_str( re, argv[2], regstr );

	free_RE( re );
	free( regstr );

	close_emphasize();

	return 0;
}
