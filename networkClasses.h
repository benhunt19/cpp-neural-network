#pragma once

#include <iostream>
#include <vector>
#include <fstream>
#include <assert.h>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

class Neuron {
	// the weights and biases between a node on 
	// layer on i and i+1  are stored on the 
	// neuron on layer i + 1, this will be the length
	// of ith layer
	public:
		// The structure of the weights/bias
		//[
		//	{
		//		'w': 0.45,
		//		'dw': 0.02,
		//	},
		// ...
		//] 
		// 
		struct connection {
			float w;
			float dw;
		};
		vector<connection> data;    // array of weights and weight deltas for back prop
		float value = 0.0f;	// used o store activation through each iteration
		float z = 0.0f;				// used in forward and back propegation
		float b = 0.0f;				// bias term
		float db = 0.0f;			// change in bias term


		Neuron(vector<int>& shape, int& layer_index, int& node_index) {
			if (layer_index != 0 && layer_index < shape.size()) {
				// resize data and data_delta
				b = uniformRandom(-0.5, 0.5);
				db = 0;
				data.resize(shape[layer_index - 1]); // one weight per neuron in previous layer
				for (int i = 0; i < shape[layer_index - 1]; i++) {
					data[i].w = uniformRandom(-0.5, 0.5);
					data[i].dw = 0;
				};
			};
		};

		void setValue(float value_in) {
			value = value_in;
		};
	
		void setZ(float value) {
			z = value;
		};

		void printData() {
			cout << "B: " << b << "db : " << db << endl;
			cout << "Weights: " << endl;
			for (int i = 1; i < data.size(); i++) {
				cout << data[i].w << " " << data[i].dw << endl;
			};
		};

		void setWeight(int index, float value) {
			data[index].w = value;
		};

		void setBias(float value) {
			b = value;
		};

		void setDeltaWeight(int index, float value) {
			data[index].dw = value;
		};

		void setDeltaBias(float value){
			db = value;
		};

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
				};
			};
		};
};

class NeuralNetwork {
	// A vector of Network Layers
	// we will define the majority of the processing 
	// functions at this level
	public:
		vector<NetworkLayer> layers;
		// to initialise the NN
		float alpha = 0.2;       // global learning rate
		float accuracy = 0;      // to be set once trained
		NeuralNetwork(vector<int> shape, string activation, float alpha_in) {
			alpha = alpha_in;
			for (int i = 0; i < shape.size(); i++) {
				if (i < shape.size() - 1) {
					// determine activation function for hidden layers
					// input layer (zeroth) doesn't need an activation
					layers.push_back(NetworkLayer(shape, i, i > 0 ? activation : "", alpha));
				} else {
					// use softmax for the final / output layer,
					// ensure it sums to one as in probability
					layers.push_back(NetworkLayer(shape, i, softMax_const, alpha));
				};

			};
		};

		// print values of each neuron on a certain layer
		void printLayerVals(int layer_index) {
			cout << "Current node values for layer: " << layer_index << endl;
			for (int i = 0; i < layers[layer_index].neurons.size(); i++) {
				cout << i << ": " << layers[layer_index].neurons[i].value << endl;
			}
		};

		// Print current Z value for all neurons on a certain layer
		void printZVals(int layer_index) {
			cout << "Current Z node values for layer: " << layer_index << endl;
			for (int i = 0; i < layers[layer_index].neurons.size(); i++) {
				cout << layers[layer_index].neurons[i].z << endl;
			}
		};

		// Reset current gradients (dw) for all neurons
		void resetGradients() {
			for (int l = 1; l < layers.size(); l++) {
				for (int n = 0; n < layers[l].neurons.size(); n++) {
					layers[l].neurons[n].setDeltaBias(0);
					for (int w = 0; w < layers[l].neurons[n].data.size(); w++) {
						layers[l].neurons[n].setDeltaWeight(w, 0);
					};
				};
			};
		};

		// Divide current gradients by batch size for adjustment
		void averageGradients(int batch_size) {
			for (int l = 1; l < layers.size(); ++l) {
				for (auto& neuron : layers[l].neurons) {
					neuron.db /= batch_size;
					for (auto& obj : neuron.data) {
						obj.dw /= batch_size;
					};
				};
			};
		};

		// Get the networks suggested output for once an input has been run
		int getOutputResults() {
			int fin_index = layers.size() - 1;
			int layer_size = layers[fin_index].neurons.size();
			vector<float> values(layer_size);
			for (int i = 0; i < layer_size; i++) {
				values[i] = layers[fin_index].neurons[i].value;
			};
			vector<float>::iterator max = max_element(values.begin(), values.end());
			int max_index = distance(values.begin(), max);
			return max_index;
		};

		void forwardPropagation(Image& image) {
			// --- FORWARD PROPAGATION ---
			// Initiate the zeroth layer...
			for (int i = 0; i < layers[0].neurons.size(); i++) {
				layers[0].neurons[i].setValue(
					(float)image.image_data[i] / maxPixels // normalise pixles [0, 1]
				);
			};

			// loop over remaining layers...
			int count = 0;
			float z = 0; // generic term used for calculation
			for (int j = 1; j < layers.size(); j++) {
				// loop over each neuron
				for (int k = 0; k < layers[j].neurons.size(); k++) {
					// loop over each link into the neuron...
					z = 0;
					// Z = W.*A + b
					for (int l = 0; l < layers[j].neurons[k].data.size(); l++) {
						z += layers[j].neurons[k].data[l].w * layers[j - 1].neurons[l].value;
					};
					z += layers[j].neurons[k].b; // add bias component
					layers[j].neurons[k].setZ(z);
				};
				// Potential activation functions:
				// 'RElU'
				// 'sigmoid'
				// 'tanh'
				// 'softMax'
				// Activation function processing:
				if (layers[j].activation == RElU_const) {
					for (int k = 0; k < layers[j].neurons.size(); k++) {
						layers[j].neurons[k].setValue(
							RElU(layers[j].neurons[k].z)
						);
					};
				} else if (layers[j].activation == softMax_const) {
					vector<Neuron> tmp_neuron_vec = layers[j].neurons;
					for (int k = 0; k < layers[j].neurons.size(); k++)
					{
						layers[j].neurons[k].setValue(
							softMax(tmp_neuron_vec, k)
						);
					};
				} else if (layers[j].activation == sigmoid_const) {
					for (int k = 0; k < layers[j].neurons.size(); k++) {
						layers[j].neurons[k].setValue(
							sigmoid(layers[j].neurons[k].z)
						);
					};
				};
			};
		};

		void backwardPropagation(Image& image) {
			// --- BACK PROPAGATION ---
			// Iterate through the layers
			int fin_index = layers.size() - 1;
			// 1. Compute dz for the final layer: dz = A - Y
			vector<float> dz_fin(layers[fin_index].neurons.size());
			for (int i = 0; i < layers[fin_index].neurons.size(); i++) {
				dz_fin[i] = layers[fin_index].neurons[i].value - ((int)image.label == i ? 1.0f : 0.0f);
			};

			// Update gradients for the final layer
			for (int x = 0; x < dz_fin.size(); x++) {
				layers[fin_index].neurons[x].db += dz_fin[x];
				for (int y = 0; y < layers[fin_index].neurons[x].data.size(); y++) {
					layers[fin_index].neurons[x].data[y].dw += dz_fin[x] * layers[fin_index - 1].neurons[y].value;
				};
			};

			// 2. Backpropagate through the hidden layers
			vector<float> dz_previous = dz_fin;
			vector<float> dz;
			for (int l = fin_index - 1; l > 0; l--) {
				dz.resize(layers[l].neurons.size(), 0.0f);

				// Compute dz for current layer not including input
				for (int x = 0; x < layers[l].neurons.size(); x++) {
					float tmp = 0.0f;
					for (int y = 0; y < layers[l + 1].neurons.size(); y++) {
						tmp += layers[l + 1].neurons[y].data[x].w * dz_previous[y];
					};

					// Apply derivative of activation function
					if (layers[l].activation == RElU_const) {
						dz[x] = tmp * heaviside(layers[l].neurons[x].z);
					} else if (layers[l].activation == sigmoid_const) {
						dz[x] = tmp * sigmoid_deriv(layers[l].neurons[x].z);
					};
				};

				// Update gradients for weights and biases
				for (int n = 0; n < layers[l].neurons.size(); n++) {
					layers[l].neurons[n].db += dz[n];

					for (int w = 0; w < layers[l].neurons[n].data.size(); w++) {
						layers[l].neurons[n].data[w].dw += dz[n] * layers[l - 1].neurons[w].value;
					};
				};

				// Move to the previous layer
				dz_previous = dz;
			};
		};

		// adjust the weights and biases based on gradients and learning rate
		void paramAdjust() {
			// Adjust the weights and biases of all layers apart from input / zeroth
			// For each layer
			for (int l = 1; l < layers.size(); l++) {
				// For each neuron
				for (int n = 0; n < layers[l].neurons.size(); n++) {
					layers[l].neurons[n].setBias(layers[l].neurons[n].b - layers[l].alpha * layers[l].neurons[n].db);
					// For each weight
					for (int w = 0; w < layers[l].neurons[n].data.size(); w++) {
						layers[l].neurons[n].setWeight(w, layers[l].neurons[n].data[w].w - layers[l].alpha * layers[l].neurons[n].data[w].dw);
					};
				};
			};
		};

		// run foward and back prop on an image
		void train(Image& image) {
			forwardPropagation(image);
			backwardPropagation(image);
		};

		// Train network on images via batch
		void trainVector(vector<Image>& images, int batch_size, int epoch) {
			cout << "Training on " << images.size() << " images in batches of " << batch_size << endl;

			// repeat testing
			for (int x = 0; x < epoch; x++) {
				cout << "Epoch: " << x + 1 << "/" << epoch << endl;
				// Divide dataset into mini-batches
				int num_batches = images.size() / batch_size;
				for (int batch = 0; batch < num_batches; batch++) {
					// Create mini-batch
					vector<Image> mini_batch(images.begin() + batch * batch_size, images.begin() + (batch + 1) * batch_size);

					//reset the gradients in the new batch
					resetGradients();

					// Process each image in the batch
					for (auto& image : mini_batch) {
						train(image);
					}

					// Average gradients over the batch
					averageGradients(mini_batch.size());

					// Update parameters with averaged deltas
					paramAdjust();
				}
			}

		};

		// Test image through the current neural network
		int testImage(Image& image) {
			forwardPropagation(image);
			return getOutputResults();
		};

		// Test network on array of images and output accuracy
		void testVector(vector<Image>& images) {
			int counter = 0;
			int correct = 0;
			for (auto& image : images) {
				counter++;
				if (image.label == testImage(image)) {
					correct++;
				};
			};
			accuracy = (float)correct * 100 / (float)counter;
			cout << "Accuracy: " << accuracy << "%" << endl;
		};

		// Name network based on weights and biases and accuacy
		string nameNetwork() {
			string retString = "";
			for (int l = 0; l < layers.size(); l++) {
				retString += to_string(layers[l].neurons.size());
				if (l += layers.size() - 1) {
					retString += "-";
				}

			};
			if (accuracy) {
				retString += "_";
				retString += to_string((int)accuracy);
				retString += "pcnt";
			};
			return retString;
		};

		// Save the network to tensorflow compatable JSON
		void toJson() {
			// Create a JSON object
			json jsonObject;

			jsonObject["name"] = "C++ Neural Network";
			jsonObject["accuracy"] = accuracy;
			// for each layer
			jsonObject["layers"] = {};
			for (int l = 0; l < layers.size(); l++) {
				jsonObject["layers"][l] = {
					{ "activation", layers[l].activation },
					{"neurons", {}}
				};
				// for each neuron
				for (int n = 0; n < layers[l].neurons.size(); n++) {
					jsonObject["layers"][l]["neurons"][n] = {
						{ "bias", layers[l].neurons[n].b },
						{ "weights",{}}
					};
					vector<float> weights(layers[l].neurons[n].data.size());
					// for each weight
					for (int w = 0; w < layers[l].neurons[n].data.size(); w++) {
						weights[w] = layers[l].neurons[n].data[w].w;
					};
					jsonObject["layers"][l]["neurons"][n]["weights"] = weights;
				}
			};
			// Write JSON to a file
			ofstream outFile("./Saved Networks/model_" + nameNetwork() + getCurrentTimestamp() + ".json");
			if (outFile.is_open()) {
				outFile << jsonObject.dump(4); // Pretty-print with 4 spaces
				outFile.close();
			} else {
				cerr << "Unable to open file for writing!" << std::endl;
			};
		};
};