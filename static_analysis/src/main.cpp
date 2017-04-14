
#include <cstring>
#include <string>

/// Some documentation
struct s{
	std::string i;
};

int main(int argc, char *argv[]){
	int b = 1;
	b++;


	void* p;

	int* c = new int(2);


	char dir[1024];
	std::strcpy(dir, argv[1]);
	std::sprintf(dir, argv[1]);
}

