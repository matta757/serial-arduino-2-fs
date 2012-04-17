#include <gtest/gtest.h>

#include "../../../src/comm/serialprotocol.h"
#include "../../../src/comm/impl/siocbridge.h"
#include <boost/foreach.hpp>
#include <stdio.h>

const std::string sendToLan( unsigned char id, short value ) {

	char data[6];

	comm::Command c;
	c.id = id;
	c.value = value;
	comm::SerialProtocol *serial = new comm::SerialProtocol();
	serial->copyTo( &c, data );
	delete serial;

	comm::sioc::SiocBridge *bridge = new comm::sioc::SiocBridge();

	std::string stream;
	BOOST_FOREACH( char c, std::string( data, 6) ) {
		bridge->toLan( c, &stream );
	}
	delete bridge;

	//printf( "[test] lan stream: %s\n", result.c_str() );
	return stream;
}

// SIOC  send
TEST( SiocBridge, test_init ) {
	std::string stream = sendToLan( comm::sioc::control::init, 0 );
	ASSERT_TRUE( stream.compare("Arn.Inicio:\r\n") == 0 );
}

TEST( SiocBridge, test_resp_1 ) {
	std::string stream = sendToLan( 35, 5 );
	ASSERT_TRUE( stream.compare("Arn.Resp:35=5:\r\n") == 0 );
}

TEST( SiocBridge, test_resp_2 ) {
	std::string stream = sendToLan( 3, -1250 );
	ASSERT_TRUE( stream.compare("Arn.Resp:3=-1250:\r\n") == 0 );
}

TEST( SiocBridge, test_resp_3 ) {
	std::string stream = sendToLan( 1, 1 );
	ASSERT_FALSE( stream.compare("Arn.Resp:1=1:") == 0 );
}

TEST( SiocBridge, test_protocol) {

	comm::SerialProtocol *serial = new comm::SerialProtocol();
	comm::sioc::SiocBridge *bridge = new comm::sioc::SiocBridge();

	char data[6];
	comm::Command c;
	std::string stream;

	//reserve cmd 1
	c.id = comm::sioc::control::add;

	c.value = 1;
	serial->copyTo( &c, data );
	BOOST_FOREACH( char c, std::string( data, 6) ) {
		ASSERT_FALSE( bridge->toLan( c, &stream ) );
	}

	//reserve cmd 239
	c.value = 239;
	serial->copyTo( &c, data );
	BOOST_FOREACH( char c, std::string( data, 6) ) {
		ASSERT_FALSE( bridge->toLan( c, &stream ) );
	}

	//init
	c.id = comm::sioc::control::init;
	serial->copyTo( &c, data );
	BOOST_FOREACH( char c, std::string( data, 6) ) {
		bridge->toLan( c, &stream );
	}
	ASSERT_TRUE( stream.compare("Arn.Inicio:1:239:\r\n") == 0 );

	//reserve cmd 239 again
	c.id = comm::sioc::control::add;
	c.value = 239;
	serial->copyTo( &c, data );
	BOOST_FOREACH( char c, std::string( data, 6) ) {
		ASSERT_FALSE( bridge->toLan( c, &stream ) );
	}

	//init
	/*
	stream.clear();
	c.id = comm::sioc::control::init;
	serial->copyTo( &c, data );
	BOOST_FOREACH( char c, std::string( data, 6) ) {
		bridge->toLan( c, &stream );
	}
	ASSERT_TRUE( stream.compare("Arn.Inicio:1:239:\r\n") == 0 );
	 */
	delete serial;
	delete bridge;

}

// SIOC receive

TEST( SiocBridge, test_serial_resp) {

	comm::sioc::SiocBridge *bridge = new comm::sioc::SiocBridge();

	std::string stream = "";
	ASSERT_TRUE( bridge->toSerial("Arn.Resp:0=0:\r\n", &stream ) );

	comm::SerialProtocol *serial = new comm::SerialProtocol();

	BOOST_FOREACH( char c, stream ) {
		serial->parse( c );
	}

	ASSERT_TRUE( serial->cmdAvailable() );
	comm::Command cmd = serial->getCmd();

	delete serial;
	delete bridge;

	ASSERT_TRUE( cmd.id == 0 && cmd.value ==0 );
}

