// regex.c: abstract regex data type

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#include "regex.h"


// ---- Type SimplePat ----


// SimplePat sp = make_SimplePat( t, s );
//	Construct a SimplePat out of simple type t and optional string s
//
SimplePat make_SimplePat( SimpleType t, optstr s )
{
	SimplePat new = (SimplePat)malloc(sizeof(struct SimplePat));
	assert( new != NULL );
	new->t = t;
	new->s = s;
	return new;
}


// print_SimplePat( f, sp );
//	Print simple pattern sp to file f
//
void print_SimplePat( FILE *f, SimplePat p )
{
	switch( p->t )
	{
	case SimpleType_is_Dot:
		fputs( "Dot", f );
		break;
	case SimpleType_is_One:
		fprintf( f, "One(%s)", p->s );
		break;
	case SimpleType_is_OptDot:
		fputs( "OptDot", f );
		break;
	case SimpleType_is_Opt:
		fprintf( f, "Opt(%s)", p->s );
		break;
	case SimpleType_is_DotStar:
		fputs( "DotStar", f );
		break;
	case SimpleType_is_Star:
		fprintf( f, "Star(%s)", p->s );
		break;
	case SimpleType_is_DotPlus:
		fputs( "DotPlus", f );
		break;
	case SimpleType_is_Plus:
		fprintf( f, "Plus(%s)", p->s );
		break;
	default:
		abort();
	}
}


// sprint_set( s, set, repeat );
//	Append set (with optional repeat char, '\0' if none)
//	to string buffer s.
//
static void sprint_set( char *s, char *set, char repeat )
{
	assert( set != NULL );
	int len = strlen(set);
	if( len > 1 )
	{
		*s++ = '[';
		*s = '\0';
	}
	strcpy( s, set );
	s += strlen(s);
	if( len > 1 )
	{
		*s++ = ']';
		*s = '\0';
	}
	if( repeat != '\0' )
	{
		*s++ = repeat;
		*s = '\0';
	}
}


// stringify_SimplePat( s, sp );
//	stringify Simple Pattern sp as source form (eg. ".*")
//	into string buffer s.
//
void stringify_SimplePat( char *s, SimplePat sp )
{
	assert( sp != NULL );
	*s = '\0';
	switch( sp->t )
	{
	case SimpleType_is_Dot:
		strcpy( s, "." );
		break;
	case SimpleType_is_One:
		sprint_set( s, sp->s, '\0' );
		break;
	case SimpleType_is_OptDot:
		strcpy( s, ".?" );
		break;
	case SimpleType_is_Opt:
		sprint_set( s, sp->s, '?' );
		break;
	case SimpleType_is_DotStar:
		strcpy( s, ".*" );
		break;
	case SimpleType_is_Star:
		sprint_set( s, sp->s, '*' );
		break;
	case SimpleType_is_DotPlus:
		strcpy( s, ".+" );
		break;
	case SimpleType_is_Plus:
		sprint_set( s, sp->s, '+' );
		break;
	default:
		abort();
	}
}


// free_SimplePat( sp );
//	Free simple pattern sp
//
void free_SimplePat( SimplePat sp )
{
	free( sp->s );
 	free( sp );
}


// ---- Type SPList ----


// SPList l = make_SPList( maxsize );
//	Construct an empty list of size maxsize (dynamically
//	allocated but not dynamically growable)
//
SPList make_SPList( int maxsize )
{
	// STUB: Question 1a - implement this missing function
 	SPList new   = NULL;
	return new;
}


// print_SPList( f, l );
//	Print list l out to file f.
//
void print_SPList( FILE *f, SPList l )
{
	assert( l != NULL );
 	for( int i=0; i<l->nel; i++ )
	{
		print_SimplePat( f, l->l[i] );
		if( i<l->nel-1 ) fputs( ", ", f );
	}
}


// stringify_SPList( s, l );
//	Stringify list l into string buffer s.
//
void stringify_SPList( char *s, SPList l )
{
	assert( l != NULL );
	*s = '\0';
 	for( int i=0; i<l->nel; i++ )
	{
		stringify_SimplePat( s, l->l[i] );
		s += strlen(s);
	}
}


// push_SPList( l, sp );
//	Push simple pattern sp onto list l.
//
void push_SPList( SPList l, SimplePat sp )
{
	// STUB: Question 1b - implement this missing function
	assert( l != NULL );
}


// free_SPList( l );
//	Free list l's resources.
//
void free_SPList( SPList l )
{
	assert( l != NULL );
	for( int i=0; i<l->nel; i++ )
	{
		free_SimplePat( l->l[i] );
	}
 	free( l->l );
 	free( l );
}


// ---- Type REFlags ----


// init_REFlags( flags );
//	Initialise the RE Flags - all false
//
void init_REFlags( REFlags flags )
{
	flags->startanchor   = false;
	flags->endanchor     = false;
}


#define SHOWFLAG(val)   {if(flags->val){if(!first){fprintf(f,", ");}first=false;fprintf(f, #val);}}


// print_REFlags( f, flags );
//	Print out flags to file f.
//
void print_REFlags( FILE *f, REFlags flags )
{
	bool first=true;
	SHOWFLAG( startanchor );
	SHOWFLAG( endanchor );
}


// ---- Type RE ----


// REFlags flags; SPList l;
// RE re = make_RE( flags, l );
//	Construct an abstract RE out of some flags and a list of atoms.
//
RE make_RE( REFlags flags, SPList l )
{
	RE new = (RE)malloc(sizeof(struct RE));
	assert( new != NULL );
	new->flags = flags;
	new->l     = l;
	return new;
}


// print_RE( f, re );
//	Print abstract regex re out onto file f.
//
void print_RE( FILE *f, RE p )
{
	fputs( "flags: {", f );
	print_REFlags( f, p->flags );
	fputs( "}, [ ", f );
	print_SPList( f, p->l );
	fputs( " ]", f );
}


// stringify_RE( s, re );
//	Stringify abstract regex re to string buffer s
//	(s is assumed to be big enough).
//
void stringify_RE( char *s, RE re )
{
	assert( re != NULL );
	*s = '\0';
	if( re->flags->startanchor ) *s++ = '^';
	stringify_SPList( s, re->l );
	s += strlen(s);
	if( re->flags->endanchor ) *s++ = '$';
	*s = '\0';
}


// free_RE( re );
//	Free abstract regex re's resources.
//
void free_RE( RE re )
{
	free_SPList( re->l );
	free( re );
}
