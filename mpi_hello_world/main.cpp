// mpi
#include <mpi.h>

// std
#include <string>

// cstd
#include <cstdio>


int main(int argc, char *argv[]){
	// Initialize the MPI environment
	if(MPI_Init( &argc, &argv ) != MPI_SUCCESS){
		MPI_Abort( MPI_COMM_WORLD, 1 );
	}

	// Get the number of processes
	int world_size;
	if(MPI_Comm_size(MPI_COMM_WORLD, &world_size) != MPI_SUCCESS){
		MPI_Abort( MPI_COMM_WORLD, 2 );
	}
	
	// Get the rank of the process
	int world_rank;
	if(MPI_Comm_rank(MPI_COMM_WORLD, &world_rank) != MPI_SUCCESS){
		MPI_Abort( MPI_COMM_WORLD, 3 );
	}

	// Get the name of the processor
	std::string processor_name(MPI_MAX_PROCESSOR_NAME, '\0');
	int name_len = processor_name.size();
	if(MPI_Get_processor_name(&processor_name[0], &name_len) != MPI_SUCCESS){
		MPI_Abort( MPI_COMM_WORLD, 4 );
	}
	processor_name.resize(name_len);

	// Print off a hello world message from every process -> normally you would send all message to the first process and print them from there
	printf("Hello world from processor \"%s\", rank %d out of %d processors\n", processor_name.c_str(), world_rank, world_size);

	// Finalize the MPI environment. No more MPI calls can be made after this
	if(MPI_Finalize() != MPI_SUCCESS){
		MPI_Abort( MPI_COMM_WORLD, 6 );
	}

	return 0;
}
