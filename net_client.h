

#pragma once

#include "net_common.h"
#include "net_tsqueue.h"
#include "net_message.h"
#include "net_connection.h"

namespace olc
{
	namespace net
	{
		template<typename T>
		class client_interface
		{

			client_interface() :m_socket(m_context)
			{
				//intialize the socket with the io context so it can do stuff
		    }
			virtual ~client_interface()
			{
				// if the client is destroyed always try and disconnect from the server
				Disconnect();
			}
		public:
			//connect to server with hostname/ip-address and port
			bool Connect(const std::string& host, const uint16_t port) {

				try {
					//create a connection
					m_connection = std::make_unique<connection<T>>();//TODO

					//Resolve hostname./ip address into tangiable physical address
					asio::ip::tcp::resolver resolver(m_context);
					m_endpoints = resolver.resolve(host, std::to_string(port));

					//tell the connection object to connect to server
					m_connection->ConnectToServer(m_endpoints);

					//start context thread
					thrContext = std::thread([this]() {m_context.run();});
				}
				catch (std::exception& e) {
					std::cerr << "Client Exception: " << e.what() << "\n";
					return false;
				}
				return false;
			 }

			//Disconnect from server
			void Disconnect() {
             //if connection exits and it is connected then..
				if (IsConnnected()) {
					//..disconnect from server gracefully
					m_connection->Disconnect();
				}
				//either way we are also done with the asio context..
				m_context.stop();
				//.. and its thread

				if (thrContext.joinable()) {
					thrContext.join();
				}
				//destroy the connection object 
				m_connection.release();
			}
			// check if client is actually connected to a server
			bool IsConnected() {
				if (m_connection)return m_connection->IsConnected();
				else return false;
			}
			//retrieve queue of messages from the server 
			tsqueue, owned_message<T >> &Incoming()
			{
				return m_qMessagesIn;
			}
		protected :
			//asio context handles the data transfer..
			asio::io_context m_context;
			/////..... but needs athread of its own to excute its work commands;
			std::thread thrContext;


			// This is the hardware socket that is connected to server
			asio::ip::tcp::socket m_socket;

			//the client has a single instance of a connection object which handles data transfer
			std::unique_ptr<connection<T>> m_connection;
		private:
			//This is the thread safe queue of incomming messages from the server
			tsqueue<owned_message<T>> m_qMessagesIn;

		 };
		

	}
}