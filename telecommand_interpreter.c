#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
	int ret = 0;

	if (argc != 3 || strcmp(argv[1], "-i")) {
		fprintf(stderr, "usage: %s -i batch_file\n", argv[0]);
		ret = 1;
	}

	return ret;
}
