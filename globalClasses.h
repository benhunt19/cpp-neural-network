#pragma once
#include <iostream>
#include <vector>

using namespace std;

class TestClass {

	public:
		int number;
		TestClass() {
			cout << "you have created a new test class" << endl;
		}
		TestClass(int num) {
			cout << "you have created a test class with an integer param..." << endl;
			number = num;
			cout << "number: " << number << endl;
		}
		static void testStatic() {
			cout << "This is the output of a static class to see if we can get it anywhere..." << endl;
		}
};

class FileUtils {
	// review this
vector<vector<unsigned char>> training_images;
vector<unsigned long> training_labels;
vector<vector<unsigned char>> testing_imagesl;
vector<unsigned long> testing_labels;
public:
	static ifstream getFileFromPath(string path, string filename) {
		ifstream file1((path).c_str(), ios::binary);
		file1.open((path + filename).c_str(), ios::binary);
		string fileOpen = file1.is_open() ? "True" : "False";
		cout << "Is the file open? " << fileOpen << endl;
		long int magic;
		file1.read((char*)&magic, sizeof(magic));
		return file1;
	};
};