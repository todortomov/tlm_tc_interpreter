#include <errno.h>
#include <stdio.h>
#include <string.h>

/* Input line length maximum */
#define LINE_LEN_MAX 255

int main(int argc, char *argv[])
{
	FILE *input;
	char line[LINE_LEN_MAX];
	int ret = 0;

	if (argc != 3 || strcmp(argv[1], "-i")) {
		fprintf(stderr, "usage: %s -i batch_file\n", argv[0]);
		ret = 1;
		goto end;
	}

	input = fopen(argv[2], "r");
	if (input == NULL) {
		fprintf(stderr, "%s: error opening file \"%s\": %s\n",
			argv[0], argv[2], strerror(errno));
		ret = 1;
		goto end;
	}

	while(fgets(line, sizeof(line), input)) {
		if (line[strlen(line) - 1] != '\n') {
			fprintf(stderr, "%s: too long input line \"%s...\"\n",
				argv[0], line);
			ret = 1;
			goto end_fclose;
		}
	}

end_fclose:
	fclose(input);

end:
	return ret;
}
