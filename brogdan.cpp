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

#include "flags.cpp"

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

void parse_flags( std::string unparsed_text ) {
	int eof_pos = unparsed_text.length();
	int pos1 = 0;
	int pos2 = 0;

	int result_find = 0;
	result_find = unparsed_text.find( "***FLAGS\n" );
	if ( result_find == std::string::npos ) {
		return;
	}
	pos1 = result_find + strlen( "***FLAGS\n" );
	
	result_find = unparsed_text.find( "\n***" );
	if ( result_find == std::string::npos ) {
		pos2 = eof_pos;
	} else {
		pos2 = result_find;
	}

	std::string unparsed_flags = unparsed_text.substr( pos1, pos2 - pos1 );
	pos1 = 0;

	while ( true ) {
		std::string flag_name;
		int flag_value;
		result_find = unparsed_flags.find( ":", pos1 );
		if ( result_find == std::string::npos ) {
			break;	// No more flags :)
		}
		pos2 = result_find;
		flag_name = unparsed_flags.substr( pos1, pos2 - pos1 );

		pos1 = pos2 + strlen( ":" );
		result_find = unparsed_flags.find( "\n" );
		if ( result_find == std::string::npos ) {
			pos2 = eof_pos;
		}
		std::string result_substr = unparsed_flags.substr( pos1, pos2 - pos1 );
		flag_value = std::stoi( result_substr );

		flags::set( flag_name, flag_value );

		pos1 = pos2 + strlen( "\n" );
		if ( pos1 >= eof_pos ) { break; }
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
		if ( result_find == std::string::npos ) {
			pos2 = eof_pos;
		} else {
			pos2 = result_find;
		}
		dialogue = unparsed_text.substr( pos1, pos2 - pos1 );
	}

	//Find choice destinations and choice text (e.g. "1.You go west. 2.You go north. 3.You cast a spell.")

	for ( int i=0; i<10; i++) {
		choice_destinations[i] = "";
	}
	
	result_find = unparsed_text.find( "***CHOICES\n" );
	if ( result_find == std::string::npos ) {
		std::cout << "NO CHOICES FOUND. " + universal_error_text + "\n";
	} else {
		pos1 = result_find + strlen( "***CHOICES\n" );

		result_find = unparsed_text.find( "\n***", pos1 );
		if ( result_find == std::string::npos ) {
			pos2 = eof_pos;
		} else {
			pos2 = result_find;
		}
		std::string unparsed_choices = unparsed_text.substr( pos1, pos2 - pos1 );
		// pos1 and pos2 are now positions in unparsed_choices, NOT unparsed_text
		pos1 = 0;
		pos2 = 0;

		int choice_i = 0;
		while ( true ) {
			// Find choice destination
			result_find = unparsed_choices.find( ":", pos1 );
			if ( result_find == std::string::npos ) {
				// If first go round, parsing error: Choices section is not populated.
				// Else, choices must have ended.
				break; 
			}
			pos2 = result_find;
			std::string result_substr = unparsed_choices.substr( pos1, pos2 - pos1 );

			bool failed_conditional_flag = false;
			if ( result_substr == "IF" ) {
				// Conditional behaviour based on flags! :)
				pos1 = pos2 + strlen( ":" );
				// The following code is not safe for parsing errors. Refactor required.
				// Retrieving result_find and detecting parse errors should be a function.
				pos2 = unparsed_choices.find( ":", pos1 );
				result_substr = unparsed_choices.substr( pos1, pos2 - pos1 ); 
				int current_flag_value = flags::get( result_substr );

				pos1 = pos2 + strlen( ":" );
				pos2 = unparsed_choices.find( ":", pos1 );
				result_substr = unparsed_choices.substr( pos1, pos2 - pos1 );
				int compare_flag_value = std::stoi( result_substr );
				
				if ( current_flag_value != compare_flag_value ) {
					failed_conditional_flag = true;
				}

				// Set up to read choice destination
				pos1 = pos2 + strlen( ":" );
				pos2 = unparsed_choices.find( ":", pos1 );
				
			}

			if ( !failed_conditional_flag ) { // <-- Refactor required.
				choice_destinations[ choice_i ] = result_substr;
			}	

			// Find choice text
			pos1 = pos2 + strlen( ":" );
			result_find = unparsed_choices.find( "\n", pos1 );
			if ( result_find == std::string::npos ) {
				// Assume that this is the last line of the file.
				pos2 = eof_pos;
			} else {
				pos2 = result_find;
			}
			if ( !failed_conditional_flag ) { // <-- Refactor required.
				choice_text[ choice_i ] = unparsed_choices.substr( pos1, pos2 - pos1 );
			}
			pos1 = pos2 + strlen( "\n" );
			if ( pos1 >= eof_pos ) { break; } // This is a success.
			if ( !failed_conditional_flag ) { // <-- Refactor required.
				choice_i++;
			}
		}
	}
	parse_flags( unparsed_text );
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

