
#include "FindFiles.h"
#include "dirent.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
using namespace std;

#ifdef WIN32
#define DIRSPLIT '\\'
#else
#define DIRSPLIT '/'
#endif

string toUpper(const string& Instr)
{
	string str = Instr;
	int n = str.size();
	char c = 'a';
	for (int i = 0; i < n; i++)
	{
		c = str.at(i);
		if (c >= 'a'&&c <= 'z')
		{
			str.at(i) = c + 'A' - 'a';
		}
	}
	return str;
}

void LocalCheckDir(string &sDir)
{
	if (sDir.size() < 1)
		return;
	if (sDir[sDir.size() - 1] != DIRSPLIT)
		sDir += DIRSPLIT;

}

/* Find files and subdirectories recursively */
int
find_directory(
const char *dirname)
{
	DIR *dir;
	char buffer[PATH_MAX + 2];
	char *p = buffer;
	const char *src;
	char *end = &buffer[PATH_MAX];
	int ok;

	/* Copy directory name to buffer */
	src = dirname;
	while (p < end  &&  *src != '\0') {
		*p++ = *src++;
	}
	*p = '\0';

	/* Open directory stream */
	dir = opendir(dirname);
	if (dir != NULL) {
		struct dirent *ent;

		/* Print all files and directories within the directory */
		while ((ent = readdir(dir)) != NULL) {
			char *q = p;
			char c;

			/* Get final character of directory name */
			if (buffer < q) {
				c = q[-1];
			}
			else {
				c = ':';
			}

			/* Append directory separator if not already there */
			if (c != ':'  &&  c != '/'  &&  c != '\\') {
				*q++ = '/';
			}

			/* Append file name */
			src = ent->d_name;
			while (q < end  &&  *src != '\0') {
				*q++ = *src++;
			}
			*q = '\0';

			/* Decide what to do with the directory entry */
			switch (ent->d_type) {
			case DT_LNK:
			case DT_REG:
				/* Output file name with directory */
				printf("%s\n", buffer);
				break;

			case DT_DIR:
				/* Scan sub-directory recursively */
				if (strcmp(ent->d_name, ".") != 0
					&& strcmp(ent->d_name, "..") != 0) {
					find_directory(buffer);
				}
				break;

			default:
				/* Ignore device entries */
				/*NOP*/;
			}

		}

		closedir(dir);
		ok = 1;

	}
	else {
		/* Could not open directory */
		printf("Cannot open directory %s\n", dirname);
		ok = 0;
	}

	return ok;
}
int find_directory(const string &basepath, const string &subdir,const vector<string> &extends, vector<string> &filenames, vector<string> *pdirnames)
{
	string fullpath = basepath;
	LocalCheckDir(fullpath);

	string relDir = subdir;
	
	LocalCheckDir(relDir);

	fullpath += relDir;


	DIR *dir = opendir(fullpath.c_str());
	if (dir != NULL) {
		struct dirent *ent;
		bool bAdded = false;
		/* Print all files and directories within the directory */
		while ((ent = readdir(dir)) != NULL) {

			string szTmp = ent->d_name;
			switch (ent->d_type) {
			case DT_LNK:
			case DT_REG:
				/* Output file name with directory */
				bAdded = false;
				for (size_t u = 0; u < extends.size(); u++)
				{
					size_t nPos = szTmp.rfind(extends[u]);
					if (nPos != string::npos && szTmp.size() - nPos == extends[u].size())
					{
						bAdded = true;
						break;
					}
				}
				if (bAdded)
				{
					string ss = relDir + szTmp;
					filenames.push_back(ss);
				}
			break;

			case DT_DIR:
				/* Scan sub-directory recursively */
				if (strcmp(ent->d_name, ".") != 0
					&& strcmp(ent->d_name, "..") != 0) {

					string ss = relDir + szTmp;
					LocalCheckDir(ss);
					if (pdirnames)
						pdirnames->push_back(ss);
					find_directory(basepath,ss,extends,filenames,pdirnames);
				}
				break;

			default:
				/* Ignore device entries */
				/*NOP*/;
			}

		}
		closedir(dir);
		return 1;
	}
	return 0;
}
int MyFindFile(string szPath, vector<string> szFilters, vector<string> &filenames, vector<string> *pdirnames)
{
	int nSize = szFilters.size();
	for (size_t i = 0; i < nSize; i++)
	{
		szFilters.push_back(toUpper(szFilters[i]));
	}
	filenames.clear();
	if (pdirnames)
		pdirnames->clear();

	return find_directory(szPath, "", szFilters, filenames, pdirnames);
}

void FileWrite(const cv::Mat &img, std::vector<cv::KeyPoint> &keyPoint, const cv::Mat &src, const std::string &filename){
	cv::FileStorage fs(filename, cv::FileStorage::WRITE);
	cv::write(fs, "image", img);
	cv::write(fs, "KeyPoint", keyPoint);
	cv::write(fs, "Mat", src);
	fs.release();
}

void FileRead(cv::Mat &img, std::vector<cv::KeyPoint> &keyPoint, cv::Mat &dst, std::string &filename){
	cv::FileStorage fs(filename, cv::FileStorage::READ);
	cv::read(fs["image"], img);
	cv::read(fs["KeyPoint"], keyPoint);
	cv::read(fs["Mat"], dst);
	fs.release();
}
std::string absolute_name(std::string input)
{
	auto pos = input.rfind(".");
	string base_name = input.substr(0, pos);
	return base_name;
}