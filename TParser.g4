parser grammar TParser;

options {
	tokenVocab = TLexer;
}

main: SELECT fields FROM FIELD where_clause? portion_clause? EOF;
fields: ALL | FIELD (COMMA FIELD)*;
where_clause: WHERE condition (AND condition)*;
condition: FIELD EQUAL QUOTE;
portion_clause: LIMIT NUM (OFFSET NUM)? | OFFSET NUM (LIMIT NUM)?;