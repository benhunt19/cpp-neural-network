#pragma once
#include <iostream>
#include <vector>
#include <fstream>

using namespace std;


class File {
public:
	int32_t num_rows = 0;
	int32_t num_cols = 0;
	int32_t num_images = 0;
	int32_t magic_number = 0;
	int32_t heaederOffset = 16;
	string filePath;
	string fName;
	int32_t image_size = 0;


	File(string path, string filename) {
		filePath = path;
		fName = filename;
		ifstream file((path).c_str(), ios::binary);
		file.open((path + filename).c_str(), ios::binary);
		if (!file.is_open()){
			cerr << "Error: Cannot open file " << filename << endl;
		}
		else {
			// Innit the file
			// First 4 bytes are a magic string
			file.read(reinterpret_cast<char*>(&magic_number), sizeof(magic_number));
			magic_number = swapEndian(magic_number); // Swap byte order manually
			cout << "Magic Number: " << magic_number << endl;

			// Second 4 bytes are the number of images
			file.read(reinterpret_cast<char*>(&num_images), sizeof(num_images));
			num_images = swapEndian(num_images);
			cout << "Number of Images: " << num_images << endl;

			// Third 4 bytes are the number of rows
			file.read(reinterpret_cast<char*>(&num_rows), sizeof(num_rows));
			num_rows = swapEndian(num_rows);
			cout << "Number of Rows: " << num_rows << endl;

			// Fourth 4 bytes are the number of columns
			file.read(reinterpret_cast<char*>(&num_cols), sizeof(num_cols));
			num_cols = swapEndian(num_cols);
			cout << "Number of Columns: " << num_cols << endl;

			// Calculate image size
			image_size = num_rows * num_cols;
			cout << "innit image_size" << image_size << endl;

			file.close();
		}
		
	}

	// processing function
	int32_t swapEndian(int32_t value) {
		return ((value & 0xFF000000) >> 24) | // Move byte 4 to byte 1
			((value & 0x00FF0000) >> 8) | // Move byte 3 to byte 2
			((value & 0x0000FF00) << 8) | // Move byte 2 to byte 3
			((value & 0x000000FF) << 24);  // Move byte 1 to byte 4
	};

	void printImageAtIndex(int image_index) {
		vector<uint8_t> image_data(image_size);
		ifstream file((filePath).c_str(), ios::binary);
		file.open((filePath + fName).c_str(), ios::binary);
		int offset = 16 + image_index * image_size; // Header + image offset
		file.seekg(offset, std::ios::beg); // Move to the specific image
		file.read(reinterpret_cast<char*>(image_data.data()), image_size);
		float tmp = 0;
		for (int x = 0; x < image_data.size(); x++) {
			tmp = image_data[x] == 0 ? 0 : 1;
			cout << tmp;
			if ((x + 1) % num_cols == 0) cout << endl;
		};
		cout << endl;
	};

	void getImage(int image_index) {
		vector<uint8_t> image_data(image_size);
		ifstream file((filePath).c_str(), ios::binary);
		file.open((filePath + fName).c_str(), ios::binary);
		int offset = 16 + image_index * image_size; // Header + image offset
		file.seekg(offset, std::ios::beg); // Move to the specific image
		file.read(reinterpret_cast<char*>(image_data.data()), image_size);
		//Image retImage;// = Image(image_data, num_rows, num_cols, 784); // review this
	}



};


class Image {
public:
	int32_t num_rows = 0;
	int32_t num_cols = 0;
	int32_t image_size = 0;
	vector<uint8_t> image_data = {};

	Image(vector<uint8_t> image_data_in, int32_t num_rows_in, int32_t num_cols_in, int32_t image_size_in) {
		image_data = image_data_in;
		image_size = image_size_in;
		num_rows = num_rows_in;
		num_cols = num_cols_in;
	};

	void print() {
		int32_t tmp = 0;
		for (int x = 0; x < image_data.size(); x++) {
			tmp = image_data[x] == 0 ? 0 : 1;
			cout << tmp;
			if ((x + 1) % image_size == 0) cout << endl;
		};
		cout << endl;
	};
};