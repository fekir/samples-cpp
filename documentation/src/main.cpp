
// class hierarchy sample, taken from http://www.stack.nl/~dimitri/doxygen/manual/diagrams.html
#include "hierarchy/diagrams_a.hpp"
#include "hierarchy/diagrams_b.hpp"
#include "hierarchy/diagrams_c.hpp"
#include "hierarchy/diagrams_d.hpp"
#include "hierarchy/diagrams_e.hpp"

#include <string>


#include "lib.hpp"

/// Some documentation
struct s{
	std::string i;
};

int main(){
	A a; (void)a;

	auto i = lib();
}

