#include <iostream>
#include <string>
#include <windows.h>
#include <processthreadsapi.h>
#pragma comment(lib, "winmm.lib")

using namespace std;

#define pageSize 4096 
#define numberOfPages 13 
#define numberOfReaders 7 
#define numberOfWriters 7 



int main() {
    string stringFileMappingName = "bufferMapping";
    wstring wstringFileMappingName = wstring(stringFileMappingName.begin(), stringFileMappingName.end());
    HANDLE fileMappingHandle = OpenFileMapping(GENERIC_WRITE, FALSE, L"bufferMapping");
    LPVOID fileMappingViewPointer = MapViewOfFile(fileMappingHandle, FILE_MAP_ALL_ACCESS, 0, 0, 0);

    HANDLE writingSemaphores[numberOfPages];
    HANDLE readingSemaphores[numberOfPages];
    for (int i = 0; i < numberOfPages; ++i) {
        string semaphoreName = "writingSemaphore" + to_string(i);
         
        writingSemaphores[i] = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, FALSE, semaphoreName.c_str());
        semaphoreName = "readingSemaphore" + to_string(i);
        readingSemaphores[i] = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, FALSE, semaphoreName.c_str());
    }
     
    FILE* logger;
    string loggerName = "C:\\Users\\asus\\Documents\\OS\\os_lab_4_1\\logFiles\\logReader" + to_string(GetCurrentProcessId()) + ".txt";
    fopen_s(&logger, loggerName.c_str(), "w");

    
    for (int i = 0; i < 3; ++i) {
         
        fprintf(logger, "Time:%d Status:0 \n", timeGetTime()); 

        int freePageIndex = WaitForMultipleObjects(
            numberOfPages,
            readingSemaphores,
            FALSE,
            INFINITE
        );
        
      
        fprintf(logger, "Time:%d Status:1 Page number: %d \n", timeGetTime(), freePageIndex);
        char* someData = new char[255];
        memcpy(
            someData,
            (LPVOID)((long long)fileMappingViewPointer + freePageIndex * pageSize),
            strlen(someData) * sizeof(char)
        );
        srand(time(0));
        Sleep(500 + rand() % 1000);
         
        fprintf(logger, "Time:%d Status:2 Page number:%d \n", timeGetTime(), freePageIndex);

        ReleaseSemaphore(writingSemaphores[freePageIndex], 1, NULL);
    }
     
    fclose(logger);
    for (int i = 0; i < numberOfPages; ++i) {
        CloseHandle(writingSemaphores[i]);
        CloseHandle(readingSemaphores[i]);
    }

    return 0;
}
