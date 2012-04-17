#pragma once

static const char BYTE_INIT = 0xD5;
static const char BYTE_FIN = 0x58;
static const short BUF_SIZE = 4;

namespace comm {

typedef struct
{
	unsigned char id;
	short value;
} Command;

class SerialProtocol
{
public:
	SerialProtocol( void );
#ifdef __AVR__
	virtual ~SerialProtocol( void );
#endif

	Command getCmd( void );

	bool cmdAvailable( void );
	bool parse( char data );
	void copyTo( comm::Command *cmd, char* buf );

protected:
	bool proccessBuffer( void );

private:
	short _index;
	char _buffer[BUF_SIZE];
	bool _parsing;
	bool _cmdAvailable;
	Command _cmd;
};

}
