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

#pragma once

#include <string>

namespace app::cfg
{
class Config
{
  public:
    static Config& Get();

    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;

    std::string GetConnectionString() const;

  private:
    Config();
    ~Config() = default;

    void SetConnectionString(const std::string& connectionString);

    std::string mConnectionString;
};
} // namespace app::cfg
