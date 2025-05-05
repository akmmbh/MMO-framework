#include<iostream>
#include<chrono>
#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif
#define ASIO_STANDALONE
//---------boost includees---------------//
#include<boost/asio.hpp>
#include<boost/asio/ts/buffer.hpp>
#include<boost/asio/ts/internet.hpp>
//---------------------------------------//
//-------------------Document or Concepts------------//






//---------------------------------------------------//

//-----------namespaces--------------------//
namespace asio = boost::asio;
namespace sys = boost::system;

//-----------------------------------------//

//---------------some data allocation-------//


std::vector<char>vBuffer(1 * 1024);

//-----------------------------------------//

//----------------Functions-----------------//




//we have operations we can asynchrounsly read the data from the site
void GrabSomeData(asio::ip::tcp::socket& socket) {
	//it is primarly a asio context with some work to do 
	socket.async_read_some(asio::buffer(vBuffer.data(), vBuffer.size()),
		[&](std::error_code ec, std::size_t length) {
			if (!ec) {
				std::cout << "\n\nRead" << length << "bytes\n\n";
				for (int i = 0;i < length;i++) {
					std::cout << vBuffer[i];

					GrabSomeData(socket);
				}
			}
		});
}
//------------------------------------------//

int main() {
	sys::error_code ec;
	//create a context = essentially the platform specific interface
	

	/*context is the space where asio do its work with job of asio to do
	so if asio do not have any thing to do it exit immidiately*/

	asio::io_context context;
	/*context is doing is waiting for some condition then
	excuting the code fo that condition
	and when we wait for something it just get blocks
   */

	//Give some fake tasks to asio so the context doesnot finish
	asio::executor_work_guard<asio::io_context::executor_type> idleWork = asio::make_work_guard(context);





	/*start the context
	//in this will run the context without blocking our main program
	giving it a seperate so when it need to stop and wait so it wil
	not block the main thread
	*/
	std::thread thrContext = std::thread([&]() {context.run();});

	//Get the address of somewhere we wish to connect to
	asio::ip::tcp::endpoint endpoint(asio::ip::make_address("51.38.81.49", ec), 80);

	//create a scoket , the contex will deliever the implementation
	asio::ip::tcp::socket socket(context);
	
	//tell socket to try and connect
	socket.connect(endpoint, ec);

	if (!ec) {
		std::cout << "Connected!" << std::endl;
	}
	else {
		std::cout << "Failed to connect to address :\n" << ec.message() << std::endl;
	}
	if (socket.is_open()) {
		GrabSomeData(socket);
		std::string sRequest = "GET /index.html HTTP/1.1\r\n"
			"Host: example.com\r\n"
			"Connection: close\r\n\r\n";
		//for reading and writing of data you have to deal with the
		//asio buffers where we have to sent the data and the size of it 

		socket.write_some(asio::buffer(sRequest.data(), sRequest.size()), ec);

		//this will wait until there is data present for reading
		
		/*
		//socket.wait(socket.wait_read);


		//size_t bytes = socket.available();
		//std::cout << "Bytes Available :" << bytes << std::endl;
		//
		////if they are available then i am able to read it

		//if (bytes > 0) {
		//	std::vector<char> vBuffer(bytes);
		//	socket.read_some(asio::buffer(vBuffer.data(), vBuffer.size()), ec);

		//	for (auto c : vBuffer)std::cout << c; 
		//}
		*/
		
		/*
		now also we donot get the data after we use asyn 
		so we have to think asynchronously and place the 
		GrabsomeData before writing to socket
		*/
		/*I donot want that my program close prematurely 
		  so i just wait for sometime*/

		using namespace std::chrono_literals;
		std::this_thread::sleep_for(2000ms);

		context.stop();
		if (thrContext.joinable())thrContext.join();
	}

	return 0;
}
