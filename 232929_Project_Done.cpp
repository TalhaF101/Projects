#include "Header.h" // linking the Header file
#include <iostream> //This line includes the input/output stream library 
#include <fstream> // This library allows us to perform input and output operations on files
#include <string> // Used for various functions for string manipulation
#include <sstream> // This allows us to perform input and output operations on strings as if they are streams


int main()
{
	string Protocols = "D:/Protocol.txt"; // Path of Protocol File 
	string rules = "D:/Rules.txt"; // Path of Rules File
	string resultFileName = "D:/result.txt"; // Path of Result File

	ofstream resultFile(resultFileName); // Opening the Result File for Writing 

	if (!resultFile.is_open()) // Check if the File is open 
	{
		cout << "Error opening the Result File: " << resultFileName << endl;
		return 1;
	}

	string data = readingALL(Protocols); // Reading all the Data into a String 

	extracting_Packets(data, rules, resultFile); // Take the Entire Data then compare it with the rules and write it in the file

	resultFile.close();// Close the result file

	return 0;
}