#include "flags.h"

void flags::set( std::string flag_name, int flag_value ) {
	flags_map[ flag_name ] = flag_value;
}

int flags::get( std::string flag_name ) {
	std::map< std::string, int >::iterator result_map_find = flags_map.find( flag_name ); 
	if ( result_map_find == flags_map.end() ) {
		return 0; // flag not found
	} else {
		return result_map_find->second;
	}
}
