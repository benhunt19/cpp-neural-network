// cpp-neural-network.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <iostream>
#include <fstream>
#include "header.h"
#include "globalClasses.h"

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
	int end_index = 5000;

	vector<Image> training_images = trainingData.getImageVector(start_index, end_index);
	vector<char> training_labels = labelData.getLabelVector(start_index, end_index);

	// pair images and labels;
	for (int i = start_index; i < end_index; i++) {
		training_images[i].setLabel(training_labels[i]);
	}
	
	for (int i = 0; i < 10; i++) {
		cout << int(training_images[i].label) << endl;
		training_images[i].print();
	}


	return 1;
}