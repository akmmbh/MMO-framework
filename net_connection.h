#pragma once

#pragma once

#include "net_common.h"
#include "net_tsqueue.h"
#include "net_message.h"


namespace olc
{
	namespace net
	{
		template<typename T>
		class connection : public std::enable_shared_from_this<connection<T>>
		{   //we have inherit from this whihc let us to create a shared pointer internally from withint this object

		public:
			connection()
			{
			}

			virtual ~connection()
			{
			}
		public:
			//only asked by client
			bool ConnectToServer();
			//can ask both cliend and server
			bool Disconnect();
			//
			bool IsConnected() const;
		public:
			bool Send(const message<T>& msg);

		protected:
			// each connection has a unique socked to a remote
			//means a connection can have different clients  
			boost::asio::ip::tcp::socket m_socket;

			//This context is shared with whole asio instance 
			// but we do not want to have different asio contect for diff clients
			asio::io_context& m_asioContext;

			//This queue holds all the message to be sent to remote side 
			//of the connection
			tsqueue<message<T>> m_qMessagesOut;


			// this queue holds all the messages that have been revieved from teh remote side of this connection . Note it is a reference
			//as the owner of this connectio is expected to provide a queue
			tsqueue<owned_message>& m_qMessagesIn;








		};

	}
}