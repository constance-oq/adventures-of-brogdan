/* brogdan.py

A text adventure game by Zoe

dialogue
blahblah
blah
***
	11:option a
	12:option b
	35:option c

*/

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

std::string universal_error_text = "If you see this text, ping Zoe and tell her \"You are a very shitty wizard\"."; // Not you, dipshit. You're reading the source code.
std::string dialogue = "";
std::string choice_destinations[10] = {};
std::string choice_text[10] = {};

/*
class parsing_error: public std::exception {
	public:
		char* what() const throw() {
			return "Parsing error.";
		}
};

class file_open_error: public std::exception {
	public:
		char* what() const throw() {
			return "Unable to open file".
*/

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

void parse_text( std::string unparsed_text ) {
	// Parse text for dialogue and options.
	int pos1 = 0;
	int pos2 = 0;
	int eof_pos = unparsed_text.length();

	//Find dialogue
	dialogue = "";
	int result_find = 0;
	result_find = unparsed_text.find( "***DIALOGUE\n" );
	if ( result_find != std::string::npos ) {
		pos1 = result_find + strlen ( "***DIALOGUE\n" );

		result_find = unparsed_text.find( "\n***", pos1 );
		pos2 = result_find;
		dialogue = unparsed_text.substr( pos1, pos2 - pos1 );
	}

	//Find choice destinations and choice text (e.g. "1.You go west. 2.You go north. 3.You cast a spell.")

	for ( int i=0; i<10; i++) {
		choice_destinations[i] = "";
	}
	
	result_find = unparsed_text.find( "***CHOICES\n" );
	if ( result_find == std::string::npos ) {
		std::cout << "NO CHOICES FOUND. " + universal_error_text + "\n";
	}
	if ( result_find != std::string::npos ) {
		pos1 = result_find + strlen( "***CHOICES\n" );
		
		int choice_i = 0;
		while ( true ) {
			// Find choice destination
			result_find = unparsed_text.find( ":", pos1 );
			if ( result_find == std::string::npos ) {
				// If first go round, parsing error: can't find any choices.
				// Else, choices must have ended.
				break; 
			}
			pos2 = result_find;
			/* Heed my warning!
				Some moons from now, but not too many, there will be a great parsing error:
				In which some unsuspecting folk or otherwise malicious evildoer shall place a
				colon past the end of the CHOICES delimiter, whereafter a cataclysm
				of speech shall occur. Ye will be offered the choice between "1. Cast a spell" or
				"2. <chunk of ascii art>".
			*/
			choice_destinations[ choice_i ] = unparsed_text.substr( pos1, pos2 - pos1 );
			
			// Find choice text
			pos1 = pos2 + strlen( ":" );
			result_find = unparsed_text.find( "\n", pos1 );
			if ( result_find == std::string::npos ) {
				// Assume that this is the last line of the file.
				pos2 = eof_pos;
			} else {
				pos2 = result_find;
			}
			choice_text[ choice_i ] = unparsed_text.substr( pos1, pos2 - pos1 );
			pos1 = pos2 + strlen( "\n" );
			if ( pos1 >= eof_pos ) { break; } // This is a success.
			choice_i++;
		}
	}
}

void load_page( std::string file ) {
	std::string unparsed_text = read_file( file );		
	parse_text( unparsed_text );
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

	std::cout << "Welcome to the demo of the Adventures of Brogdan, by Zoe. Type numbers and press enter to make choices.\n\n\n";

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

//WHEREAMI:
/* Compile this shit in an easy-to-open format. No terminal necessary.
*/


