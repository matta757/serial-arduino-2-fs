#include <gtest/gtest.h>

#include "../../../src/comm/serialprotocol.h"
#include "../../../src/comm/impl/siocbridge.h"
#include <stdio.h>

bool testCommand( comm::SerialProtocol *serial, unsigned char id, short value ) {

	int i;
	char data[6];
	bool result = false;

	comm::Command c;
	c.id = id;
	c.value = value;
	serial->copyTo( &c, data );

	for (i = 0; i < 6; i++) {
		serial->parse( data[i] );
	}

	if (serial->cmdAvailable()) {
		comm::Command c2 = serial->getCmd();
		result = c2.id == id && c2.value == value;
	} else {
		printf( "[SerialProtocolTest] error, cmd not available.\n" );
	}

	return result;

}

TEST( Types, check_size ) {
	ASSERT_TRUE( sizeof(short) == 2 );
}

TEST( SerialProtocol, test_value ) {
	comm::SerialProtocol *c = new comm::SerialProtocol();

	bool r;
	for (int id = 0; id < 255; ++id) {
		for (int v = -32767; v <= 32767; ++v) {
			r = testCommand( c, id, v );
			if (!r) {
				printf( "[SerialProtocolTest] error, id=%d, value=%d\n", id, v );
			}
			ASSERT_TRUE( r );
		}
	}

	delete c;
}

TEST( SerialProtocol, test_drop) {

	comm::SerialProtocol *serial = new comm::SerialProtocol();

	char data[6];

	comm::Command c;
	c.id = 35;
	c.value = 5;
	serial->copyTo( &c, data );

	serial->parse( 0x01 );
	serial->parse( 0x25 );
	serial->parse( 0x82 );
	for (int i = 0; i < 6; i++) {
		serial->parse( data[i] );
	}
	serial->parse( 0x23 );
	serial->parse( 0x54 );

	ASSERT_TRUE(serial->cmdAvailable());
	comm::Command cmd = serial->getCmd();
	ASSERT_TRUE( cmd.id == 35 && cmd.value == 5 );

	delete serial;
}

