#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#include <fileapi.h>
#include <synchapi.h>
#define MAX_BUFFER 1024
#elif
$compiler error
#endif

int main() {
	HANDLE hdir;
	DWORD dwWaitStatus;
	BOOL result = FALSE;
	char current_dir[MAX_PATH];

	if(!GetCurrentDirectoryA(MAX_PATH, current_dir)) {
		printf("GetCurrentDirectoryA failed (%lu)\n", GetLastError());
		return 1;
	}

	printf("%s", current_dir);

	hdir = FindFirstChangeNotificationA(current_dir,TRUE,
										FILE_NOTIFY_CHANGE_LAST_WRITE | 
										FILE_NOTIFY_CHANGE_FILE_NAME |
										FILE_NOTIFY_CHANGE_DIR_NAME |
										FILE_NOTIFY_CHANGE_ATTRIBUTES
										);

	if( hdir == INVALID_HANDLE_VALUE) {
		printf("FindFirstChangeNotification function failed, error (%lu)\n", GetLastError());
	}

	printf("start watching");

	BYTE Buffer[MAX_BUFFER];
	DWORD dwBytesReturned = 0;
	
	
	while(TRUE){
		dwWaitStatus = WaitForSingleObject(hdir, INFINITE);

		switch(dwWaitStatus) {
		case WAIT_OBJECT_0:

			result = ReadDirectoryChangesW(
										   hdir,
										   Buffer,
										   sizeof(Buffer),
										   TRUE,
										   FILE_NOTIFY_CHANGE_LAST_WRITE | 
										   FILE_NOTIFY_CHANGE_FILE_NAME |
										   FILE_NOTIFY_CHANGE_DIR_NAME |
										   FILE_NOTIFY_CHANGE_ATTRIBUTES,
										   &dwBytesReturned,
										   NULL,
										   NULL
										   );

			if(result) {
				DWORD dwOffset = 0;
				PFILE_NOTIFY_INFORMATION pNotifyInfo = (PFILE_NOTIFY_INFORMATION)Buffer;

				do {
					WCHAR wszFileName[MAX_PATH];
					lstrcpynW(wszFileName, pNotifyInfo->FileName, pNotifyInfo->FileNameLength / sizeof(WCHAR) + 1);

					WCHAR wszExt[_MAX_EXT];
					_wsplitpath_s(wszFileName, NULL, 0, NULL, 0, NULL, 0, wszExt, _MAX_EXT);
					printf("File: %ws, Extension: %ws\n", wszFileName, wszExt);

					if(_wcsicmp(wszExt, L".c") == 0) {
						system("buildcleaver.bat");
						break;
					}

					dwOffset += pNotifyInfo->NextEntryOffset;
					pNotifyInfo = (PFILE_NOTIFY_INFORMATION)(Buffer + dwOffset);
					
				} while (pNotifyInfo->NextEntryOffset != 0);

			}
			else{
				printf("ReadDirectoryChangesW function failed, error (%lu)\n", GetLastError());
				return 1;
			}

			if(!FindNextChangeNotification(hdir)) {
				printf("FindNextChangeNotification function failed, error (%lu)\n", GetLastError());
				return 1;
			}

			// build using the script
			break;
		case WAIT_TIMEOUT:
			printf("Wait timed out.\n");
		default:
			printf("Unhandled dwWaitStatus: %lu\n", dwWaitStatus);
		}

	}

	printf("end watching");
	return 0;
}