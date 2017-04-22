#pragma once
#include <iostream>;

class objectfile {
	std::string fileString;
	std::string fileName;
public:
	objectfile(std::string fileString, std::string filename);
	std::string getTitle() {
		return this->fileName;
	}
	std::string getPath() {
		return this->fileString;
	}
};
