// cpp-neural-network.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <iostream>
#include <fstream>
#include "globalFunctions.h"
#include "fileClasses.h"
#include "networkClasses.h"

using namespace std;

/*GLOBALS*/
// Images
const string trainImagesPath = "C:/Users/benhu/PycharmProjects/Data Sets/mnist-dataset/versions/1/train-images-idx3-ubyte";
const string trainImagesFile = "/train-images-idx3-ubyte";
const string testImagesPath = "C:/Users/benhu/PycharmProjects/Data Sets/mnist-dataset/versions/1/t10k-images-idx3-ubyte";
const string testImagesFile = "/t10k-images-idx3-ubyte";
// Labels
const string trainLabelsPath = "C:/Users/benhu/PycharmProjects/Data Sets/mnist-dataset/versions/1/train-labels-idx1-ubyte";
const string trainLabelsFile = "/train-labels-idx1-ubyte";
const string testLabelsPath = "C:/Users/benhu/PycharmProjects/Data Sets/mnist-dataset/versions/1/t10k-labels-idx1-ubyte";
const string testLabelsFile = "/t10k-labels-idx1-ubyte";

int main()
{
	File trainingImageData = File(trainImagesPath, trainImagesFile);
	File trainingLabelData = File(trainLabelsPath, trainLabelsFile);

	int start_index = 0;
	int end_index = 60000;
	int batch_size = 16;

	vector<Image> training_images = trainingImageData.getImageVector(start_index, end_index);
	vector<char> training_labels = trainingLabelData.getLabelVector(start_index, end_index);
	int final_layer_count = 10; // labels are 0, 1, 2, ... , 9 

	// zip together the images and their corresponding labels
	for (int i = start_index; i < end_index; i++) {
		training_images[i].setLabel(training_labels[i]);
	}
	
	// create the neural network
	NeuralNetwork nn = NeuralNetwork(
		{
			trainingImageData.image_size,  // input (layer zero)
			10,							   // layer one
			final_layer_count              // final layer (layer two)
		},    
		RElU_const                         // activation to use
	);
	
	// train the neural network
	nn.trainVector(
		training_images,
		batch_size
	);

	File testImageData = File(testImagesPath, testImagesFile);
	File testLabelData = File(testLabelsPath, testLabelsFile);

	int start_index_test = 0;
	int end_index_test = 256;

	vector<Image> test_images = testImageData.getImageVector(start_index_test, end_index_test);
	vector<char> test_labels = testLabelData.getLabelVector(start_index_test, end_index_test);

	// zip together the images and their corresponding labels
	for (int i = start_index_test; i < end_index_test; i++) {
		test_images[i].setLabel(test_labels[i]);
	}

	cout << "~~~~~~~~~~ TESTING ~~~~~~~~~~" << endl;

	nn.testVector(test_images);

	return 1;
}