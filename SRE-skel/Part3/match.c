// match.c: regex pattern matching via recursive search

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include "regex.h"
#include "match.h"
#include "emphasize.h"


#undef MATCH_DEBUG
#define MATCH_DEBUG


//
// highlight_match( startcol, line, startpos, endpos );
//	attempt to highlight the matching part (line[startpos..endpos]) given
//	that the line starts at column startcol
//
static void highlight_match( int startcol, char *line, int startpos, int endpos )
{
	int matchw = 1+endpos-startpos;

	#if 0
	printf( "debug: hm: startcol=%d, startpos=%d, endpos=%d, matchw=%d, "
		"line=<%s>\n", startcol, startpos, endpos, matchw, line );
	#endif

	char256 before;
	assert( startpos<sizeof(char256) );
	strncpy( before, line, startpos );
	before[startpos] = '\0';

	char256 matched;
	strncpy( matched, line+startpos, matchw );
	assert( matchw<sizeof(char256) );
	matched[matchw] = '\0';

	char256 after;
	*after = '\0';
	if( line[endpos] != '\0' )
	{
		assert( strlen(line+endpos+1)<sizeof(char256) );
		strcpy( after, line+endpos+1 );
	}

	#if 0
	printf( "debug: before=<%s>, matched=<%s>, after=<%s>\n",
		before, matched, after );
	//exit(1);
	#endif

	printf( "%s", before );
	emphasize( matched );
	printf( "%s\n", after );
}


// int firstoff = -1;
// int noff     = 0;
// bool matched = match_simplepat( sp, str, currpos, &firstoff, &noff );
//	attempt to match a simple pattern sp against str (currpos is only
//	used for debugging).  Return false if sp can't match at all.
//
//	Otherwise, return true having set firstoff>=0 and noff>0 - these
//	represent the range of possible match offsets of all the positions
//	immediately after sp has matched successfully, i.e. the positions
//	where the rest of the regex might be matched at.
//
bool match_simplepat( SimplePat sp, char *str, int currpos, int *fp, int *np )
{
	char *set      = sp->s;

	// range (firstoff,noff) == firstoff..firstoff+noff-1 (if noff>0)
	int   firstoff = -1;
	int   noff     = 0;

	#ifdef MATCH_DEBUG
	printf( "debug: m_sp: attempting match of " );
	print_SimplePat( stdout, sp );
	printf( " against str '%s', currpos %d\n", str, currpos );
	#endif

	switch( sp->t )
	{
	case SimpleType_is_Dot:			// consume exactly one char
		if( *str != '\0' )
		{
			firstoff = 1; noff = 1;
			assert( noff == 1 && firstoff == 1 );
		}
		break;
	case SimpleType_is_One:			// consume exactly one set-char
		if( *str != '\0' && strchr(set,*str) != NULL )
		{
			firstoff = 1; noff = 1;
			assert( noff == 1 && firstoff == 1 );
		}
		break;
	case SimpleType_is_OptDot:		// consume zero or one char
		firstoff = 0; noff = 1;		// - can consume zero chars
		if( *str != '\0' )
		{
			noff++;			// - can consume one char
		}
		assert( noff>0 && firstoff == 0 );
		break;
	case SimpleType_is_Opt:			// consume zero or one set-char
		firstoff = 0; noff = 1;		// - can consume zero chars
		if( *str != '\0' && strchr(set,*str) != NULL )
		{
			noff++;			// - can consume one char
		}
		assert( noff>0 && firstoff == 0 );
		break;
	case SimpleType_is_DotStar:		// consume zero or more chars
		// STUB: complete this

		// NOTE: if this simplepattern does match the target string,
		// here's an assertion that should be true.. you might like to
		// put it inside your "if matches" code
		// assert( noff>0 && firstoff == 0 );
		break;
	case SimpleType_is_Star:		// consume zero or more set-chars
		// STUB: complete this

		// NOTE: if this simplepattern does match the target string,
		// here's an assertion that should be true.. you might like to
		// put it inside your "if matches" code
		// assert( noff>0 && firstoff == 0 );
		break;
	case SimpleType_is_DotPlus:		// consume one or more chars
		// STUB: complete this

		// NOTE: if this simplepattern does match the target string,
		// here's an assertion that should be true.. you might like to
		// put it inside your "if matches" code
		// assert( noff>0 && firstoff == 1 );
		break;
	case SimpleType_is_Plus:		// consume one or more set-chars
		// STUB: complete this

		// NOTE: if this simplepattern does match the target string,
		// here's an assertion that should be true.. you might like to
		// put it inside your "if matches" code
		// assert( noff>0 && firstoff == 1 );
		break;
	default:
		abort();
	}

	#ifdef MATCH_DEBUG
	printf( "debug: m_sp: %d possible match offsets: [", noff );
	if( noff>0 )
	{
		for( int i=0; i<noff; i++ )
		{
			printf( "%d, ", firstoff+i );
		}
	}
	printf( "]\nagainst str '%s', currpos %d\n", str, currpos );
	#endif

	if( noff == 0 ) return false;	// sp didn't match

	*fp = firstoff;
	*np = noff;
	return true;
}


//
// int endpos = -1;
// bool matched = match_at( currpos, str, nel, l, endanchor, &endpos );
//	Attempt to match str (the unconsumed part, i.e. str == &line[currpos])
//	against the list l of nel SimplePatterns.  If a match is found, store
//	the last matching position in endpos, and return true. Otherwise return
//	false to indicate no match found.  Don't need to handle startanchor but
//	so need to handle endanchor.
//
bool match_at( int currpos, char *str, int nel, SimplePat *l, bool endanchor, int *endposp )
{
	#ifdef MATCH_DEBUG
	printf( "debug: m_s_at@entry: attempting match of list [" );
	for( int i=0; i<nel; i++ )
	{
		print_SimplePat( stdout, l[i] );
		if( i<nel-1 ) printf( ", " );
	}
	printf( "], len %d, against str '%s', currpos %d, with endanchor %d\n",
		nel, str, currpos, endanchor );
	#endif

	if( nel == 0 )
	{
		// matched whole list, handle endanchor
		if( endanchor && *str != '\0' )
		{
			#ifdef MATCH_DEBUG
			printf( "debug: m_s_at: match failing at end, currpos"
				" %d cos %c not NUL when endanchor\n",
				currpos, *str );
			#endif
			return false;
		}
		#ifdef MATCH_DEBUG
		printf( "debug: m_s_at: match succeeded at end, currpos %d"
			" char %c\n", currpos, *str );
		#endif
		*endposp = currpos-1;	// always one step beyond..
		return true;
	}

	// otherwise first try to match head(l) against str and build a range
	// of possible match offsets of all the positions immediately after
	// head(l) matches successfully, i.e. the positions where tail(l) might
	// be matched at.

	// range (firstoff,noff) == firstoff..firstoff+noff-1 (if noff>0)
	int   firstoff = -1;
	int   noff     = 0;
	bool  matchhead = match_simplepat( l[0], str, currpos, &firstoff, &noff );

	if( ! matchhead )
	{
		// no match starting at currpos because head didn't match
		return false;
	}

	// ok, try matching rest of the list of simple patterns from offset
	// off[i] for each possible offset in reverse order (for greediness)
	nel--; l++;
	int lastoff = firstoff + noff - 1;
	for( int of=lastoff; of>=firstoff; of-- )
	{
		#ifdef MATCH_DEBUG
		printf( "debug: m_s_at: trying offset %d against rest "
			"of l: [", of );
		for( int j=0; j<nel; j++ )
		{
			print_SimplePat( stdout, l[j] );
			if( j<nel-1 ) printf( ", " );
		}
		printf( "] against str '%s', currpos %d\n",
			str+of, currpos+of );
		#endif

		if( match_at( currpos+of, str+of, nel, l,
					     endanchor, endposp ) )
		{
			return true;
		}
	}

	// otherwise no match starting at currpos
	return false;
}


//
// RE re;
// int startpos, endpos;
// bool ismatched = match( re, str, &startpos, &endpos );
//	Attempt to match str against the abstract regex re
//	(anchored at the start of the string if startanchor is true,
//	floating along the string otherwise)
//	return true iff the regex matches the string,
//	and also set startpos and endpos.
//
bool match( RE re, char *str, int *sp, int *ep )
{
	bool startanchor = re->flags->startanchor;
	bool endanchor   = re->flags->endanchor;
	bool matched     = false;
	*sp              = 0;
	*ep              = 0;

	if( startanchor )
	{
		int endpos = -1;
		matched = match_at( 0, str, re->l->nel, re->l->l,
					     endanchor, &endpos );
		if( matched )
		{
			*sp = 0;
			*ep = endpos;
		}
	} else
	{
		for( int currpos=0; str[currpos] != '\0' && !matched; currpos++ )
		{
			// match starting at pos currpos
			int endpos = -1;
			matched = match_at( currpos, str+currpos,
						     re->l->nel, re->l->l,
						     endanchor, &endpos );
			if( matched )
			{
				*sp = currpos;
				*ep = endpos;
			}
		}
	}

	return matched;
}


//
// report_match_str( re, searchstr, regex );
//	Perform a match - from a string not a file - and report on the results
//
void report_match_str( RE re, char *searchstr, char *origregex )
{
	#if 0
	printf( "debug: rms: searchstr=<%s>\n", searchstr );
	#endif
	int startpos, endpos;
	bool matched = match( re, searchstr, &startpos, &endpos );
	#if 0
	printf( "debug: rms after match: searchstr=<%s>\n", searchstr );
	#endif
	if( matched )
	{
		printf( "have matched RE /%s/ against target: ", origregex );
		highlight_match( 0, searchstr, startpos, endpos );
	} else
	{
		printf( "no match for RE /%s/ against target: %s\n",
			origregex, searchstr );
	}
}
