// mpi
#include <mpi.h>

// std
#include <cstdio>
#include <cmath>

const double g_pi = 3.14159265358979323846264338327950288419716939937510582;


// calculate partial integral of 4/(1+x^2) on [0,1) and returns partial integral
double partial_integral(const int myid, const int numprocs, const int intervals){
		const double h = 1.0 / intervals;
		double sum = 0.0;
		for(int i = myid + 1; i <= intervals; i += numprocs){
			const double x = h * (i - 0.5);
			sum += 1.0 / (1.0 + x*x);
		}
		return 4 * h * sum;
}

int main(int argc, char *argv[]){

	if(MPI_Init( &argc, &argv ) != MPI_SUCCESS){
		MPI_Abort( MPI_COMM_WORLD, 1 );
	}
	
	int numprocs;
	if(MPI_Comm_size(MPI_COMM_WORLD, &numprocs) != MPI_SUCCESS){
		MPI_Abort( MPI_COMM_WORLD, 2 );
	}
	
	int myid;
	if(MPI_Comm_rank(MPI_COMM_WORLD, &myid) != MPI_SUCCESS){
		MPI_Abort( MPI_COMM_WORLD, 3 );
	}
	
	int n = 0;
	if(myid == 0){
		std::cout << "Enter the number of intervals:\n";
		scanf("%d",&n);
		std::cout << "Entered: " << n << "\n";
	}
	
	MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD); 
	
	// not asking time after "MPI_Bcast" or MPI_Wtime also count the time for the input

	const double time1 = MPI_Wtime();

	const double partial_pi = partial_integral(myid, numprocs, n);


	double pi = 0.0;
	MPI_Reduce(&partial_pi, &pi,         1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	
	// asking time after "MPI_Reduce" or MPI_Wtime will not count the time for collecting the result

	const double timetot = MPI_Wtime() - time1;
	double timetotale = 0.0;
	MPI_Reduce(&timetot,    &timetotale, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

	double timemin = 0.0;
	MPI_Reduce(&timetot,    &timemin,    1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD);

	double timemax = 0.0;
	MPI_Reduce(&timetot,    &timemax,    1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

	if(myid == 0){
		std::cout << "pi is approximately " << pi << "\n";
		std::cout << "approximation error is " << std::abs(pi - g_pi) << "\n";
		
		std::cout << "minimum time is approximately " << timemin << "\n";
		std::cout << "maximum time is approximately " << timemax << "\n";
		std::cout << "average time is approximately " << timetotale/numprocs << "\n";
	}
	
	if(MPI_Finalize() != MPI_SUCCESS){
		MPI_Abort( MPI_COMM_WORLD, 6 );
	}
	
	return 0;
}
