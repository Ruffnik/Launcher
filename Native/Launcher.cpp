#include "Windows.h"
#include <string>
#include <array>
#include <filesystem>
#include <cstdlib>
using namespace std;
using namespace filesystem;

#pragma region State
static path Launcher;
static path INI;
#pragma endregion

#pragma region Plumbing
wstring Remove(wstring Haystack, wchar_t Needle)
{
	Haystack.erase(remove(Haystack.begin(), Haystack.end(), Needle));
	return Haystack;
}

wstring SysErrorMessage(DWORD Error)
{
	LPTSTR Buffer = nullptr;
	if (FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		nullptr,
		Error,
		LANG_USER_DEFAULT,
		(LPTSTR)&Buffer,
		0, nullptr))
	{
		wstring Result = Buffer;
		LocalFree(Buffer);
		for (auto& Needle : { '\r', '\n' })
			Result = Remove(Result, Needle);
		return Result;
	}
	else
		return to_wstring(Error);
}

wstring GetValue(wstring Key)
{
	//Recommended max length
	array<wchar_t, 70> Buffer{};
	GetPrivateProfileString(
		L"Launcher",
		Key.c_str(),
		nullptr,
		Buffer.data(),
		static_cast<DWORD>(Buffer.size()),
		INI.c_str());
	return Buffer.data();
}

void Throw(DWORD Error)
{
	MessageBox(nullptr, SysErrorMessage(Error).c_str(), Launcher.stem().c_str(), MB_OK);
	exit(Error);
}
#pragma endregion

int WinMain(_In_ HINSTANCE Instance, _In_opt_ HINSTANCE PrevInstance, _In_ LPSTR CmdLine, _In_ int ShowCmd)
{
	Launcher = Remove(GetCommandLine(), '"');//TODO: validate (& fix) when passed parameters
	INI = Launcher.replace_extension("ini");
	if (!exists(INI))
		Throw(ERROR_FILE_NOT_FOUND);

	auto FileName = GetValue(L"FileName");
	if (FileName.empty())
		Throw(ERROR_PATH_NOT_FOUND);
	auto Arguments = GetValue(L"Arguments");
	if (32 >= reinterpret_cast<INT_PTR>(ShellExecute(nullptr, nullptr, FileName.c_str(), Arguments.empty() ? nullptr : Arguments.c_str(), Launcher.parent_path().c_str(), ShowCmd)))//learn.microsoft.com/windows/win32/api/shellapi/nf-shellapi-shellexecutew
		Throw(GetLastError());
}