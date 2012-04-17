#pragma once

#include "serialprotocol.h"
#include <string>
#include <vector>

namespace comm {

class SerialProtocol;

class LanSerialBridge
{
public:
	LanSerialBridge();
	virtual ~LanSerialBridge();

	virtual const std::string initSerial( void );

	bool toLan( char chunk, std::string *stream );
	bool toSerial( const std::string &in, std::string *out );

	SerialProtocol *getSerial( void );

protected:
	virtual bool getString( comm::Command cmd, std::string *out )=0;
	virtual void getCmd( const std::string &stream, std::vector<comm::Command> *cmds )=0;

private:
	SerialProtocol *_serial;
};

}

