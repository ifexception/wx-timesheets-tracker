// Productivity tool to help you track the time you spend on tasks
// Copyright (C) 2020  Szymon Welgus
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
//  Contact:
//    szymonwelgus at gmail dot com

#include "databasebackup.h"

#include <algorithm>

#include <wx/datetime.h>
#include <wx/file.h>
#include <wx/stdpaths.h>

#include "../common/common.h"

namespace app::svc
{
DatabaseBackup::DatabaseBackup(std::shared_ptr<cfg::Configuration> config,
    std::shared_ptr<spdlog::logger> logger,
    sqlite::database* database)
    : pConfig(config)
    , pLogger(logger)
    , pDatabase(database)
{
}

bool DatabaseBackup::Execute()
{
    wxString fileName = CreateBackupFileName();
    bool result = CreateBackupFile(fileName);
    result = ExecuteBackup(fileName);
    result = MoveBackupFileToBackupDirectory(fileName);
    return result;
}

wxString DatabaseBackup::CreateBackupFileName()
{
    auto dateTime = wxDateTime::Now();
    auto dateTimeString = dateTime.FormatISODate();
    auto indexOfPeriod = common::GetDatabaseFileName().find(wxT("."), 0);
    wxString databaseFileName = "";
    if (indexOfPeriod != wxString::npos) {
        databaseFileName = common::GetDatabaseFileName().substr(0, indexOfPeriod);
    } else {
        return wxGetEmptyString();
    }
    auto backupFileName = wxString::Format(wxT("%s.%s.db"), databaseFileName, dateTimeString);
    std::replace(backupFileName.begin(), backupFileName.end(), ':', '.');

    return backupFileName;
}

bool DatabaseBackup::CreateBackupFile(const wxString& fileName)
{
    wxFile file;
    bool success = file.Create(fileName);
    if (!success) {
        pLogger->error("Failed to create file {0}", fileName.ToStdString());
    }
    file.Close();
    return success;
}

bool DatabaseBackup::ExecuteBackup(const wxString& fileName)
{
    try {
        auto config = sqlite::sqlite_config{ sqlite::OpenFlags::READWRITE, nullptr, sqlite::Encoding::UTF8 };
        sqlite::database backupConnection(fileName.ToStdString(), config);
        auto existingConnection = pDatabase->connection();

        auto state = std::unique_ptr<sqlite3_backup, decltype(&sqlite3_backup_finish)>(
            sqlite3_backup_init(backupConnection.connection().get(), "main", existingConnection.get(), "main"),
            sqlite3_backup_finish);

        if (state) {
            int rc = -1;
            do {
                rc = sqlite3_backup_step(state.get(), -1);
            } while (rc == SQLITE_OK || rc == SQLITE_BUSY || rc == SQLITE_LOCKED);
        }
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured when running database backup - {0:d} : {1}", e.get_code(), e.what());
        return false;
    }
    return true;
}

bool DatabaseBackup::MoveBackupFileToBackupDirectory(const wxString& fileName)
{
    auto currentDatabaseDirectory = wxPathOnly(wxStandardPaths::Get().GetExecutablePath());
    auto fullFilePath = wxString::Format(wxT("%s\\%s"), currentDatabaseDirectory, fileName);
    auto destinationFullFilePath = wxString::Format(wxT("%s\\%s"), pConfig->GetBackupPath(), fileName);

    if (wxCopyFile(fullFilePath, destinationFullFilePath, true)) {
        if (wxRemoveFile(fullFilePath)) {
            return true;
        } else {
            pLogger->error("Failed to remove file {0}", fullFilePath.ToStdString());
        }
    } else {
        pLogger->error(
            "Failed to copy file {0} to {1}", fullFilePath.ToStdString(), destinationFullFilePath.ToStdString());
    }

    return false;
}
} // namespace app::svc
