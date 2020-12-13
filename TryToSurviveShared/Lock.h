#pragma once
#include <Windows.h>
#include <iostream>
#include <mutex>


namespace ttsv {

	class Lock {
	public:

		Lock(std::mutex* l_mutex):m_mutex(l_mutex)
		{
			m_mutex->lock();
		}

		~Lock()
		{
			m_mutex->unlock();
		}

	private:
		std::mutex* m_mutex;
	};
}