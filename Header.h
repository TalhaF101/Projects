#pragma once
#include <iostream> //This line includes the input/output stream library 
#include <fstream> // This library allows us to perform input and output operations on files
#include <string> // Used for various functions for string manipulation
#include <sstream> // This allows us to perform input and output operations on strings as if they are streams

using namespace std;

struct Info // defining a structure 
{
	string src; // string to store the Source Ip
	string dst; // string to store the Destination Ip
	string protocol; // string to store the Protocol
};                                          
                                                    // End Structure 

bool conversion(const string& part, int& value) //converion of the string ips to integers 
{
	istringstream stream(part); // the conversion 
	stream >> value;// the storing of the integer from the stream

	// Check for conversion errors
	return !stream.fail() && stream.eof(); // Only returns when it doesnt fail and the end is reached 
}
                                                   // End of String to Int

bool range_check(const string& ruleIP, const string& packetIP)
{
	size_t dash = ruleIP.find('-'); // Find the position of '-' in the ruleIP

	if (dash != string::npos)
	{
		string rangeStart = ruleIP.substr(0, dash);
		string rangeEnd = ruleIP.substr(dash + 1);

		size_t dotPos = packetIP.find('.');
		if (dotPos != string::npos)
		{
			string packetStart = packetIP.substr(0, dotPos);
			string packetEnd = packetIP.substr(dotPos + 1);

			int rangeStartInt, rangeEndInt, packetStartInt, packetEndInt;
			if (conversion(rangeStart, rangeStartInt) &&
				conversion(rangeEnd, rangeEndInt) &&
				conversion(packetStart, packetStartInt) &&
				conversion(packetEnd, packetEndInt))
			{

				int packetIPInt = packetStartInt * 256 + packetEndInt;
				return packetIPInt >= rangeStartInt && packetIPInt <= rangeEndInt;
			}
		}
	}
	else // If no '-', compare ruleIP and packetIP directly
	{
		return ruleIP == packetIP;
	}

	// If all conditions fail, return false
	return false;
}
                                                   // End Range Check

string extracting_data(const string& line, const string& Char)
{
	size_t start = line.find(Char); // look for the first Occurance of the Whatever is in Char and stores it in start 

	if (start != string::npos) // if the Char is not found in the string line then this gets equal 
	{
		start += Char.length(); // This is used to move to the next char after finding the first one 

		size_t end = line.find(';', start); // this runs from start and if the (;) is found then stores it in end

		if (end != string::npos) //if end is not equal to npos then string was found 
		{
			return line.substr(start, end - start);  // this extracts the data and returns it that is from the start to the difference in between 
		}
		else
		{
			return line.substr(start);// this is done so that if the packet in spilt on different lines 
		}
	}
	return ""; // an empty string should be returned when start is equal to npos
}
                                                  // End Extracting Data

string readingALL(const string& file) //parameter is a constant reference string (The File)
{
	ifstream Protocol(file);	//reading the data from the Input file 
	if (!Protocol.is_open()) // Checking if the file was found 
	{
		cout << "Error opening the Protocol File: " << file << endl;
		return ""; // The empty string signals an eroor opening the file 
	}

	string data;
	string line;

	while (!Protocol.eof()) // Continue reading lines until the end of the file is reached
	{
		getline(Protocol, line); // Read a line from the file
		data += line + '\n';   // Append the line (along with a newline character) to the data string
	}

	Protocol.close(); // Closes the file 
	return data; // Return the concatenated string containing the entire content of the file
}
                                                   // End readingALL

void compare(const Info& PacketInfo, ifstream& rules, ofstream& resultFile)
{
	string Num, Type, Val, Action;
	bool ruleApplied = false;

	while (rules >> Num >> Type >> Val >> Action)// with in the loop it reads the data from it and store it in these variables repectivily 
	{
		//Check for type and the Ip 

		bool srcMatch = (Type == "SRC" && range_check(Val, PacketInfo.src));
		bool dstMatch = (Type == "DST" && range_check(Val, PacketInfo.dst));
		bool proMatch = (Type == "PRO" && PacketInfo.protocol == Val);

		if (srcMatch || dstMatch || proMatch)
		{
			
			bool packetMatch = true;

			if (packetMatch)
			{
				//  write data to the file 
				resultFile << "SRC " << PacketInfo.src << " DST " << PacketInfo.dst
					<< " PRO " << PacketInfo.protocol << " " << Action << " " << Num << endl;

				ruleApplied = true;
				break;

			}
		}
	}

}
                                                  // End of Compare 

void extracting_Packets(const string& line, const string& ruleFile, ofstream& resultFile)
{
	size_t PacketStart = line.find('[');// Find the position of the opening bracket '[' in the line

	while (PacketStart != string::npos)// Check if '[' is found
	{
		size_t Packetend = line.find(']', PacketStart + 1); // Find the position of the closing bracket ']' starting from the position after '['	

		if (Packetend != string::npos && Packetend > PacketStart) // Check if ']' is found and it is after '['
		{
			string PacketData = line.substr(PacketStart + 1, Packetend - PacketStart - 1);// Extract the substring between '[' and ']' (exclusive)
			// Store the extracted PacketData to source, destination, and protocol 
			Info data;

			data.src = extracting_data(PacketData, "SRC:");
			data.dst = extracting_data(PacketData, "DST:");
			data.protocol = extracting_data(PacketData, "PRO:");

			ifstream rules(ruleFile);	// Open the rules file and compare the extracted packet data with the rules
			if (!rules.is_open()) 
			{
				cout << "Error opening the Rules File: " << ruleFile << endl;
				return;
			}

			compare(data, rules, resultFile);// Call the compare function to check and apply rules

			rules.close();// Close the rules file

			PacketStart = line.find('[', Packetend + 1);
		}
		else 
		{
			break;
		}
	}
}
                                                  // End of Extracting Packets


                                          /*<<<<<<<<<<<<<<<<END>>>>>>>>>>>>>>>>>>>>>>*/