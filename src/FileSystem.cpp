#include "FileSystem.h"

#ifdef _WIN32
#include <Windows.h>
#endif

void ChangeWorkingDirectory()
{
#ifdef _WIN32
	char buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	//move into a string(just makes it easier
	string exeFullFilename(buffer);
	cout << "Exe Path & Filename " << exeFullFilename << endl;
	//now stripout the exe
	string exeDirectory = exeFullFilename.substr(0, exeFullFilename.find_last_of("\\"));
	cout << "Exe Directory " << exeDirectory << endl;
	SetCurrentDirectory(exeDirectory.c_str());
#endif
}