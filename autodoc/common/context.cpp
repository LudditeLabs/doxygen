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

#include "autodoc/common/context.h"
#include "autodoc/common/contentdb.h"
#include "message.h"
#include "qfileinfo.h"
#include "qdir.h"


autodoc::Context *autodoc::Context::m_instance = nullptr;


autodoc::Context::Context()
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
    m_contentDb.reset(new ContentDb(db, insertFileFunc));

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
