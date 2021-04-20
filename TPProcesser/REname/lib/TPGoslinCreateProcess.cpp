// Code adapted from https://docs.microsoft.com/es-es/windows/win32/procthread/creating-processes

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <string>
#include <sstream>

#define TPGOSLIN_PATH "TPProcesser\\REname\\lib\\cppgoslin-1.1.2\\TPGoslin.exe"

void TPGoslinCreateProcess( std::string workDirPath )
{
    //LPSTR s = const_cast<char *>(workDirPath.c_str());
    std::string command = TPGOSLIN_PATH;
    command += " ";
    command += workDirPath;
    LPSTR s = const_cast<char *>(command.c_str());

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );

    // Start the child process. 
    if( !CreateProcess( NULL,   // No module name (use command line)
        s,        // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &si,            // Pointer to STARTUPINFO structure
        &pi )           // Pointer to PROCESS_INFORMATION structure
    ) 
    {
        printf( "CreateProcess failed (%d).\n", GetLastError() );
        return;
    }

    // Wait until child process exits.
    WaitForSingleObject( pi.hProcess, INFINITE );

    // Close process and thread handles. 
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );
}