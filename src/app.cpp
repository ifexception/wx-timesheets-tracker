//  TimesheetsTracker is a desktop that aids you in tracking your timesheets
//  and seeing what work you have done.
//
//  Copyright(C)<2018><Szymon Welgus>
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
//
//  Contact:
//    szymonwelgus at gmail dot com

#include "app.h"

#include <wx/msw/registry.h>

#include "common/constants.h"
#include "frame/mainframe.h"

namespace app
{
App::~App()
{ }

bool App::OnInit()
{
    bool logDirectoryCreated = CreateLogsDirectory();
    if (!logDirectoryCreated) {
        return false;
    }

    bool loggingInitialized = InitializeLogging();
    if (!loggingInitialized) {
        return false;
    }

    bool dbFileExists = DatabaseFileExists();
    if (!dbFileExists) {
        return false;
    }

    bool isInstalled = IsInstalled();

    frame::MainFrame* mf = new frame::MainFrame(nullptr);
    if (!isInstalled) {
        bool wizardSetupSuccess = mf->OnStartUp();
        if (!wizardSetupSuccess) {
            return false;
        }
    }
    mf->Show(true);
    return true;
}

bool App::CreateLogsDirectory()
{
    wxString logDirectory(wxT("logs"));
    bool logDirectoryExists = wxDirExists(logDirectory);
    if (!logDirectoryExists) {
        bool success = wxMkDir(logDirectory);
        return success;
    }
    return true;
}

bool App::InitializeLogging()
{
#ifdef _DEBUG
    spdlog::set_level(spdlog::level::info);
#else
    spdlog::set_level(spdlog::level::warn);
#endif

    std::string logDirectory = std::string(Constants::LogsDirectory) + std::string("/") +
        std::string(Constants::LogsFilename);
    spdlog::flush_every(std::chrono::seconds(3));
    try {
        pLogger = spdlog::daily_logger_st(Constants::LoggerName, logDirectory);
        pLogger->info(Constants::Info::LoggerInitialized);
    } catch (const spdlog::spdlog_ex& e) {
        wxString error;
        error.Printf(wxT("Error initializing logger: %s"), e.what());
        wxMessageBox(error, wxT("Error"), wxOK_DEFAULT | wxICON_EXCLAMATION);
        return false;
    }

    return true;
}
bool App::DatabaseFileExists()
{
    bool dbFileExists = wxFileExists(wxT("tasks-tracker.db")); // FIXME: remove hardcoded string
    return dbFileExists;
}
bool App::IsInstalled()
{
    wxRegKey key(wxRegKey::HKLM, "SOFTWARE\\TasksTracker");

    if (key.Exists()) {
        long value = -1;
        key.QueryValue(wxT("Installed"), &value);
        return !!value;
    } else {
        key.Create();
        key.SetValue("Installed", 1);
        return false;
    }
    // FIXME need to figure out way to see if registry was potentially tampered with when running this
    // FIXME should app fix it or bomb out on the user?
}
} // namespace app

wxIMPLEMENT_APP(app::App);
