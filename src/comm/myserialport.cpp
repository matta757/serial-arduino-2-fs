#include "impl/siocbridge.h"
#include <boost/bind.hpp> 
#include <boost/asio.hpp> 
#include <boost/asio/serial_port.hpp> 
#include <boost/thread.hpp> 
#include <boost/lexical_cast.hpp> 
#include <boost/algorithm/string.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/foreach.hpp>
#include <deque>
#include <iostream>
#include <string>
#include <math.h>


using namespace std;
using namespace boost;
using boost::asio::ip::tcp;

#define SERIAL_READ_SIZE 1

class MySerialPort
{
public:
	MySerialPort( boost::asio::io_service& io_service, unsigned int baud, const string& device ) //, Game *game)
	:
			active_( true ),
			io_service_( io_service ),
			serialPort( io_service, device ),
			socket( io_service ) {

		// UDP

		printf( "[Bridge] Connecting to localhost...\n" );
		// Get a list of endpoints corresponding to the server name.
		tcp::resolver resolver( io_service );
		tcp::resolver::query query( "localhost", "8092" );
		tcp::resolver::iterator endpoint_iterator = resolver.resolve( query );

		// Try each endpoint until we successfully establish a connection.
		//	tcp::socket socket(io_service);
		//printf("[Bridge] Connecting to serial...");
		boost::asio::connect( socket, endpoint_iterator );
		//printf("[OK]\n");

		//SERIAL
		if (!serialPort.is_open()) {
			cout << "Failed to open serial port\n";
			return;
		}
		boost::asio::serial_port_base::baud_rate baud_option( baud );
		serialPort.set_option( baud_option ); // set the baud rate after the port has been opened

		_bridge = new comm::sioc::SiocBridge();
		startThreads();
	}

	void startThreads() {
		boost::thread_group threads;
		threads.create_thread( boost::bind( &MySerialPort::read_start, this ) );
		threads.create_thread( boost::bind( &MySerialPort::out_thread, this ) );
	}

	void readLan( std::string *s ) {
		boost::asio::streambuf response;
		size_t n = boost::asio::read_until( socket, response, "\r\n" );
		response.commit( n );
		std::istream is( &response );
		//string t;
		is >> *s;
		//s->append(t);

	}

	void out_thread() {

		printf( "[LAN2Serial] thread started.\n" );

		//init serial
		BOOST_FOREACH( char c, _bridge->initSerial() ) {
			write( c );
			//printf( "[LAN2Serial] thread started.\n" );
		}

		//TODO send( 0,0 );

		std::string serialStream;
		std::string s;

		printf( "[LAN2Serial] serial read, start loop.\n" );
		while (true) {
			//	printf( "[LAN2Serial] serial read, start loop.\n" );
			serialStream.clear();
			s.clear();
			readLan( &s );

			//std::cout << "[READ] " << &response;
			if (_bridge->toSerial( s, &serialStream )) {

				//printf( "[LAN2Serial] ENVIOOOOO. %s\n", serialStream.c_str() ); 
BOOST_FOREACH			( char c, serialStream ) {
				write( c );
			}
		}
	}
		printf( "[LAN2Serial] out thread removed" );
	}

	void write( const char msg ) // pass the write data to the do_write function via the io service in the other thread
	        {
		io_service_.post( boost::bind( &MySerialPort::do_write, this, msg ) );
	}

	void close() // call the do_close function via the io service in the other thread 
	{
		io_service_.post( boost::bind( &MySerialPort::do_close, this, boost::system::error_code() ) );
	}

	bool active() // return true if the socket is still active 
	{
		return active_;
	}

private:
	void read_start( void ) { // Start an asynchronous read and call read_complete when it completes or fails
		serialPort.async_read_some( boost::asio::buffer( _serialData, SERIAL_READ_SIZE ),
		        boost::bind( &MySerialPort::read_complete, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred ) );
	}

	//	on read send data from Serial to LAN
	void read_complete( const boost::system::error_code& error, size_t /*bytes_transferred*/) {
		if (!error) {
			std::string stream;
			if (_bridge->toLan( _serialData[0], &stream )) {
				boost::asio::streambuf request;
				std::ostream request_stream( &request );
				request_stream << stream;
				boost::asio::write( socket, request );
			}
			read_start();
		} else {
			printf( "[comm::serial] Error on read.\n" );
			do_close( error );
		}
	}

	void do_write( const int msg ) { // callback to handle write call from outside this class
		bool write_in_progress = !write_msgs_.empty(); // is there anything currently being written?
		write_msgs_.push_back( msg ); // store in write buffer
		if (!write_in_progress) // if nothing is currently being written, then start
			write_start();
	}

	void write_start( void ) { // Start an asynchronous write and call write_complete when it completes or fails
		boost::asio::async_write( serialPort, boost::asio::buffer( &write_msgs_.front(), 1 ), boost::bind( &MySerialPort::write_complete, this, boost::asio::placeholders::error ) );
	}

	void write_complete( const boost::system::error_code& error ) { // the asynchronous read operation has now completed or failed and returned an error
		if (!error) { // write completed, so send next write data
			write_msgs_.pop_front(); // remove the completed data
			if (!write_msgs_.empty()) // if there is anthing left to be written
				write_start(); // then start sending the next item in the buffer
		} else
			do_close( error );
	}

	void do_close( const boost::system::error_code& error ) { // something has gone wrong, so close the socket & make this object inactive
		if (error == boost::asio::error::operation_aborted) // if this call is the result of a timer cancel()
			return; // ignore it because the connection cancelled the timer
		if (error)
			cout << "Error: " << error.message() << endl; // show the error message
		else
			cout << "Error: Connection did not succeed.\n";
		serialPort.close();
		active_ = false;
	}

private:
	char _serialData[SERIAL_READ_SIZE];		// data read from the socket
	bool active_;							// remains true while this object is still operating
	boost::asio::io_service& io_service_;	// the main IO service that runs this connection
	boost::asio::serial_port serialPort;	// the serial port this instance is connected to
	deque<char> write_msgs_; // buffered write data

	tcp::socket socket;
	comm::LanSerialBridge *_bridge;
};
