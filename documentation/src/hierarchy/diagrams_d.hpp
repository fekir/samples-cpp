#ifndef _DIAGRAM_D_H
#define _DIAGRAM_D_H

#include "diagrams_a.hpp"
#include "diagrams_b.hpp"
#include "diagrams_c.hpp"

class C;

class D : virtual protected  A, private B { 
	public: 
		C m_c; 
};

#endif
