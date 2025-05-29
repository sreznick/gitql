parser grammar TParser;

options {
	tokenVocab = TLexer;
}

main: SELECT fields FROM FIELD where_clause? portion_clause? EOF;
fields: ALL | FIELD (COMMA FIELD)*;
where_clause: WHERE condition (AND condition)*;
condition: equal | contains | range;
range: FIELD IN RANGE QUOTE AND QUOTE;
equal: FIELD EQUAL QUOTE;
contains: QUOTE IN FIELD;
portion_clause
	: LIMIT NUM (OFFSET NUM)? #limitFirst
	| OFFSET NUM (LIMIT NUM)? #offsetFirst
	;