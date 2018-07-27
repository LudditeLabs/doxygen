#include "sqlite3utils.h"
#include "qglobal.h"
#include "settings.h"
#include "message.h"
#include <cstdlib>
#include <sqlite3.h>


bool bindTextParameter(SqlStmt &s,const char *name,const char *value, bool _static)
{
  int idx = sqlite3_bind_parameter_index(s.stmt, name);
  if (idx==0) {
    msg("sqlite3_bind_parameter_index(%s)[%s] failed: %s\n", name, s.query, sqlite3_errmsg(s.db));
    return false;
  }
  int rv = sqlite3_bind_text(s.stmt, idx, value, -1, _static==TRUE?SQLITE_STATIC:SQLITE_TRANSIENT);
  if (rv!=SQLITE_OK) {
    msg("sqlite3_bind_text(%s)[%s] failed: %s\n", name, s.query, sqlite3_errmsg(s.db));
    return false;
  }
  return true;
}

bool bindBlobParameter(SqlStmt &s,const char *name,const void *value, size_t size, bool _static)
{
  int idx = sqlite3_bind_parameter_index(s.stmt, name);
  if (idx==0) {
    msg("sqlite3_bind_parameter_index(%s)[%s] failed: %s\n", name, s.query, sqlite3_errmsg(s.db));
    return false;
  }
  int rv = sqlite3_bind_blob64(s.stmt, idx, value, size, _static==TRUE?SQLITE_STATIC:SQLITE_TRANSIENT);
  if (rv!=SQLITE_OK) {
    msg("sqlite3_bind_blob64(%s)[%s] failed: %s\n", name, s.query, sqlite3_errmsg(s.db));
    return false;
  }
  return true;
}


bool bindIntParameter(SqlStmt &s,const char *name,int value)
{
  int idx = sqlite3_bind_parameter_index(s.stmt, name);
  if (idx==0) {
    msg("sqlite3_bind_parameter_index(%s)[%s] failed: %s\n", name, s.query, sqlite3_errmsg(s.db));
    return false;
  }
  int rv = sqlite3_bind_int(s.stmt, idx, value);
  if (rv!=SQLITE_OK) {
    msg("sqlite3_bind_int(%s)[%s] failed: %s\n", name, s.query, sqlite3_errmsg(s.db));
    return false;
  }
  return true;
}

int step(SqlStmt &s,bool getRowId, bool select)
{
  int rowid=-1;
  int rc = sqlite3_step(s.stmt);
  if (rc!=SQLITE_DONE && rc!=SQLITE_ROW)
  {
    msg("sqlite3_step: %s\n", sqlite3_errmsg(s.db));
    sqlite3_reset(s.stmt);
    sqlite3_clear_bindings(s.stmt);
    return -1;
  }
  if (getRowId && select) rowid = sqlite3_column_int(s.stmt, 0); // works on selects, doesn't on inserts
  if (getRowId && !select) rowid = sqlite3_last_insert_rowid(s.db); //works on inserts, doesn't on selects
  sqlite3_reset(s.stmt);
  sqlite3_clear_bindings(s.stmt); // XXX When should this really be called
  return rowid;
}

int prepareStatement(sqlite3 *db, SqlStmt &s)
{
  int rc;
  rc = sqlite3_prepare_v2(db,s.query,-1,&s.stmt,0);
  if (rc!=SQLITE_OK)
  {
    msg("prepare failed for %s\n%s\n", s.query, sqlite3_errmsg(db));
    s.db = NULL;
    return -1;
  }
  s.db = db;
  return rc;
}

void beginTransaction(sqlite3 *db)
{
  char * sErrMsg = 0;
  sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, &sErrMsg);
}

void endTransaction(sqlite3 *db)
{
  char * sErrMsg = 0;
  sqlite3_exec(db, "END TRANSACTION", NULL, NULL, &sErrMsg);
}
