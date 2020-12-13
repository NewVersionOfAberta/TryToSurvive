#pragma once
#include <Windows.h>
#include <iostream>

namespace ttsv {

	class Lock {
	public:

		Lock(CRITICAL_SECTION& l_mutex):m_mutex(l_mutex)
		{
			lock();
		}
		~Lock()
		{
			if (m_mutex.OwningThread) {
				unlock();
			}

		}
		void lock()
		{
			
			EnterCriticalSection(&m_mutex);
			//std::cout << "Locked " << m_mutex.OwningThread << std::endl;
		}
		void unlock()
		{
			std::cout << "Unlocked " << m_mutex.OwningThread << std::endl;
			//LeaveCriticalSection(&m_mutex);
		}
	private:
		CRITICAL_SECTION m_mutex;
	};
}