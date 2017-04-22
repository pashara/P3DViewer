#include "objectfile.h"
#include <iostream>
using namespace std;

objectfile::objectfile(string fileString, string filename) {
	this->fileString = fileString;
	this->fileName = filename;
}