#ifndef _DIAGRAMS_B_H
#define _DIAGRAMS_B_H

#include <memory>

class A;

class B { 
	public: 
		std::unique_ptr<A> m_a; 
};

#endif
