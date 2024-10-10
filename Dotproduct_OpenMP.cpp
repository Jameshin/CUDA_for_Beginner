// ���� A�� B�� ����(dot product) C�� ���ϴ� �����Դϴ�. 
// reduction�� critical session�� ���� ������Ѻ��ϴ�.
// n_repeat�� ������ ��հ� ����� Serial ������� �� �����մϴ�.

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#include <iostream>
#include <stack>
#include <ctime>

std::stack<clock_t> tictoc_stack;

void tic() {
	tictoc_stack.push(clock());
}

double toc() {
	double time_elapsed = ((double)(clock() - tictoc_stack.top())) / CLOCKS_PER_SEC;
	std::cout << "time elapsed : "
		<< time_elapsed
		<< std::endl;
	tictoc_stack.pop();
	return time_elapsed;
}

int main()
{
	int N = 30000000;
	double* A = new double[N];
	double* B = new double[N];
	double C_omp = 0.0f;
	double C_serial = 0.0f;
	int thread_count = 4;
	int n_repeat = 5;
	double serial_time = 0.0f;
	double critical_time = 0.0f;
	double reduction_time = 0.0f;
	int temp;
	int diff;

	for (int i = 0; i < N; i++)
	{
		A[i] = 1;
		B[i] = 1;
	}

	// 5�� �ݺ� ��
	for (int c = 0; c < n_repeat; c++) {


		// serial ����
		C_serial = 0.0f;
		tic();
		for (int i = 0; i < N; i++)
		{
			C_serial += A[i] * B[i];
		}
		std::cout << "Serial version ";
		serial_time+=toc();

		// critical session ���� 
		C_omp = 0.0f;
		tic();
#pragma omp parallel for num_threads(thread_count) private(temp) 
		for (int i = 0; i < N; i++)
		{
			temp = A[i] * B[i];
#pragma omp critical
			{
				C_omp += temp;
			}
		}
		std::cout << "OpenMP critical session version ";
		critical_time += toc();

		// ��� ����
		diff = abs(C_serial - C_omp);
		if (diff == 0)
			printf("OpenMP result is equal to serial result.\n");
		else
			printf("OpenMP result is different to serial result.\n");

		// reduction ���� 
		C_omp = 0.0f;
		tic();
#pragma omp parallel for num_threads(thread_count) private(temp) reduction(+:C_omp) 
		for (int i = 0; i < N; i++)
		{
			temp = A[i] * B[i];
			C_omp += temp;
		}
		std::cout << "OpenMP reduction version ";
		reduction_time += toc();

		// ��� ����
		diff = abs(C_serial - C_omp);
		if (diff == 0)
			printf("OpenMP result is equal to serial result.\n");
		else
			printf("OpenMP result is different to serial result.\n");


	}
	std::cout << "\nSerial version : " << serial_time / n_repeat << std::endl;
	std::cout << "OpenMP critical session version : " << critical_time / n_repeat << std::endl;
	std::cout << "OpenMP reduction version : " << reduction_time / n_repeat << std::endl;

	getchar();

	delete A;
	delete B;

	return 0;
}