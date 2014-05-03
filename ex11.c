/* Eitan Kondratovsky 315872937 */

#include <stdio.h>
#include <sys/fcntl.h>
#include <stdlib.h>
#include <string.h>

#define PARAMETER_NUMBER_ERROR \
"Error. Please enter exectly 2 arguments.\n"
#define PARAMETER_ERROR_SIZE 42
#define BUFFER_SIZE 10
#define ERROR_SIZE 50
#define ERROR_DETECTED 5
#define IGNORE_CHARACTER '\r'

size_t IntToString(char *str, int x);

/*
The program inputes the two files' paths from the terminal line and opens the
given files using the system call open. After it, the program compars between
both files and returns the result of comaring, 2 if both given files are the
same and otherwise, returns 1. Before returning the result we close both files.
*/
int main(int argc, char *argv[]) {
	char buffer1[BUFFER_SIZE], buffer2[BUFFER_SIZE];
	char *firstFilePath, *secondFilePath;
	int firstFileDescriptor, secondFileDescriptor;
	int countFile1, countFile2;
	int result = 0, index1, index2, error = 0, i;

	write(1, argv[0], strlen(argv[0]));

	if (argc != 3) {
		write(2, PARAMETER_NUMBER_ERROR, PARAMETER_ERROR_SIZE);
		return ERROR_DETECTED;
	}

	firstFilePath = argv[1];
	secondFilePath = argv[2];

	firstFileDescriptor = open(firstFilePath, O_RDONLY);

	// TODO remove it
	IntToString(buffer1, firstFileDescriptor);
	write(1, "ffd was declarated\n", 19);
	write(1, buffer1, strlen(buffer1));
	write(1, "\n", 1);

	// means file open did not take place 
	if (firstFileDescriptor < 0){
		write(2, "error after open file1\n", 23);
		return ERROR_DETECTED;
	}

	secondFileDescriptor = open(secondFilePath, O_RDONLY);

	// TODO remove it
	IntToString(buffer1, secondFileDescriptor);
	write(1, "sfd was declarated\n", 19);
	write(1, buffer1, strlen(buffer1));
	write(1, "\n", 1);

	// means file open did not take place 
	if (secondFileDescriptor < 0){
		// write an error message
		write(2, "error after open file2\n", 23);
		// close first file discriptor
		close(firstFileDescriptor);
		return ERROR_DETECTED;
	}

	result = 2; // we assume the files are the same and check it later

	if((countFile1 = read(firstFileDescriptor, buffer1, BUFFER_SIZE)) == -1) {
		write(2, "error read file1\n", 17);
		// close both files
		close(firstFileDescriptor);
		close(secondFileDescriptor);
		exit(6);
	}

	index1 = 0;

	if((countFile2 = read(secondFileDescriptor, buffer2, BUFFER_SIZE)) == -1) {
		write(2, "error read file2\n", 17);;
		// close both files
		close(firstFileDescriptor);
		close(secondFileDescriptor);
		exit(6);

	}

	index2 = 0;

	for (i=0; i < 30 && countFile1 != 0 && countFile2 != 0; i++) {
		// index1 = index2 = 0;
		printf("start loop\n");
		while (index1 < countFile1 && index2 < countFile2) {
			printf("%c = %c\n", buffer1[index1], buffer2[index2]);
			if (buffer1[index1] == IGNORE_CHARACTER) {
				printf("ignor character\n");
				index1++;
			} else if (buffer2[index2] == IGNORE_CHARACTER) {
				printf("ignor character\n");
				index2++;
			} else if (buffer1[index1] != buffer2[index2]) {
				result = 1;
				// close both files
				close(firstFileDescriptor);
				close(secondFileDescriptor);
				exit(1);
			} else {
				printf("both equals\n");
				index1++;
				index2++;
			}
		}
		printf("end loop\n");
		if (index1 == countFile1) {
			index1 = 0;
			if((countFile1 = read(firstFileDescriptor, buffer1,
					BUFFER_SIZE)) == -1) {
				write(2, "error read file1\n", 17);
				// close both files
				close(firstFileDescriptor);
				close(secondFileDescriptor);
				exit(6);
			}
		}
		if (index2 == countFile2) {
			index2 = 0;
			if((countFile2 = read(secondFileDescriptor, buffer2,
					BUFFER_SIZE)) == -1) {
				write(2, "error read file2\n", 17);;
				// close both files
				close(firstFileDescriptor);
				close(secondFileDescriptor);
				exit(6);
			}
		}
	}
	if (countFile1 != 0) {
		printf("fix it!!!!!!!!!!!!!!!!1\n");
	}
	else if (countFile2 != 0) {
		printf("fix it!!!!!!!!!!!!!!!!1\n");
	}

	// close both files
	close(firstFileDescriptor);
	close(secondFileDescriptor);

	return result;
}

// TODO remove it
/*
IntToString converts the int x to a decimal numeral, which is written to s.
No terminal null character is written.  The number of characters written is
returned.  s MUST point to space with enough room for the numeral,
including a leading '-' if x is negative.
*/
size_t IntToString(char *s, int x)
{
    //  Set pointer to current position.
    char *p = s;

    //  Set t to absolute value of x.
    unsigned t = x;
    if (x < 0) t = -t;

    //  Write digits.
    do
    {
        *p++ = '0' + t % 10;
        t /= 10;
    } while (t);

    //  If x is negative, write sign.
    if (x < 0)
        *p++ = '-';

    //  Remember the return value, the number of characters written.
    size_t r = p-s;

    // add end of string char
    *(p) = '\0';

    //  Since we wrote the characters in reverse order, reverse them.
    while (s < --p)
    {
        char t = *s;
        *s++ = *p;
        *p = t;
    }

    return r;
}