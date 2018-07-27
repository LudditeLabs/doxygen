#ifndef SQLITE3UTILS_H
#define SQLITE3UTILS_H

#include <cstddef>

struct sqlite3;
struct sqlite3_stmt;

struct SqlStmt {
  const char   *query;
  sqlite3_stmt *stmt;
  sqlite3 *db;
};


bool bindTextParameter(SqlStmt &s,const char *name,const char *value, bool _static=true);
bool bindBlobParameter(SqlStmt &s,const char *name,const void *value, size_t size, bool _static=true);
bool bindIntParameter(SqlStmt &s,const char *name,int value);
int step(SqlStmt &s,bool getRowId=false, bool select=false);
int prepareStatement(sqlite3 *db, SqlStmt &s);
void beginTransaction(sqlite3 *db);
void endTransaction(sqlite3 *db);

#endif // SQLITE3UTILS_H
