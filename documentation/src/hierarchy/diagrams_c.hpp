#ifndef _DIAGRAMS_C_H
#define _DIAGRAMS_C_H

#include "diagrams_a.hpp"

#include <memory>

class D;

class C : public A { 
	public: 
		std::unique_ptr<D> m_d; 
};

#endif
