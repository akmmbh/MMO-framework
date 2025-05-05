#pragma once
#include "net_common.h"

// i want to make a queue would be thread safe 
// as multiple client will try to access it and want to write to it


/*
 virtual function -:
  so it is used when base class and derived class both have same named function so 
  when we call any function it will already resolved at compile time based on type of pointer in being 
  used not the actual object is used 


  so when we use virtual key word it will resolve at 
  runtime based on actual object not on the pointer type 
  this is runtime polymorphism 


  // we have a vtable means a virtual table for mange a virtual functions 
*/
namespace olc
{ // i am writing this class to my olc namespace

	namespace net
	{
		template<typename T> 
		class tsqueue {
		public:
			tsqueue() = default;
			// i do not want to anyone copy my queue

			tsqueue(const tsqueue<T>&) = delete;
		     
			virtual ~tsqueue() { clear();
			
			//why i am using virtual with this destructor

			//if any derived class will made so all distructor will be called in correct roder
			// ensures coplete cleanup of derived object resources
			}
		public:
			//return and maintiains item at front of queue 
			const T& front() {
				// protect anything while running when below line will running 
				
				std::scoped_lock lock(muxQueue);
				
				return deqQueue.front();
		 }
			//return and maintains item at back of queue
			const T& back()
			{   //protect anything while running when below  line will running

				std::scoped_lock(muxQueue);
				return deqQueue.back();
			}
      // Add an item to back of queue
			void push_back(const T& item) {
				std::scoped_lock lock(muxQueue);
				//move is used to transfer the ownership of resources instead of coping 
				deqQueue.emplace_back(std::move(item));
			}

			//Add an item to front of Queue
			void push_front(const T& item) {
				std::scoped_lock lock(muxQueue);
				deqQueue.emplace_front(std::move(item));

			}
			//Return number of items in Queue
			size_t count()
			{
				std::scoped_lock lock(muxQueue);
				return deqQueue.size();
			}
			//clears Queue
			void clear() {
				std::scoped_lock lock(muxQueue);
				deqQueue.clear();

			}
			//Remove and return the item form the front of queue
			T pop_front()
			{
				std::scoped_lock lock(muxQueue);
				auto t = std::move(deqQueue.front());
				deqQueue.pop_front();
				return t;
			}
			//remove item from the back of queue
			T pop_back()
			{
				std::scoped_lock lock(muxQueue);
				auto t = std::move(deqQueue.back());
				deqQueue.pop_back();
				return t;
			}
		protected:
			std::mutex muxQueue;
			std::deque<T> deqQueue;

		};
	}
}