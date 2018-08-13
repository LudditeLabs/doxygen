#ifndef AUTODOC_COMMON_CONTEXT_H
#define AUTODOC_COMMON_CONTEXT_H

#include "qcstring.h"
#include <memory>

#define autodocCtx autodoc::Context::instance

typedef int (*InsertFileFunc)(const char*);
struct sqlite3;

namespace autodoc
{

class AutodocDb;

class Context
{
public:
    Context();
    ~Context();

    static Context* instance()
    {
        if (!m_instance)
            m_instance = new Context;
        return m_instance;
    }

    const QCString& contentDbFilename() const { return m_contentDbFilename; }
    void setContentDbFilename(const QCString &filename)
    {
        m_contentDbFilename = filename;
    }

    void removeDbFile();
    bool initDb(sqlite3 *db, InsertFileFunc insertFileFunc);
    AutodocDb* contentDb() const { return m_contentDb.get(); }

private:
    static Context *m_instance;

    QCString m_contentDbFilename;
    std::unique_ptr<AutodocDb> m_contentDb;
};

} // namespace autodoc


#endif // AUTODOC_COMMON_CONTEXT_H
