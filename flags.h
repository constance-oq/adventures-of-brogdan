#ifndef FLAGS_H
#define FLAGS_H

#include <map>

namespace flags
{
	std::map< std::string, int > flags_map;
	void set( std::string flag_name, int flag_value );
	int get( std::string flag_name );
}

#endif
