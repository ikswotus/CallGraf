// PlusPlusTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


#include <stdio.h>
#include <time.h>
#include<iostream>

#include <vector>
#include <fstream>

#include <sstream>
#include <random>
#include <climits>
#include <algorithm>
#include <functional>
#include <string>


void WriteFile(std::string s, const std::vector<std::string>& lines)
{
	try

	{
		std::ofstream outFile(s);
		for (std::vector<std::string>::const_iterator it = lines.cbegin(); it != lines.cend(); it++)
		{
			outFile << it->c_str();
		}
		outFile.close();
	}
	catch (const std::exception exc)
	{
		// TODO: Log errors
		std::cout << exc.what();
	}
}

int main()
{
	std::string fname = "E:\\Code\\TestProcess_" + generate_hex(20) + ".bin";

	std::vector<std::string> test(20);
	for (int i = 0; i < 20; i++)
		test.push_back("Line: " + std::to_string(i) + "\n");
	WriteFile(fname, test);

	/*time_t seconds;

	seconds = time(NULL);

	printf("%ld seconds", seconds);
*/
	char x;
	std::cin >> x;


    return 0;
}

