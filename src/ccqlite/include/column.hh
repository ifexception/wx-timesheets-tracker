//  TimesheetsTracker is a desktop that aids you in tracking your timesheets
//  and seeing what work you have done.
//
//  Copyright(C) <2018> <Szymon Welgus>
//
//  This program is free software : you can redistribute it and /
//  or modify it under the terms of the GNU General Public License as published
//  by the Free Software Foundation
//  , either version 3 of the License
//  , or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful
//  , but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.
//


#pragma once

#include <string>

#include <sqlite3.h>

#include "ccqliteapi.hh"
#include "value_type.hh"

namespace ccqlite
{
class CCQLITE_API column
{
  public:
    column() = delete;
    explicit column(const column& other);
    explicit column(sqlite3_stmt* handle, const int index);
    ~column() = default;

    column& operator=(const column& other);

    const std::string get_name() const noexcept;
#ifdef SQLITE_ENABLE_COLUMN_METADATA
    const std::string get_origin_name() const noexcept;
#endif // SQLITE_ENABLE_COLUMN_METADATA

    int get_int() const noexcept;
    unsigned int get_uint() const noexcept;
    long long get_int64() const noexcept;
    double get_double() const noexcept;
    std::string get_text() const noexcept;
    const void* get_blob() const noexcept;
    std::string get_string() const noexcept;

    value_type get_type() const noexcept;

  private:
    sqlite3_stmt* pHandle;
    int mIndex;
};
} // namespace ccqlite