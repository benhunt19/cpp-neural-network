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
	float z = 0.0f;         // used in forward and back propegation


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
		};
	};

	void setValue(float value) {
		tmp_value = value;
	};
	
	void setZ(float value) {
		z = value;
	};

	void printData() {
		for (int i = 1; i < data.size(); i++) {
			for (auto x : data[i]) {
				cout << x.first << ' ' << x.second << ' ';
			}
			cout << endl;
		};
	};

	void printDelta() {
		for (int i = 1; i < data_delta.size(); i++) {
			for (auto x : data_delta[i]) {
				cout << x.first << ' ' << x.second << ' ';
			}
			cout << endl;
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
	float alpha;
	NetworkLayer(vector<int>& shape, int& index, string activation_in, float alpha_in) {
		activation = activation_in;
		alpha = alpha_in;
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
	const float alpha = 0.1; // global learning rate
	NeuralNetwork(vector<int> shape) {
		for (int i = 0; i < shape.size(); i++) {
			if (i < shape.size() - 1) {
				// Use RElU activation for all but last layer
				layers.push_back(NetworkLayer(shape, i, RElU_const, alpha));
			}
			else {
				// use softmax for the final layer,
				// ensure it sums to one as in probability
				layers.push_back(NetworkLayer(shape, i, softMax_const, alpha));
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

		// --- FORWARD PROPAGATION ---
		// Initiate the zeroth layer...
		for (int i = 0; i < layers[0].neurons.size(); i++) {
			layers[0].neurons[i].setValue(
				(float)image.image_data[i] / maxPixles // normalise pixles [0, 1]
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
				layers[j].neurons[k].setZ(z);
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
						RElU(layers[j].neurons[k].z)
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

		// --- BACK PROPEGATION ---
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
		// working backward from the second last layer
		vector<float> dz;
		vector<float> dz_previous;
		dz_previous = dz_fin;
		float tmp = 0;
		if (fin_index - 1 > 0) {
			// for each remaining layer greater than the zeroth...
			for (int l = fin_index - 1; l > 0; l--) {
				// dz[i] = W[i+1] * dz[i+1] * g'(z[i]) // with i being the layer...
				dz.resize(layers[l].neurons.size());
				for (int x = 0; x < layers[l].neurons.size(); x++) {
					tmp = 0;
					for (int y = 0; y < layers[l + 1].neurons[x].data.size(); y++) {
						tmp += layers[l + 1].neurons[x].data[y]['w'] * dz_previous[y] * heaviside(layers[l].neurons[x].z);
					}
					dz[x] = tmp;
				}
				dz_previous = dz;

				// dw[i] = 1/m * dz[i] * X (input layer)
				// For all neurons on layer
				for (int n = 0; n < layers[l].neurons.size(); n++) {
					// for all weights on the neuron
					for (int w = 0; w < layers[l].neurons[n].data.size(); w++) {
						layers[l].neurons[n].data_delta[w]['w'] = 0;
						// for all values on the previous layer
						for (int v = 0; v < layers[l - 1].neurons.size(); v++) {
							layers[l].neurons[n].data_delta[w]['w'] += dz[n] * layers[l - 1].neurons[v].tmp_value;
						};
					};
				};
				// ...
				// db[i] = 1/m 8 sum(dz[i])
				// calculate dz
				float sumz = 0;
				for (int z = 0; z < dz.size(); z++) {
					sumz += dz[z];
				}
				// update change to biaz
				for (int b = 0; b < layers[l].neurons.size(); b++) {
					for (int n = 0; n < layers[l].neurons[b].data_delta.size(); n++) {
						layers[l].neurons[b].data_delta[n]['b'] = sumz;
					};
				};

			};

			// 4. Adjust the weights and biases of all layers apart from input / zeroth
			for (int l = 1; l < layers.size(); l++) {
				for (int n = 0; n < layers[l].neurons.size(); n++) {
					for (int w = 0; w < layers[l].neurons[n].data.size(); w++) {
						layers[l].neurons[n].setWeight(w, layers[l].neurons[n].data[w]['w'] - layers[l].alpha * layers[l].neurons[n].data_delta[w]['w']);
						layers[l].neurons[n].setBias(w, layers[l].neurons[n].data[w]['b'] - layers[l].alpha * layers[l].neurons[n].data_delta[w]['b']);
					};
				};
			};
		};
	};

	void trainVector(vector<Image>& images) {
		cout << "Training on " << images.size() << " images..." << endl;
		for (auto image : images) {
			train(image);
		}
	}

};