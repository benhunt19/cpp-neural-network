#pragma once
#include <iostream>
#include <vector>
#include <fstream>
#include <map>
#include <assert.h>
using namespace std;



// ---- GLOBAL CLASSES -----


class Image {
public:
	int32_t num_rows = 0;
	int32_t num_cols = 0;
	int32_t image_size = 0;
	vector<uint8_t> image_data = {};
	char label = ' ';

	Image(vector<uint8_t> image_data_in, int32_t num_rows_in, int32_t num_cols_in, int32_t image_size_in) {
		image_data = image_data_in;
		image_size = image_size_in;
		num_rows = num_rows_in;
		num_cols = num_cols_in;
	};

	void print() {
		char tmp = ' ';
		for (int x = 0; x < image_data.size(); x++) {
			tmp = image_data[x] > 250 ? '*' : image_data[x] > 100 ? '.' : ' ';
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
	string type; // 'images' or  'labels'
	int32_t magic_number = 0;
	// Image Data
	int32_t num_rows;
	int32_t num_cols;
	int32_t num_images;
	int32_t image_offset = 16;
	int32_t image_size;
	// Label data
	int32_t num_labels;
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
				cout << "File Type: " << type;
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
				cout << "File Type: " << type;
				file.read((char*)&num_labels, sizeof(num_labels));
				num_labels = swapEndian(num_labels);
				cout << "Num labels: " << num_labels << endl;
			}
			file.close();
		}
		
	}

	Image getImage(int image_index) {
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


	void printImageAtIndex(int image_index) {
		assert(type == "images");
		Image imageToPrint = getImage(image_index);
		imageToPrint.print();
	};

	vector<Image> getImageVector(int start_index, int end_index) {
		assert(type == "images");
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

	vector<char> getLabelVector(int start_index, int end_index) {
		assert(type == "labels");
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



// potentially move this to it's own file

class Neuron {
	// the weights and biases between a node on 
	// layer on i and i+1  are stored on the 
	// neuron on layer i + 1, this will be the length
	// of ith layer
public:
	// The structure of the weights/bias will be 
	//[
	//	{
	//		'w': 0.45,
	//		'b': 0.88
	//	},
	//	{
	//		'w': 0.15,
	//		'b': 0.80
	//	},
	// ... etc...
	//] 
	// 
	vector<map<char, float>> data;
	int layer;
	int index;
	float tmp_value = 0.0f; // used to store value on an iteration through
	
	Neuron(vector<int>& shape, int& layer_index, int& node_index) {
		layer = layer_index;
		index = node_index;
		if (layer_index != 0 && layer_index < shape.size()) {
			data.resize(shape[layer_index - 1]);
			for (int i = 0; i < shape[layer_index - 1]; i++) {
				data[i] = {
					{ 'w', uniformRandom(-1, 1) }, // weights
					{ 'b', uniformRandom(-1, 1) }  // bias
				}; 
			};

		}
	};

	void setValue(float value) {
		tmp_value = value;
	};

	void printData() {
		for (int i = 1; i < data.size(); i++) {
			for (auto x : data[i]) {
				cout << x.first << ' ' << x.second << ' ';
			}
			cout << "\n";
		};
	};

	void calculateValue() {

	}

};

class NetworkLayer {
	// A vector of neurons as well as the activation 
	// we will use in forward propigation
public:
	vector<Neuron> neurons;
	string activation;
	NetworkLayer(vector<int>& shape, int& index, string activation_in) {
		activation = activation_in;
		if (index < shape.size() ) {
			for (int i = 0; i < shape[index]; i++) {
				neurons.push_back(Neuron(shape, index, i));
			}
		}
	}
};

class NeuralNetwork {
	// A vector of Network Layers
	// we will define the majority of the processing 
	// functions at this level
public:
	vector<NetworkLayer> layers;
	// to initialise the NN
	NeuralNetwork(vector<int> shape) {
		for (int i = 0; i < shape.size(); i++) {
			if (i < shape.size() - 1) {
				// Use RElU activation for all but last layer
				layers.push_back(NetworkLayer(shape, i, RElU_const));
			}
			else {
				// use softmax for the final layer,
				// ensure it sums to one as in probability
				layers.push_back(NetworkLayer(shape, i, softMax_const));
			}
			
		};
	};

	void printNetwork() {
		for (auto layer : layers) {
			for (auto neuron : layer.neurons) {
				cout << "layer: " << neuron.layer << " ,index: " << neuron.index << endl;
			}
		}
	}

	void printNodeValues(int layer, int neuron) {
		cout << "Layer: " << layer << " ,index: " << neuron << endl;
		layers[layer].neurons[neuron].printData();
	}

	void printLayerVals(int layer_index) {
		cout << "Current node values for layer: " << layer_index << endl;
		for (int i = 0; i < layers[layer_index].neurons.size(); i++) {
			cout << layers[layer_index].neurons[i].tmp_value << endl;
		}
	}

	void train(Image& image) {
		cout << "Training data on: " << endl;
		image.print();

		// FORWARD PROPAGATION

		// Initiate the zeroth layer...
		for (int i = 0; i < layers[0].neurons.size() ; i++) {
			layers[0].neurons[i].setValue(
				(float)image.image_data[i] / maxPixles // normalise pixles
			);
		}

		// loop over remaining layers...
		int count = 0;
		float z = 0; // generic term used in each term
		for (int j = 1; j < layers.size();  j++) {
			// loop over each neuron
			for (int k = 0; k < layers[j].neurons.size(); k++) {
				// loop over each link into the neuron...
				z = 0;
				for (int l = 0; l < layers[j].neurons[k].data.size() ; l++) {
					// Z = W.*A + b
					z += layers[j].neurons[k].data[l]['w'] * layers[j - 1].neurons[k].tmp_value + layers[j].neurons[k].data[l]['b'];
				}
				layers[j].neurons[k].setValue(z);
			}
			// once neurons are populated, we can adjust using activation function
			// potentially could have 
			// 'RElU'
			// 'sigmoid'
			// 'tanh'
			// 'softMax'
			// Activation function processing:
			if (layers[j].activation == "RElU") {
				for (int k = 0; k < layers[j].neurons.size(); k++) {
					layers[j].neurons[k].setValue(
						RElU(layers[j].neurons[k].tmp_value)
					);
				}
			}
			else if (layers[j].activation == "softMax") {
				for (int k = 0; k < layers[j].neurons.size(); k++) {
					layers[j].neurons[k].setValue(
						softMax(layers[j].neurons, k)
					);
				}
			}
		};
		// BACK PROPEGATION

	}

	void trainVector(vector<Image>& images) {
		for (auto image : images) {
			train(image);
		}
	}

};