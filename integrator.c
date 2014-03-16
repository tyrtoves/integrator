#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

double func(double x) {
	return x ;
}


double simple_integrate(double a, double b, double dx, double (*func)(double)) {
	double result = 0, i;
	for(i = a; i < b; i += dx)
		result += ((func(i + dx) + func(i))/2) * dx;
	return result;
}

double integrate(double a, double b, double dx, int processors, double (*func)(double)) {
	double d = (b - a) / processors;
	int i;
	for (i = 0; i < processors; ++i) {
		int pid = fork();
		if (pid == 0) {
			int fd = open("calc", O_WRONLY | O_CREAT | O_APPEND, 0666);
			double current_res = simple_integrate(a + d * i, a + d * (i + 1), dx, func);
			write(fd, &current_res, sizeof(current_res));
			close(fd);
			exit(0);
		} 
		else if (pid == -1) {
			
			perror("PID");
		}
	}
	double result = 0, current_res;
	int fd = open("calc", O_RDONLY);
	for (i = 0; i < processors; ++i) {
		while( read(fd, &current_res, sizeof(current_res)) != sizeof(current_res) )
			sleep(1);
		result += current_res;
	}
	close(fd);
	return result;	
}

int main() {
	double result = integrate(0, 10, 0.0001, 4, func);
	printf("%f \n", result);
	return 0;
}
