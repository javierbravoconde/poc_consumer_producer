//============================================================================
// Name        : poc_consumer_producer.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <string>
#include <list>
#include <sstream>
#include <unistd.h>


class ProducerConsumer {

public:

	ProducerConsumer()
	{
		//_readyToConsume = false;
		pthread_mutex_init(&_qLock, NULL);
		pthread_cond_init(&cond_ready_to_consume, NULL);
	}

	static void* consumeThread(void* ptr) {
		ProducerConsumer* self = static_cast<ProducerConsumer*>(ptr);
		while (1) {
			std::string data = self->consumeData();
			std::cout << "Consumed: " << data << std::endl;
		}
		return 0;
	}

	std::string consumeData() {

		pthread_mutex_lock (&_qLock);

		if(_buffer.empty()){ //If nothing produced so far, we sleep until something arrives
			pthread_cond_wait(&cond_ready_to_consume, &_qLock);
		}

		std::stringstream ss;

		while (!_buffer.empty()) {

			ss << _buffer.front();
			_buffer.pop_front();
		}

		std::string aProducedData = ss.str();

		pthread_mutex_unlock(&_qLock);

		return aProducedData;
	}

	static void* produceThread(void* ptr) {

		ProducerConsumer* self = static_cast<ProducerConsumer*>(ptr);
		self->produceData();

	}

	void produceData() {

		int counter = 0;

		while (1)
		{
			std::string aReadValue;
			std::cin >> aReadValue;

			std::cout << "Read: " << aReadValue << std::endl;

			pthread_mutex_lock (&_qLock);

			_buffer.push_back(aReadValue);
			++counter;

			if(counter >= 5){
				pthread_cond_signal (&cond_ready_to_consume);
			}
			pthread_mutex_unlock(&_qLock);
		}

	}

	std::list<std::string> _buffer;
	pthread_cond_t cond_ready_to_consume;
	pthread_mutex_t _qLock;

};

int main() {

	ProducerConsumer aProdCon;

	pthread_t aConsumerThread;
	pthread_t aProducerThread;

	pthread_create(&aProducerThread, NULL, &ProducerConsumer::produceThread, &aProdCon);
	pthread_create(&aConsumerThread, NULL, &ProducerConsumer::consumeThread, &aProdCon);

	pthread_join(aProducerThread, NULL);
	pthread_join(aConsumerThread, NULL);


	return 0;
}
