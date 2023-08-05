#include <stdio.h>

int main()
{
	printf("Hello world from RV32I\n");
	printf("Fibonacci sequence:\n");

	int a=0, b=1, c=1;

	while(c < 1000)
	{
		printf("%d ", c);
		c = a + b;
		a = b;
		b = c;
	}

	printf("\nByee!\n");
	return 0;
}
