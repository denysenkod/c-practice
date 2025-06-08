// parseregex.c:	a module to turn the string form of a regex
//			into an abstract regex ADT

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>

#include "regex.h"
#include "parseregex.h"


// char *p = parse_range( s, set );
//	Parse (from s, a string) what must follow a '[' in a regex,
//	i.e. a range comprising a non-empty set of characters followed
//	by a ']'.  For example, s might be "abc]rest_of_regex" or
//	"ab-kp-q]rest_of_regex".  Abort if there is no ']' in s.
//	Writes a properly ASCII NUL-terminated string into set,
//	expanding all character ranges, not permitting any duplicate
//	characters in the set, then returns a pointer to the ']' in s.
//	
char *parse_range( char *s, char *set )
{
	// Notes: a range comprises one or more miniranges followed by a ']'.
	// a minirange is either a single character ch (like 'a'), or ch1-ch2
	// (like b-k or p-q).  miniranges are immediately adjacent to one
	// another.

	bool seen[256];
	for( int i=0; i<256; i++ ) seen[i] = false;

	do {
		char startch = *s;
		if( ! seen[(unsigned int)startch] )
		{
			*set++ = startch;
			seen[(unsigned int)startch] = true;
		}
		s++;
		if( *s == '-' )
		{
			s++;
			char endch = *s++;
			assert( startch <= endch );
			for( char c=startch+1; c<=endch; c++ )
			{
				if( ! seen[(unsigned int)c] )
				{
					*set++ = c;
					seen[(unsigned int)c] = true;
				}
			}
		}

	} while( *s != '\0' && *s != ']' );

	assert( s != NULL && *s == ']' );
	*set = '\0';
	return s;
}


// RE re = parse_regex( flags, str );
//	Parse <str>, the string form of a simplified regex, into <re>,
//	the abstract form.  Use flags. Abort if the string form is not valid.
//
RE parse_regex( REFlags flags, char *input_str )
{
	char256 str;
	strcpy( str, input_str );
	int len = strlen(str);
	if( *str == '^' )
	{
		flags->startanchor = true;
		memmove( str, str+1, len );   // move whole string down, even \0
		len--;
	}
	char lastch = str[len-1];
	if( lastch == '$' )
	{
		flags->endanchor = true;
		len--;
		str[len] = '\0';
	}
	SPList l = make_SPList( len+4 );
	char *s = str;
	for( ; *s != '\0'; s++ )
	{
		bool    isdot  = false;
		char256 setstr = "";

		if( *s == '.' )
		{
			isdot = true;
		} else if( *s == '[' )
		{
			s = parse_range( s+1, setstr );
			assert( *s == ']' );
		} else
		{
			setstr[0] = *s;		// set with one member
			setstr[1] = '\0';
		}
		char *set = NULL;	// strdup()ed set, if needed
		if( ! isdot )
		{
			set = strdup(setstr);
			assert( set != NULL );
		}

		SimpleType st = SimpleType_is_Dot;	// to suppress warning

		// STUB: Question 2: write code here that looks for repetition
		//	 characters like '?', '*' or '+' (skipping over such a
		//	 char if one was found), and then sets t to the right
		//	 simpletype, depending on the combination of isdot and
		//	 any rep char found, using the following table:
		//
		//		 ?      *       +       no-rep-char
		//	 isdot   OptDot DotStar DotPlus Dot
		//	 !isdot  Opt    Star    Plus    One

		// finally, after your code, make a simple pattern from st and
		// set, and then push that pattern onto the list
		push_SPList( l, make_SimplePat( st, set ) );
	}

	RE re = make_RE( flags, l );
	return re;
}
