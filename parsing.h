#ifndef PARSING_H
#define PARSING_H

#include <string>

namespace parsing {
	void parse_text(
		std::string unparsed_text,
		std::string* dialogue,
		std::string choice_destinations[10],
		std::string choice_text[10]
	);
	std::string substr_by_delimiters( std::string unparsed_text, std::string start, std::string end );
	int find_or_else(
		std::string text,
		std::string target,
		int start,
		const char* exception_message
	);
	void parse_dialogue( std::string unparsed_text, std::string* dialogue);
	void parse_choices(
		std::string unparsed_text,
		std::string choice_destinations[10],
		std::string choice_text[10]
	);
	void parse_single_choice(
		std::string line,
		int choice_i,
		std::string choice_destinations[10],
		std::string choice_text[10]
	);
	bool parse_choice_condition(
		std::string line,
		int choice_i,
		std::string choice_destinations[10],
		std::string choice_text[10]
	);
	void parse_flags( std::string unparsed_text );
	class ParsingException : public std::exception {
		private:
		const char* message;
		
		public:
		ParsingException( const char* msg ) : message( msg ) {}
		const char* what() { return message; }
	};
}

#endif
