// This file was generated by queries.rb
typedef struct query_str {const char *query; } query_str;
size_t num_queries = 14;

query_str queries[15] = {
{"BEGIN WORK"},
{"INSERT INTO test VALUES(\"kkk1\", {\"a\":1})"},
{"SELECT d.*, META(d).id FROM test AS d WHERE d.a >= 0"},
{"SAVEPOINT s1"},
{"UPDATE test AS d SET d.b = 10 WHERE d.a > 0"},
{"SELECT d.*, META(d).id FROM test AS d WHERE d.a >= 0"},
{"SAVEPOINT s2;"},
{"UPDATE test AS d SET d.b = 10, d.c = \"xyz\" WHERE d.a > 0;"},
{"SELECT d.*, META(d).id FROM test AS d WHERE d.a >= 0"},
{"ROLLBACK TRAN TO SAVEPOINT s2"},
{"SELECT d.*, META(d).id FROM test AS d WHERE d.a >= 0"},
{"INSERT INTO test VALUES(\"kkk2\", {\"a\":2})"},
{"UPDATE test AS d SET d.b = 20, d.c = \"xyz\" WHERE d.a > 0"},
{"COMMIT WORK"}};
