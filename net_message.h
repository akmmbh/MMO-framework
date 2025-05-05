#pragma once
#include "net_common.h"


namespace olc
{
	namespace net 
	{
		//Message Header is sent at start of all messages.The template allows us 
		// to use " enum class" to ensure that the messages are valid at complile time 
		template<typename T> 
		struct message_header
		{
			T id{};
			uint32_t size = 0;
		};

		template <typename T>
		struct message
		{
			message_header<T> header{};

			std::vector<uint8_t> body;
			//return size of the entire message packet in bytes
			size_t size()const
			{
				return sizeof(message_header<T>) + body.size();

			}
			//override for std::cout compatibility - produce friendly description of message
			friend std::ostream& operator << (std::ostream& os, const message<T>& msg) {
				os << "ID:" << int(msg.header.id) <<"SIZE:" << msg.header.size;
				return os;
			}

			//pushes any POD - like data into message buffer
			template<typename DataType> 
			friend message<T>& operator <<(message<T>& msg, const DataType& data) {
             
				//check that the type of data being pushed is trivally copyable
				static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to be pushed");
				//Cache curent size of vector , as this will be the point we insert the data
				size_t i = msg.body.size();


				//Resize the vector by size fo the data being pushed 
				msg.body.resize(msg.body.size() + sizeof(DataType));

				//physically copy the data into the newly allocaed vector space
				std::memcpy(msg.body.data() + i, &data, sizeof(DataType));

				//recalculate the message size
                msg.header.size = msg.size();

				//return the target message so it can be chained

				return msg;
			}

			template<typename DataType> 
			friend message<T>& operator >> (message<T>& msg, DataType& data) {

				//check that the type of data being pushed is trivially copyable
				static_assert(std::is_standard_layout<DataType>::value, "data is to compled to be get from the vector");

				//cache the location toward the end of the vector where the pulled data starts
				size_t i = msg.body.size() - sizeof(DataType);

				//physically copy the data from the vector into the user variable 

				std::memcpy(&data, msg.body.data() + i, sizeof(DataType));


				//Shrink the vector to remove read bytes and reaset end position
				msg.body.resize(i);

				//Recalculate the message size
				msg.header.size = msg.size();
				

				//Return the target message so it can be "chained"
				return msg;


			}
		};


		//forward declare the connection 
		template <typename T>
		class connection;


		template <typename T>
		struct owned_message
		{
			std::shared_ptr<connection<T>> remote = nullptr;
			message<T> msg;


			//Again a friendly string maker 
			friend std::ostream& operator<< (std::ostream& os, const owned_message<T>& msg) {
				os << msg.msg;
				return os;
			}
		};
	}
}