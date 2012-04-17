#include "serialprotocol.h"

#ifdef __AVR__
#include "WProgram.h"
#endif

namespace comm {

SerialProtocol::SerialProtocol( void ) {
	_index = 0;
	_parsing = false;
	_cmdAvailable = false;
}

#ifdef __AVR__
SerialProtocol::~SerialProtocol( void ) {
}
#endif

bool SerialProtocol::proccessBuffer( void ) {
	_cmd.id = _buffer[0];
	_cmd.value = (_buffer[1] & 0xFF) | (_buffer[2] << 8);
	return (_buffer[1] ^ _buffer[2]) == _buffer[3];
}

bool SerialProtocol::cmdAvailable( void ) {
	return _cmdAvailable;
}

bool SerialProtocol::parse( char data ) {

	bool parsed = true;

	if (data == BYTE_INIT && !_parsing) {
		_parsing = true;
	} else if (data == BYTE_FIN && _parsing && _index == BUF_SIZE) {
		_cmdAvailable = proccessBuffer();
		_parsing = false;
		_index = 0;
	} else if (_parsing && _index < BUF_SIZE) {
		_buffer[_index] = data;
		_index++;
	} else {
		parsed = false;
	}

	return parsed;
}

void SerialProtocol::copyTo( comm::Command *cmd, char* buf ) {
	buf[0] = BYTE_INIT;
	buf[1] = cmd->id;
	buf[2] = cmd->value & 0xFF;
	buf[3] = (cmd->value >> 8) & 0xFF;
	buf[4] = (cmd->value & 0xFF) ^ (((cmd->value) >> 8) & 0xFF);
	buf[5] = BYTE_FIN;
}

Command SerialProtocol::getCmd( void ) {
	_cmdAvailable = false;
	return _cmd;
}

}
