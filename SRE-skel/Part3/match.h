// match.h

extern bool match_at( int currpos, char * str, int nel, SimplePat * l, bool endanchor, int * endposp );
extern bool match( RE re, char * str, int * sp, int * ep );
extern void report_match_str( RE re, char * searchstr, char * origregex );
