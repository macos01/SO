#include "list_recursive.h"

//list_files_recursive
void list_files_recursive(std::string init_path, HDC &hdc, int *x, int *y) {

	WIN32_FIND_DATAA ffd;
	HANDLE hFind;

	hFind = FindFirstFileA((init_path + "\\*").c_str(), &ffd);

	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			//if find a directory NOT . or ..
			if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				if (strcmp(ffd.cFileName, ".") != 0 && strcmp(ffd.cFileName, "..") != 0) {

					TextOut(hdc, *x, *y, "<DIR>", 6);
					TextOut(hdc, *x + 50, *y, ffd.cFileName, _tcslen(ffd.cFileName));

					*y += 20;
					*x += 20;
					list_files_recursive(init_path + "\\" + ffd.cFileName, hdc, x, y);
					*x -= 20;
				}
				//Do nothing
			}
			//if find a file
			else {
				TextOut(hdc, *x, *y, ffd.cFileName, _tcslen(ffd.cFileName));
				*y += 20;
			}
		} while (FindNextFile(hFind, &ffd) != 0);
	}

	FindClose(hFind);

	DWORD dwError = GetLastError();
	if (dwError != ERROR_NO_MORE_FILES)
	{
		TextOut(hdc,
			5, 5,
			"Error hapeneed", 17);
	}
}


