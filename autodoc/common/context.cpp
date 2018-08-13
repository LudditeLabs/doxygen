#include "autodoc/common/context.h"
#include "autodoc/common/db.h"
#include "message.h"
#include "qfileinfo.h"
#include "qdir.h"


autodoc::Context *autodoc::Context::m_instance = nullptr;


autodoc::Context::Context()
: m_contentDbFilename("content.db")
{

}
//-----------------------------------------------------------------------------

autodoc::Context::~Context()
{

}
//-----------------------------------------------------------------------------

void autodoc::Context::removeDbFile()
{
    QFileInfo info(m_contentDbFilename);
    if (info.exists())
        info.dir(TRUE).remove(info.fileName());
}
//-----------------------------------------------------------------------------

bool autodoc::Context::initDb(sqlite3 *db, InsertFileFunc insertFileFunc)
{
    m_contentDb.reset(new AutodocDb(db, insertFileFunc));

    if (!m_contentDb->initializeSchema())
        return false;

    else if (!m_contentDb->prepareStatements())
    {
        err("Content DB: prepareStatements failed!");
        return false;
    }

    return true;
}
//-----------------------------------------------------------------------------
