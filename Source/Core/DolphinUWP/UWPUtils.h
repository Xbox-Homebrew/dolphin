#pragma once

#include <Common/FileUtil.h>
#include <Core/HW/Wiimote.h>

#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Storage.Pickers.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.ApplicationModel.Activation.h>
#include <winrt/Windows.ApplicationModel.Core.h>
#include <winrt/Windows.UI.Core.h>

#include <ppltasks.h>
#include <Core/Core.h>
#include <Core/HW/WiimoteEmu/WiimoteEmu.h>
#include <Core/HW/DVD/DVDInterface.h>

using namespace winrt;
using namespace winrt::Windows::Storage::Pickers;
using namespace Windows::ApplicationModel::Core;

namespace UWP
{
inline std::string GetUserLocation()
{
  std::string user_path = 
      winrt::to_string(winrt::Windows::Storage::ApplicationData::Current().LocalFolder().Path()) +
      "/user.txt";
  if (File::Exists(user_path))
  {
    std::ifstream t(user_path);
    std::stringstream buffer;
    buffer << t.rdbuf();

    return buffer.str();
  }
  else
  {
    return winrt::to_string(
        winrt::Windows::Storage::ApplicationData::Current().LocalFolder().Path());
  }
}

#pragma warning(push)
#pragma warning(disable : 4265)
inline winrt::fire_and_forget OpenNewUserPicker()
{
  std::string user_path =
      winrt::to_string(winrt::Windows::Storage::ApplicationData::Current().LocalFolder().Path()) +
      "/user.txt";
  FolderPicker openPicker;
  openPicker.ViewMode(PickerViewMode::List);
  openPicker.SuggestedStartLocation(PickerLocationId::ComputerFolder);

  auto folder = co_await openPicker.PickSingleFolderAsync();
  if (folder)
  {
    std::ofstream t(user_path);
    t << winrt::to_string(folder.Path().data());
  }
}


inline void SetNewUserLocation()
{
  CoreApplication::MainView().CoreWindow().Dispatcher().RunAsync(
      winrt::Windows::UI::Core::CoreDispatcherPriority::Normal, []
      {
        OpenNewUserPicker();
      });
}

inline winrt::fire_and_forget OpenDiscPicker()
{
  FileOpenPicker openPicker;
  openPicker.ViewMode(PickerViewMode::List);
  openPicker.SuggestedStartLocation(PickerLocationId::HomeGroup);
  openPicker.FileTypeFilter().Append(L".iso");
  openPicker.FileTypeFilter().Append(L".ciso");
  openPicker.FileTypeFilter().Append(L".rvz");
  openPicker.FileTypeFilter().Append(L".wbfs");
  openPicker.FileTypeFilter().Append(L".gcm");
  openPicker.FileTypeFilter().Append(L".gcz");
  openPicker.FileTypeFilter().Append(L".json");
  openPicker.FileTypeFilter().Append(L".elf");
  openPicker.FileTypeFilter().Append(L".dol");
  openPicker.FileTypeFilter().Append(L".tgc");
  openPicker.FileTypeFilter().Append(L".wad");

  auto file = co_await openPicker.PickSingleFileAsync();
  if (file)
  {
    Core::RunAsCPUThread(
        [&file] { DVDInterface::ChangeDisc(winrt::to_string(file.Path().data())); });
  }
}

inline void PickDisc()
{
  CoreApplication::MainView().CoreWindow().Dispatcher().RunAsync(
      winrt::Windows::UI::Core::CoreDispatcherPriority::Normal, [] { OpenDiscPicker(); });
}
#pragma warning(pop)

inline void ResetUserLocation()
{
  std::string user_path =
      winrt::to_string(winrt::Windows::Storage::ApplicationData::Current().LocalFolder().Path()) +
      "/user.txt";

  std::ofstream t(user_path);
  t << winrt::to_string(winrt::Windows::Storage::ApplicationData::Current().LocalFolder().Path());
}

} // namespace UWP
