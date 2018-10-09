/* Copyright (C) 2018, Luddite Labs Inc.
 *
 * This file is part of doxygen.
 *
 * Doxygen is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef AUTODOC_COMMON_CONTEXT_H
#define AUTODOC_COMMON_CONTEXT_H

#include "qcstring.h"
#include <memory>

#define autodocCtx autodoc::Context::instance

typedef int (*InsertFileFunc)(const char*);
struct sqlite3;

namespace autodoc
{

class ContentDb;

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
    ContentDb* contentDb() const { return m_contentDb.get(); }

private:
    static Context *m_instance;

    QCString m_contentDbFilename;
    std::unique_ptr<ContentDb> m_contentDb;
};

} // namespace autodoc


#endif // AUTODOC_COMMON_CONTEXT_H
