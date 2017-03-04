#ifndef _DIAGRAMS_A_H
#define _DIAGRAMS_A_H

#include <memory>

class A { 
	public: 
		std::unique_ptr<A> m_next = nullptr; 
};

#endif
