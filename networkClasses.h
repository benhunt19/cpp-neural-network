#pragma once

#include <iostream>
#include <vector>
#include <fstream>
#include <map>
#include <assert.h>
using namespace std;

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
	vector<map<char, float>> data_delta;
	int layer = 0;
	int index = 0;
	float tmp_value = 0.0f; // used to store value on an iteration through


	Neuron(vector<int>& shape, int& layer_index, int& node_index) {
		layer = layer_index;
		index = node_index;
		if (layer_index != 0 && layer_index < shape.size()) {
			// resize data and data_delta
			data.resize(shape[layer_index - 1]);
			data_delta.resize(shape[layer_index - 1]);
			for (int i = 0; i < shape[layer_index - 1]; i++) {
				data[i] = {
					{ 'w', uniformRandom(-1, 1) }, // weights
					{ 'b', uniformRandom(-1, 1) }  // bias
				};
				data_delta[i] = {
					{ 'w', 0 },				      // weights delta
					{ 'b', 0 }					  // bias delta
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

	void setWeight(int index, float value) {
		data[index]['w'] = value;
	};

	void setBias(int index, float value) {
		data[index]['b'] = value;
	};
	
	void setDeltaWeight(int index, float value) {
		data_delta[index]['w'] = value;
	};

	void setDeltaBias(int index, float value) {
		data_delta[index]['b'] = value;
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
		if (index < shape.size()) {
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
		for (int i = 0; i < layers[0].neurons.size(); i++) {
			layers[0].neurons[i].setValue(
				(float)image.image_data[i] / maxPixles // normalise pixles
			);
		}

		// loop over remaining layers...
		int count = 0;
		float z = 0; // generic term used in each term
		for (int j = 1; j < layers.size(); j++) {
			// loop over each neuron
			for (int k = 0; k < layers[j].neurons.size(); k++) {
				// loop over each link into the neuron...
				z = 0;
				for (int l = 0; l < layers[j].neurons[k].data.size(); l++) {
					// Z = W.*A + b
					z += layers[j].neurons[k].data[l]['w'] * layers[j - 1].neurons[k].tmp_value + layers[j].neurons[k].data[l]['b'];
				};
				layers[j].neurons[k].setValue(z);
			};
			// Potential activation functions:
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
				};
			}
			else if (layers[j].activation == "softMax") {
				vector<Neuron> tmp_neuron_vec = layers[j].neurons;
				for (int k = 0; k < layers[j].neurons.size(); k++) {
					layers[j].neurons[k].setValue(
						softMax(tmp_neuron_vec, k)
					);
				};
			};
		};

		// BACK PROPEGATION
		// Iterate through the layers
		// 1. Find the difference between the network and the actual on the final layer,
		// this will be: dz = A - Y
		int fin_index = layers.size() - 1;
		vector<float> dz_fin(layers[fin_index].neurons.size());
		for (int i = 0; i < layers[fin_index].neurons.size(); i++) {
			dz_fin[i] = layers[fin_index].neurons[i].tmp_value - (float)((int)image.label == i ? 1 : 0);
		};

		// 2. Find dw and db to see how much error was coming from the weights and bias
		// ... still on the final layer
		vector<float> dw_fin(layers[fin_index].neurons.size());
		vector<float> db_fin(layers[fin_index].neurons.size());
		// for each neuron 
		for (int x = 0; x < dz_fin.size(); x++) {
			// for each weight / bias
			for (int y = 0; y < layers[fin_index].neurons[x].data_delta.size(); y++) {
				layers[fin_index].neurons[x].setDeltaWeight(y, layers[fin_index - 1].neurons[y].tmp_value * dz_fin[y]);
				layers[fin_index].neurons[x].setDeltaBias(y, dz_fin[y]);
			};
		};
		// 3. process the remaining network layers...

	};

	void trainVector(vector<Image>& images) {
		cout << "Training on " << images.size() << " images..." << endl;
		for (auto image : images) {
			train(image);
		}
	}

};