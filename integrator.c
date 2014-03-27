#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <math.h>


int my_pow (int x, int y) {
	int i = 0;
	int res = x;
	for (i = 1 ; i < y; ++i) {
		res = res * res;
	}
	if (y == 0)
		return 1;
	return res;
}


float my_atoi(char* value) {
	float res = 0;
	int i;
	int pos = strlen(value);
	char point = 0;
	for (i = 0 ; i < strlen(value); ++i) {
		if (value[i] == '.')
			pos = i;
	}
	for (i = 0 ; i < strlen(value); ++i) {
		if (value[i] == '.') {
			point = 1;
			continue;
		}
		if (point == 0) {
		
			res += (float)((int)value[i] - 48) * ( my_pow(10, pos - i - 1) );
		
		}
		else {
		
			res += (float)((int)value[i] - 48) / ( my_pow(10, i - pos) );
		
		}
	}
	return res;
}


double func(double x) {
	return x ;
}


double simple_integrate(double a, double b, double dx, double (*func)(double)) {
	double result = 0, i;
	for(i = a; i < b; i += dx)
		result += ((func(i + dx) + func(i))/2) * dx;
	return result;
}



//1 процесс запустить

double integrate(int argc, char** argv, double (*func)(double)) {

	float b = my_atoi(argv[2]);
	float a = my_atoi(argv[1]);
	float dx = my_atoi(argv[3]);
	int processors = atoi(argv[4]);
	float d = (b - a) / processors;
	double result = 0, current_res = 0;
	
	int i;
	
	for (i = 0; i < processors; ++i) {
		int pid = fork();
		if (pid == 0) {
			int fd = open("calc", O_WRONLY | O_CREAT | O_APPEND, 0666);
			double current_res = 0;
			if ( processors > 1 )
				current_res = simple_integrate(a + d * i, a + d * (i + 1), dx, func);
			else
				current_res = simple_integrate(a, b, dx, func);
			write(fd, &current_res, sizeof(current_res));
			close(fd);
			exit(0);
		}
		else if (pid == -1) {
			
			perror("PID");
			exit(0);
			
		}
	}
	
	int fd = open("calc", O_RDONLY);
	for (i = 0; i < processors; ++i) {
		while( read(fd, &current_res, sizeof(current_res)) != sizeof(current_res) )
			sleep(1);
		result += current_res;
	}
	
	close(fd);
	unlink("calc");

	return result;	
}

int main(int argc, char** argv) {
	if (argc != 5) {
		printf("Wrong number of arguments \n");
		exit(0);
	}
	else {
		double result = 0;
		result = integrate(argc, argv, func);
		printf("%f \n", result);
	}
	return 0;
}
