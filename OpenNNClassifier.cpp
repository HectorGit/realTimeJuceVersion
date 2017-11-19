#pragma once
//#include "stdafx.h"

#include "OpenNNClassifier.h"

// System includes
//#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <time.h>
#include <algorithm>
#include <stdint.h>
#include <limits.h>
#include <stdexcept>

// OpenNN includes

//#include "opennn/opennn.h"
//#include "open"
//#include <opennn/multilayer_perceptron.h>
//#include <opennn/perceptron_layer.h>
//#include "training_strategy.h"
////#include "tests/*.h"
//#include "variables.h"
////#include "statistics.h"
using namespace OpenNN;
using namespace std;

//IN THE CONSTRUCTOR LOAD THE NETWORK.
OpenNNClassifier::OpenNNClassifier() {
	try
	{

		debug = true;
		tinyxml2::XMLDocument myDoc;
		myDoc.LoadFile("data/proceeds/neural_network.xml"); //FIX !
		if (debug) { cout << " Found xml file" << endl; }
		if (debug) { cout << " Creating neural network from xml file" << endl; }

		neural_network_Pointer = new NeuralNetwork(myDoc); //does this work?

		if (debug) { cout << " Created neural_network from xml file" << endl; }
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
		//return(1);
	}
}

OpenNNClassifier::~OpenNNClassifier() {
	//delete 
	//i don't know what or how to free it.
}

Vector<double> OpenNNClassifier::obtainOutputs(Vector<double> vector_input) {

	try {
		if (debug) { cout << "------------------------------------------" << endl; }
		if (debug) { cout << "\n (obtainOutputs) Input vector: \n" << vector_input.to_string() << "\n" << endl; }
		if (debug) { cout << " (obtainOutputs) Input size :" << vector_input.size() << endl; }
		if (debug) { cout << " (obtainOutputs) NN Inputs Number:" << neural_network_Pointer->get_inputs_number() << endl; }
		Vector<double> output_vector = neural_network_Pointer->calculate_outputs(vector_input);

		if (debug) {
			cout << "\n (obtainOutputs) Activations/Outputs : " << output_vector.to_string() << "\n\n" << endl;
		}
		return output_vector; //get the maximal index elsewhere.
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
		//return(1);
	}

}

size_t OpenNNClassifier::obtainClassification(Vector<double> output_vector) {
	size_t myClassificationIndex = output_vector.calculate_maximal_index();

	if (debug) {
		cout << "\n Classification Index: " << myClassificationIndex << "\n" << endl;
		cout << "\n Actual Classification: " << myClassificationIndex + 1 << "\n" << endl;
		cout << "\n Activation Value: " << output_vector.at(myClassificationIndex) << "\n" << endl;
		//if (debug) { cout << "------------------------------------------" << endl; }
	}

	return myClassificationIndex;
}

Vector<double> OpenNNClassifier::convert(Marsyas::realvec vector) {
	if (debug) { cout << "\n (CONVERT) Input realvec vector: \n" << vector << "\n" << endl; }
	Vector<double> resultingVector((int)vector.getSize()); //initialize with a size? //getSize is mrs_natural
	for (size_t i = 0; i < vector.getSize(); i++) {
		resultingVector.at(i) = vector((Marsyas::mrs_natural)i); //vector seems to require mrs_natural
	}
	return resultingVector;
}