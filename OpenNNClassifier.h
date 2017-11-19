#pragma once

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

#include "opennn/vector.h"
#include "opennn/neural_network.h"
#include <marsyas/types.h>
#include "marsyas/realvec.h" //does this work?
//#include <opennn/multilayer_perceptron.h>
//#include <opennn/perceptron_layer.h>
//#include <opennn/evolutionary_algorithm.h>
//#include "training_strategy.h"
////#include "tests/*.h"
//#include "variables.h"
////#include "statistics.h"
//using namespace OpenNN;
using namespace std;

class OpenNNClassifier {

public:
	OpenNNClassifier();
	~OpenNNClassifier();
	OpenNN::Vector<double> obtainOutputs(OpenNN::Vector<double> vector_input);
	size_t obtainClassification(OpenNN::Vector<double> output_vector);
	OpenNN::Vector<double> convert(Marsyas::realvec vector);
	OpenNN::NeuralNetwork* neural_network_Pointer;
	bool debug;
private:
};