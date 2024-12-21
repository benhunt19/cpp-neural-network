#pragma once
#include <iostream>
#include <vector>
#include <fstream>

using namespace std;


class Image {
public:
	int32_t num_rows = 0;
	int32_t num_cols = 0;
	int32_t image_size = 0;
	vector<uint8_t> image_data = {};
	char label;

	Image(vector<uint8_t> image_data_in, int32_t num_rows_in, int32_t num_cols_in, int32_t image_size_in) {
		image_data = image_data_in;
		image_size = image_size_in;
		num_rows = num_rows_in;
		num_cols = num_cols_in;
	};

	void print() {
		char tmp = 0;
		for (int x = 0; x < image_data.size(); x++) {
			tmp = image_data[x] == 0 ? ' ' : '*';
			cout << tmp;
			if ((x + 1) % num_cols == 0) cout << endl;
		};
		cout << endl;
	};

	void setLabel(char label_in) {
		label = label_in;
	};
};


class File {
public:
	string filePath;
	string fName;
	string type = ""; // 'images' or  'labels'
	int32_t magic_number = 0;
	// Image Data
	int32_t num_rows = 0;
	int32_t num_cols = 0;
	int32_t num_images = 0;
	int32_t image_offset = 16;
	int32_t image_size = 0;
	// Label data
	int32_t num_labels = 0;
	int32_t label_offset = 8;

	// processing function
	int32_t swapEndian(int32_t value) {
		return ((value & 0xFF000000) >> 24) |
			((value & 0x00FF0000) >> 8) |
			((value & 0x0000FF00) << 8) |
			((value & 0x000000FF) << 24);
	};


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

			if (magic_number == 2051) {
				type = "images";
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
				cout << "innit image_size: " << image_size << endl;
			}
			else if (magic_number == 2049)  {
				type = "labels";
				file.read((char*)&num_labels, sizeof(num_labels));
				num_labels = swapEndian(num_labels);
				cout << "Num labels: " << num_labels << endl;
			}
			file.close();
		}
		
	}

	Image getImage(int image_index) {
		if (type != "images") throw runtime_error("File Type Not Images");
		vector<uint8_t> image_data(image_size);
		ifstream file((filePath).c_str(), ios::binary);
		file.open((filePath + fName).c_str(), ios::binary);
		int offset = image_offset + image_index * image_size; // Header + image offset
		file.seekg(offset, std::ios::beg); // Move to the specific image
		file.read(reinterpret_cast<char*>(image_data.data()), image_size);
		file.close();
		return Image(image_data, num_rows, num_cols, image_size); // review this;
	};


	void printImageAtIndex(int image_index) {
		if (type != "images") throw runtime_error("File Type Not Images");
		Image imageToPrint = getImage(image_index);
		imageToPrint.print();
	};

	vector<Image> getImageVector(int start_index, int end_index) {
		if (type != "images") throw runtime_error("File Type Not Images");
		vector<Image> return_vec = {};
		vector<uint8_t> image_data(image_size);
		ifstream file((filePath).c_str(), ios::binary);
		int offset;

		file.open((filePath + fName).c_str(), ios::binary);
		for (int index = start_index; index < end_index; index++) {
			offset = image_offset + (index * image_size); // Header + image offset
			file.seekg(offset, std::ios::beg);
			file.read(reinterpret_cast<char*>(image_data.data()), image_size);
			return_vec.push_back(Image(image_data, num_rows, num_cols, image_size));
		}
		file.close();

		return return_vec;

	};

	char getLabelAtIndex(int label_index) {
		char label;
		ifstream file((filePath).c_str(), ios::binary);
		file.open((filePath + fName).c_str(), ios::binary);
		int offset = label_offset + label_index; // Header + label offset
		file.seekg(offset, ios::beg); // Move to the label
		file.read(&label, 1);
		file.close();
		cout << int(label) << endl;
		return label;
	};

	vector<char> getLabelVector(int start_index, int end_index) {
		if (type != "labels") throw runtime_error("File Type Not Images");
		vector<char> return_vec;
		ifstream file((filePath).c_str(), ios::binary);
		int offset;
		char label;
		file.open((filePath + fName).c_str(), ios::binary);
		for (int index = start_index; index < end_index; index++) {
			offset = label_offset + (index * 1); // Label offset and index, (chars are 1 byte) 
			file.seekg(offset, ios::beg);
			file.read(&label, 1);
			return_vec.push_back(label);
		}
		file.close();
		return return_vec;
	}

};

class NetworkLayer {
public:

};



// potentially move this to it's own file
class NeuralNetwork {
public:

};