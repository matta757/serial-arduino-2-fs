#include "siocbridge.h"
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <stdio.h>
#include <sstream>
#include <vector>

namespace comm {
namespace sioc {

SiocBridge::SiocBridge() {
	_init = false;
}

SiocBridge::~SiocBridge() {
}

bool SiocBridge::getString( comm::Command cmd, std::string *out ) {

	std::stringstream stream;
	bool result = true;

	if (cmd.id < control::reset) {
		//printf( "[SiocBridge]  response, id=%d, value=%d\n", cmd.id, cmd.value );
		stream << "Arn.Resp:" << (short) cmd.id << "=" << cmd.value << ":\r\n";
	} else {
		switch ( cmd.id ) {
			case control::reset:
				_cmds.clear();
				break;
			case control::init:
				if (!_init) {
					_init = true;
					stream << "Arn.Inicio:";
					BOOST_FOREACH( short cmd, _cmds ) {
						stream << cmd << ":";
					}
					stream << "\r\n";
					printf( "[SiocBridge] init, value=%s\n", stream.str().c_str() );
				} else {
					result = false;
				}
				break;
			case control::add:
				if (!_init) {
					printf( "[SiocBridge] reserve, value=%d\n", cmd.value );
					_cmds.insert( cmd.value );
				}
				result = false;
				break;
			default:
				break;
		}
	}

	//printf( "[SiocBridge] %s\n", stream.str().c_str() );
	if (result) {
		out->append( stream.str() );
	}
	return result;
}

void SiocBridge::getCmd( const std::string &stream, std::vector<comm::Command> *cmds ) {
	std::string s = stream;

	if (boost::find_first( s, "Arn.Resp:" )) { // SIOC response

//		printf( "[LAN2Serial] %s\n", stream.c_str() );

		boost::trim_left_if( s, boost::is_any_of( "Arn.Resp:" ) );
		// split values
		std::vector<std::string> values;
		boost::split( values, s, boost::is_any_of( ":" ) );

		BOOST_FOREACH ( std::string toSend, values) {
			if (boost::find_first( toSend, "=" )) {
				std::vector<std::string> toSendPair;
				boost::split( toSendPair, toSend, boost::is_any_of( "=" ) );

				comm::Command cmd;
				cmd.id = boost::lexical_cast<short>( toSendPair[0] );
				try {

					if (toSendPair[1] == "-999999") {
						cmd.value = boost::lexical_cast<short>( -1 );
					} else {
						cmd.value = boost::lexical_cast<short>( toSendPair[1] );
					}
					cmds->push_back( cmd );
				} catch (boost::bad_lexical_cast &) {
					printf( "[LAN2Serial] bad_lexical_cast, stream=%s\n", stream.c_str() );
				}
//				printf( "[LAN2Serial] id=%d, value=%d\n", cmd.id, cmd.value );
			}
		}
	}
}

}
}
