#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <tchar.h>
#include <string>
#include <iostream>
using namespace std;

void list_files_recursive(string init_path);

int main(int argc, char *argv[]) {

	fprintf(stdout, "**** List all files where the exe is located **** \n");

	cout << "Press Enter to Continue";
	cin.ignore();
	
	//get the current path
	TCHAR NPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, NPath);

	//call function
	list_files_recursive(NPath);

	cout << "Press Enter para exit \n";
	cin.ignore();

	return 0;
}

//list_files_recursive.
void list_files_recursive(string init_path) {

	WIN32_FIND_DATA ffd;
	HANDLE hFind;

	cout << init_path << "\n";

	hFind = FindFirstFileA((init_path + "\\*").c_str(), &ffd);

	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			//if find a directory NOT . or ..
			if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY && strcmp(ffd.cFileName, ".") != 0
				&& strcmp(ffd.cFileName, "..") != 0) {

				cout << "<DIR> " << ffd.cFileName << "\n";

				//list files on init_path + "\\" + (folder = ffd.cFileName)
				list_files_recursive(init_path + "\\" + ffd.cFileName);
			}

			//if find a file
			else {
				cout << ffd.cFileName << "\n";
			}

		} while (FindNextFile(hFind, &ffd) != 0);
	}
}



