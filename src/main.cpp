#include "comm/myserialport.cpp"
#include <iostream> 

int main(int /*argc*/, char** /*argv[]*/)
{ 
	try 
	{ 
		boost::asio::io_service io_service; 
		// define an instance of the main class of this program 
		MySerialPort _serialPort(io_service, boost::lexical_cast<unsigned int>(9600), "COM4"); //TODO
		// run the IO service as a separate thread, so the main thread can block on standard input 
		boost::thread t(boost::bind(&boost::asio::io_service::run, &io_service)); 
		while (_serialPort.active()) // check the internal state of the connection to make sure it's still running 
		{ 
			char ch; 
			cin.get(ch); // blocking wait for standard input 
			if (ch == 3) // ctrl-C to end program 
				break; 
			_serialPort.write(ch); 
		} 
		_serialPort.close(); // close the minicom client connection 
		t.join(); // wait for the IO service thread to close 
	}
	catch (std::exception& e) 
	{ 
		cout << "Exception: " << e.what() << endl; 
	} 
	return 0; 
} 
