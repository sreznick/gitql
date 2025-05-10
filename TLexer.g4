lexer grammar TLexer;

SELECT: 'SELECT' | 'select';
FROM: 'FROM' | 'from';
LIMIT: 'LIMIT' | 'limit';
OFFSET: 'OFFSET' | 'offset';
ALL: '*';
WHERE: 'WHERE' | 'where';
AND: 'AND' | 'and';
EQUAL: '==';
NUM: [0-9]+;
fragment QUOTE_DATA: [a-zA-Z0-9., \t\r\n]*;
QUOTE: '\'' QUOTE_DATA '\'';
FIELD: [a-zA-Z]+;
COMMA: ',';
WS: [ \t\r\n]+ -> skip;