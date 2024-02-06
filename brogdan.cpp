/* brogdan.py

A text adventure game by Zoe

*/

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

#include "flags.h"
#include "parsing.h"

std::string universal_error_text = "If you see this text, ping Zoe and tell her \"You are a very shitty wizard\"."; // Not you, dipshit. You're reading the source code.
std::string dialogue = "";
std::string choice_destinations[10] = {};
std::string choice_text[10] = {};

std::string read_file( std::string file_name ) {
	//Read file at file_name to a string
	std::ifstream myfile( file_name );
	if ( myfile.is_open() ) {
		std::string content(
			( std::istreambuf_iterator<char>( myfile ) ),
			( std::istreambuf_iterator<char>() ) 
		);
		return content;
	} else {
		std::cout << "Unable to open file '" + file_name + ".'\n";
		return "";
	}
}

void load_page( std::string file ) {
	std::string unparsed_text = read_file( file );		
	parsing::parse_text(
		unparsed_text,
		&dialogue,
		choice_destinations,
		choice_text
	);
}

void display_page() {
	std::cout << dialogue << "\n";
	int i = 0;
	while ( choice_destinations[i] != "" ) {
		std::cout << i << ". " << choice_text[i] << "\n";
		i++;
	}
}

int query_user() {
	// Query the user for their choice.
	// Try to cast input_text to an integer.
	// Integer must correspond to a non-empty string from choice_destinations[]. 

	std::string input_text;
	int input_num;

	std::cout << ">";
	std::getline( std::cin, input_text );

	if ( input_text == "exit" ) {
		exit( 0 );
	}

	try { input_num = std::stoi( input_text ); } 
	catch ( const std::invalid_argument ) { return -1; }
	catch ( const std::out_of_range ) { return -1; }

	if ( input_num < 0 || input_num > 9 ) { return -1; }
	
	if ( choice_destinations[ input_num ] == "" ) { return -1; }
	
	return input_num;
}

int main() {

	std::cout << "Welcome to the demo of the Adventures of Brogdan, by Zoe. Type numbers and press enter to make choices.";

	int choice = 0;
	choice_destinations[0] = "0";

	while ( true ) {
		load_page( "pages/" + choice_destinations[ choice ] + ".txt" );
		std::cout << "\n\n\n";
		display_page();

		while ( choice = query_user(), choice == -1 ) {
			std::cout << "Invalid choice.\n";
		}
	}

	// legacy code :^)
	// std::cout << "You are a very shitty wizard.\n";

	return 0;
}

