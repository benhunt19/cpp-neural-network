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
// Labels
const string trainLabelsPath = "C:/Users/benhu/PycharmProjects/Data Sets/mnist-dataset/versions/1/train-labels-idx1-ubyte";
const string trainLabelsFile = "/train-labels-idx1-ubyte";

int main()
{
	File trainingData = File(trainImagesPath, trainImagesFile);
	File labelData = File(trainLabelsPath, trainLabelsFile);

	int start_index = 0;
	int end_index = 20;

	vector<Image> training_images = trainingData.getImageVector(start_index, end_index);
	vector<char> training_labels = labelData.getLabelVector(start_index, end_index);
	int final_layer_count = 10; // labels are 0, 1, 2, ... , 9 

	// zip together the images and their corresponding labels
	for (int i = start_index; i < end_index; i++) {
		training_images[i].setLabel(training_labels[i]);
	}
	
	// create the neural network
	NeuralNetwork nn = NeuralNetwork({ 
		trainingData.image_size,       // layer zero
		10,							   // layer one
		final_layer_count              // layer two (final)
	});
	
	nn.trainVector(training_images);
	//nn.printLayerVals(1);
	nn.printLayerVals(2);

	return 1;
}