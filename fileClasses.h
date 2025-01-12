#pragma once

#include <iostream>
#include <vector>
#include <fstream>
#include <map>
#include <assert.h>
using namespace std;


// ---- GLOBAL FILE CLASSES -----


class Image {
	public:
		int32_t num_rows = 0;
		int32_t num_cols = 0;
		int32_t image_size = 0;
		vector<uint8_t> image_data = {};
		char label = ' ';

		// Innit image instance
		Image(vector<uint8_t> image_data_in, int32_t num_rows_in, int32_t num_cols_in, int32_t image_size_in)
		{
			image_data = image_data_in;
			image_size = image_size_in;
			num_rows = num_rows_in;
			num_cols = num_cols_in;
		};

		// Test image from front end
		Image(vector<uint8_t> image_data_in)
		{
			// for testing
			image_data = image_data_in;
			num_rows = 28;
			num_cols = 28;
		}

		// Print image to command line
		void print()
		{
			char tmp = ' ';
			for (int x = 0; x < image_data.size(); x++)
			{
				tmp = image_data[x] > 250 ? '*' : image_data[x] > 100 ? '.' : ' ';
				cout << tmp;
				if ((x + 1) % num_cols == 0) cout << endl;
			};
			cout << endl;
		};

		// Set label for training model
		void setLabel(char label_in)
		{
			label = label_in;
		};

		// Output pixel data
		void dispDataAsArray()
		{
			for (int x = 0; x < image_data.size(); x++)
			{
				cout << (int)image_data[x] << ",";
			}
		}
};


class File {
	public:
		// Innit file vars
		string filePath = "";
		string fName = "";
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
		int32_t swapEndian(int32_t value)
		{
			return ((value & 0xFF000000) >> 24) |
				((value & 0x00FF0000) >> 8) |
				((value & 0x0000FF00) << 8) |
				((value & 0x000000FF) << 24);
		};

		File(string path, string filename)
		{
			filePath = path;
			fName = filename;
			ifstream file((path).c_str(), ios::binary);
			file.open((path + filename).c_str(), ios::binary);
			if (!file.is_open())
			{
				cerr << "Error: Cannot open file " << filename << endl;
			}
			else
			{
				// Innit the file
				// First 4 bytes are a magic string
				file.read(reinterpret_cast<char*>(&magic_number), sizeof(magic_number));
				magic_number = swapEndian(magic_number); // Swap byte order manually

				// Magic number determines the content of the file
				if (magic_number == 2051)
				{

					type = "images";

					// Second 4 bytes are the number of images
					file.read(reinterpret_cast<char*>(&num_images), sizeof(num_images));
					num_images = swapEndian(num_images);

					// Third 4 bytes are the number of rows
					file.read(reinterpret_cast<char*>(&num_rows), sizeof(num_rows));
					num_rows = swapEndian(num_rows);

					// Fourth 4 bytes are the number of columns
					file.read(reinterpret_cast<char*>(&num_cols), sizeof(num_cols));
					num_cols = swapEndian(num_cols);

					cout << "File Type: " << type;
					cout << ", Images: " << num_images;
					cout << ", Rows: " << num_rows;
					cout << ", Cols: " << num_cols << endl;

					// Calculate image size
					image_size = num_rows * num_cols;
				}
				else if (magic_number == 2049) {
					type = "labels";
					file.read((char*)&num_labels, sizeof(num_labels));
					num_labels = swapEndian(num_labels);
					cout << "File Type: " << type;
					cout << ", Labels: " << num_labels << endl;
				}
				else {
					cout << "File is corrupted" << endl;
				}
				file.close();
			};

		};

		// Return image at an index in dataset
		Image getImage(int image_index)
		{
			assert(type == "images");
			vector<uint8_t> image_data(image_size);
			ifstream file((filePath).c_str(), ios::binary);
			file.open((filePath + fName).c_str(), ios::binary);
			int offset = image_offset + image_index * image_size; // Header + image offset
			file.seekg(offset, std::ios::beg); // Move to the specific image
			file.read(reinterpret_cast<char*>(image_data.data()), image_size);
			file.close();
			return Image(image_data, num_rows, num_cols, image_size); // review this;
		};

		// Print image at an index
		void printImageAtIndex(int image_index)
		{
			assert(type == "images");
			Image imageToPrint = getImage(image_index);
			imageToPrint.print();
		};

		// Efficiently get images from data from one index to another
		vector<Image> getImageVector(int start_index, int end_index)
		{
			assert(type == "images");
			vector<Image> return_vec = {};
			vector<uint8_t> image_data(image_size);
			ifstream file((filePath).c_str(), ios::binary);
			int offset;

			file.open((filePath + fName).c_str(), ios::binary);
			for (int index = start_index; index < end_index; index++)
			{
				offset = image_offset + (index * image_size); // Header + image offset
				file.seekg(offset, std::ios::beg);
				file.read(reinterpret_cast<char*>(image_data.data()), image_size);
				return_vec.push_back(Image(image_data, num_rows, num_cols, image_size));
			};
			file.close();

			return return_vec;

		};

		// Get label at index from label file
		char getLabelAtIndex(int label_index)
		{
			assert(type == "labels");
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

		// Efficiently get labels from label file and return vector
		vector<char> getLabelVector(int start_index, int end_index)
		{
			assert(type == "labels");
			vector<char> return_vec;
			ifstream file((filePath).c_str(), ios::binary);
			int offset;
			char label;
			file.open((filePath + fName).c_str(), ios::binary);
			for (int index = start_index; index < end_index; index++)
			{
				offset = label_offset + (index * 1); // Label offset and index, (chars are 1 byte) 
				file.seekg(offset, ios::beg);
				file.read(&label, 1);
				return_vec.push_back(label);
			}
			file.close();
			return return_vec;
		};

};
