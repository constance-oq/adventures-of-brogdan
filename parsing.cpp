#include "flags.h"
#include "parsing.h"

#include <cstring>
#include <sstream>

void parsing::parse_text(
		std::string unparsed_text,
		std::string* dialogue,
		std::string choice_destinations[10],
		std::string choice_text[10]
) {
	parsing::parse_dialogue( unparsed_text, dialogue );
	parsing::parse_choices( unparsed_text, choice_destinations, choice_text );
	parsing::parse_flags( unparsed_text );
}

std::string parsing::substr_by_delimiters( std::string unparsed_text, std::string start, std::string end ) {
	int pos1 = 0;
	int pos2 = 0;
	int eof_pos = unparsed_text.length();

	int result_find = 0;
	result_find = unparsed_text.find( start );
	if ( result_find == std::string::npos ) { return ""; }
	pos1 = result_find + start.length();
	result_find = unparsed_text.find( end, pos1 );
	if ( result_find == std::string::npos ) {
		pos2 = eof_pos;
	} else {
		pos2 = result_find;
	}

	return unparsed_text.substr( pos1, pos2 - pos1 );
}

int parsing::find_or_else( std::string text, std::string target, int start, const char* exception_message ) {
	int result = text.find( target, start );
	if ( result == std::string::npos ) {
		throw parsing::ParsingException( exception_message );
	}
	return result;
}

void parsing::parse_dialogue( std::string unparsed_text, std::string* dialogue ) {
	*dialogue = parsing::substr_by_delimiters( unparsed_text, "***DIALOGUE\n", "\n***" );
	if ( *dialogue == "" ) {
		throw parsing::ParsingException( "No dialogue found." );
	}
}

void parsing::parse_choices(
	std::string unparsed_text,
	std::string choice_destinations[10],
	std::string choice_text[10]
) {
	//Find choice destinations and choice text (as in "1.You go west. 2.You go north. 3.You cast a spell.")
	for ( int i=0; i<10; i++ ) {
		choice_destinations[i] = "";
		choice_text[i] = "";
	}
	
	std::string unparsed_choices = parsing::substr_by_delimiters( unparsed_text, "***CHOICES\n", "\n***" );
	
	int pos1 = 0;
	int pos2 = 0;
	int eof_pos = unparsed_choices.length();
	int choice_i = 0;

	std::istringstream unparsed_choices_stream( unparsed_choices );
	std::string line;

	while ( std::getline( unparsed_choices_stream, line ) ) {
		int result_find = line.find( ":" );
		if ( result_find == std::string::npos and choice_i == 0) {
			throw parsing::ParsingException( "Choices section not populated." );
			return;
		} else if ( result_find == std::string::npos and choice_i > 0 ) {
			return;	// No more choices found.
		}
		pos2 = result_find;
		std::string first_chunk = line.substr ( pos1, pos2 - pos1 );
		bool condition_failed = false;
		if ( first_chunk == "IF" ) {
			bool choice_condition_met = parsing::parse_choice_condition( 
				line, 
				choice_i, 
				choice_destinations, 
				choice_text 
			);
			// parse_choice_condition() will call parse_single_choice() if the flag's condition is met.
			if ( choice_condition_met ) {
				choice_i++;
			}
		} else {
			parsing::parse_single_choice( line, choice_i, choice_destinations, choice_text );
			choice_i++;
		}
	}
}

void parsing::parse_single_choice(
	std::string line,
	int choice_i,
	std::string choice_destinations[10],
	std::string choice_text[10]
) {
	int pos1 = 0;
	int pos2 = 0;
	int eof_pos = line.length();
	int result_find = 0;
	pos2 = line.find( ":", pos1 );
	// It has already been assured that there is a semicolon present.
	choice_destinations[ choice_i ] = line.substr( pos1, pos2 - pos1 );

	pos1 = pos2 + strlen( ":" );
	pos2 = eof_pos;
	choice_text[ choice_i ] = line.substr( pos1, pos2 - pos1 );
}

bool parsing::parse_choice_condition(
	std::string line,
	int choice_i,
	std::string choice_destinations[10],
	std::string choice_text[10]
) {
	int pos1 = 0;
	int pos2 = 0;
	int eof_pos = line.length();

	// Get past the "IF:" at the beginning.
	pos2 = find_or_else( line, ":", pos1, "Syntax error in flags section." );

	pos1 = pos2 + strlen( ":" );
	pos2 = find_or_else( line, ":", pos1, "Syntax error in flags section." );
	std::string flag_name = line.substr( pos1, pos2 - pos1 );

	pos1 = pos2 + strlen( ":" );
	pos2 = find_or_else( line, ":", pos1, "Syntax error in flags section." );
	int flag_compare = std::stoi( line.substr( pos1, pos2 - pos1 ) );

	pos1 = pos2 + strlen( ":" );
	pos2 = eof_pos;
	line = line.substr( pos1, pos2 - pos1 );

	int flag_value = flags::get( flag_name );
	if ( flag_value == flag_compare ) {
		parse_single_choice( line, choice_i, choice_destinations, choice_text );
		return true;
	} else {
		return false;
	}
}

void parsing::parse_flags( std::string unparsed_text ) {
	std::string unparsed_flags = parsing::substr_by_delimiters( unparsed_text, "***FLAGS\n", "\n***" );
	if ( unparsed_flags == "" ) { return; }

	int pos1 = 0;
	int pos2 = 0;
	int eof_pos = unparsed_text.length();

	while ( true ) {
		std::string flag_name;
		int flag_value;
		int result_find;
	
		result_find = unparsed_flags.find( ":", pos1 );
		if ( result_find == std::string::npos ) { break; }	// No more flags
		pos2 = result_find;
		flag_name = unparsed_flags.substr( pos1, pos2 - pos1 );

		pos1 = pos2 + strlen( ":" );
		result_find = unparsed_flags.find( "\n" );
		if ( result_find == std::string::npos ) {
			pos2 = eof_pos;
		}
		flag_value = std::stoi( unparsed_flags.substr( pos1, pos2 - pos1 ) );

		flags::set( flag_name, flag_value );

		// Set up for next iteration
		pos1 = pos2 + strlen( "\n" );
		if ( pos1 >= eof_pos ) { return; }
	}
}
