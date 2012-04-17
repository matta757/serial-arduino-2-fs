#include "lanserialbridge.h"
#include <boost/foreach.hpp>

namespace comm {

LanSerialBridge::LanSerialBridge() {
	_serial = new SerialProtocol();
}

LanSerialBridge::~LanSerialBridge() {
	delete _serial;
	_serial = NULL;
}

const std::string LanSerialBridge::initSerial( void ) {
	char data[6];
	comm::Command c;
	c.id = 0xF3;
	c.value = 0;
	_serial->copyTo( &c, data );
	return std::string( data, 6 );
}

bool LanSerialBridge::toLan( char chunk, std::string *out ) {
	_serial->parse( chunk );
	if (_serial->cmdAvailable()) {
		return getString( _serial->getCmd(), out );
	}
	return false;
}

bool LanSerialBridge::toSerial( const std::string &in, std::string *out ) {
	std::vector<comm::Command> cmds;
	getCmd( in, &cmds );

	if (cmds.size()) {
		out->clear();
		BOOST_FOREACH( comm::Command cmd, cmds ) {
			char data[6];
			_serial->copyTo( &cmd, data );
			out->append( data, 6 );
		}
		return true;
	}

	return false;
}

SerialProtocol *LanSerialBridge::getSerial( void ) {
	return _serial;
}

}
