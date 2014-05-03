/* Eitan Kondratovsky 315872937 */

#include <sys/fcntl.h>
#include <sys/types.h>
// #include <unistd.h‬‬>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
// #include <dirent.h‬‬>


#define WAITING 0
#define NO_C_FILE 1
#define COMPILATION_ERROR 2
#define COMPILATION_FINISH_SUCCESS 3
#define TIMEOUT 4
#define EXECUTION_FINISH_SUCCESS 5
#define BAD_OUTPUT 5
#define GREAT_JOB 6
#define RUN_FINISH_CORRECTLY 7
#define DELETE_FINISH 8

#define FORK_ERROR_MESSAGE "Cannot create new prosses\n"
#define FORK_ERROR 15
#define EXECV_ERROR_MESSAGE "Cannot execute execv\n"
#define EXECV_ERROR 16
#define ERROR_PID_MATCH_MESSAGE "Error child pid incorrect\n"
#define ERROR_PID_MATCH 17

#define FILE_READ_ERROR_MESSAGE "Cannot read file: "
#define FILE_READ_ERROR 7

#define OPEN_FILE_ERROR_MESSAGE "Cannot open file: "
#define OPEN_FILE_ERROR 8

#define OPEN_DIR_ERROR_MESSAGE "Cannot open directory: "
#define OPEN_DIR_ERROR 9

#define MISS_PARAMETER_MESSAGE "Expect one parameter to main\n"
#define MISS_PARAMETER_ERROR 11

#define SYSTEM_COMMAND_ERROR "Error while execute system command\n"

#define MAX_COFIG_LINE 80
#define MAX_COFIG_FILE_SIZE (3 * MAX_COFIG_LINE)
#define RESULT_FILE_NAME "results.csv"
#define MAX_FILE_NAME_LENGTH 255
#define MAX_GRADE_LENGTH 3




void GetConfigInfo(char *configFilePath, char *rootDirecrory,
		char *inputFilePath, char *outputFilePath);
void GetGrades(int resultFileDescriptor, char *path, char *input,
		char *correctOutput);
int GetCFile(char *personDirPath, char *cFilePath);
int CompileFiles(char *personDirPath);
int CompileAllFiles(int resultFileDescriptor, char *rootDirecroryPath);
int DeleteTemproryFiles(char *personDirPath);
int DeleteAllTemproryFiles(char *rootDirecroryPath);
int ExecuteAllFilesFrom(int resultFileDescriptor, char *rootDirecroryPath,
		char *inputFilePath);
int ExecuteFileFrom(char *personDirPath, char *inputFilePath);
int IsDirectory(char *dirPath);
int conainsExecutableFile(char *personDirPath);

/*

*/
int main(int argc, char *argv[]) {
	int fileDescriptor;
	char rootDirecrory[MAX_COFIG_LINE + 1], input[MAX_COFIG_LINE + 1],
			output[MAX_COFIG_LINE + 1];

	if (argc != 2) {
		write(2, MISS_PARAMETER_MESSAGE, strlen(MISS_PARAMETER_MESSAGE));
		exit(MISS_PARAMETER_ERROR);
	}

	

	GetConfigInfo(argv[1], rootDirecrory, input, output);

	umask(022);
	if((fileDescriptor = open(RESULT_FILE_NAME, O_RDWR | O_CREAT
			| O_TRUNC)) == -1) {
		write(2, OPEN_FILE_ERROR_MESSAGE, strlen(OPEN_FILE_ERROR_MESSAGE));
		write(2, RESULT_FILE_NAME, strlen(RESULT_FILE_NAME));
		write(2, "\n", 1);
		exit(OPEN_FILE_ERROR);
	}
	chmod(RESULT_FILE_NAME, 0666);

	// TODO something
	GetGrades(fileDescriptor, rootDirecrory, input, output);

	// close results file
	close(fileDescriptor);

	return 0;
}

/*


*/
void GetConfigInfo(char *configFilePath, char *rootDirecrory,
		char *inputFilePath, char *outputFilePath) {
	char configFIleInfo[MAX_COFIG_FILE_SIZE];
	int fileDescriptor, length;

	if ((fileDescriptor = open(configFilePath, O_RDONLY)) == -1) {
		write(2, OPEN_FILE_ERROR_MESSAGE, strlen(OPEN_FILE_ERROR_MESSAGE));
		write(2, configFilePath, strlen(configFilePath));
		write(2, "\n", 1);
		exit(OPEN_FILE_ERROR);
	}

	if((length = read(fileDescriptor, configFIleInfo, 
			MAX_COFIG_FILE_SIZE)) == -1) {
		write(2, FILE_READ_ERROR_MESSAGE, strlen(FILE_READ_ERROR_MESSAGE));
		exit(FILE_READ_ERROR);
	}

	strcpy(rootDirecrory, strtok(configFIleInfo, "\r\n"));
	strcpy(inputFilePath, strtok(NULL, "\r\n"));
	strcpy(outputFilePath, strtok(NULL, "\r\n"));

	// printf("configure file info:\n");
	// printf("%s\n", rootDirecrory);
	// printf("%s\n", inputFilePath);
	// printf("%s\n", outputFilePath);

	// close configure file
	close(fileDescriptor);
}

/*
run with DFS on all sub directores.
input: derictory path

*/
void GetGrades(int resultFileDescriptor, char *path, char *input,
		char *correctOutput) {
	// DIR *directory;
	// struct dirent *dirent;
	// char innerDirPath[MAX_COFIG_LINE + MAX_FILE_NAME_LENGTH + 1];
	// char personGrade[MAX_GRADE_LENGTH + 1], gradeCase[80];

	// directory = opendir(path);
	// if(directory == NULL) {
	// 	write(2, OPEN_DIR_ERROR_MESSAGE, strlen(OPEN_DIR_ERROR_MESSAGE));
	// 	write(2, path, strlen(path));
	// 	write(2, "\n", 1);
	// 	exit(OPEN_DIR_ERROR);
	// }

	// compile all files in sub derictories
	CompileAllFiles(resultFileDescriptor, path);
	// while ((dirent = readdir(directory)) != NULL) {
 //        strcpy(innerDirPath, path);
 //        strcat(innerDirPath, "/");
 //        strcat(innerDirPath, dirent->d_name);
 //        if (strcmp(dirent->d_name, ".") == 0
 //        		|| strcmp(dirent->d_name, "..") == 0) {
 //        	// do nothing
 //        } else if (IsDirectory(innerDirPath)) {
 //        	CompileFiles(innerDirPath);
 //        	GetGrades(resultFileDescriptor, innerDirPath, input, correctOutput);
 //        }
 //    }

    // execute all files
    ExecuteAllFilesFrom(resultFileDescriptor, path, input);
	// while ((dirent = readdir(directory)) != NULL) {
 //        strcpy(innerDirPath, path);
 //        strcat(innerDirPath, "/");
 //        strcat(innerDirPath, dirent->d_name);
 //        if (strcmp(dirent->d_name, ".") == 0
 //        		|| strcmp(dirent->d_name, "..") == 0) {
 //        	// do nothing
 //        } else if (IsDirectory(innerDirPath)) {
 //        	// ComputeGrade(innerDirPath, input, correctOutput);
 //        	// break;
 //        	// CompileFiles(innerDirPath);
 //        	// GetGrades(resultFileDescriptor, innerDirPath, input, correctOutput);
 //        	// strcpy(personGrade, "0");
 //        	// strcpy(gradeCase, "NO_C_FILE");
 //        	// write(resultFileDescriptor, dirent->d_name,
 //        	// 	strlen(dirent->d_name));
 //        	// write(resultFileDescriptor, ",", 1);
 //        	// write(resultFileDescriptor, personGrade, strlen(personGrade));
 //        	// write(resultFileDescriptor, ",", 1);
 //        	// write(resultFileDescriptor, gradeCase, strlen(gradeCase));
 //        	// write(resultFileDescriptor, "\r\n", 2);
 //        }
 //    }

    // delete all temprory files
    DeleteAllTemproryFiles(path);
 //    while ((dirent = readdir(directory)) != NULL) {
 //        strcpy(innerDirPath, path);
 //        strcat(innerDirPath, "/");
 //        strcat(innerDirPath, dirent->d_name);
 //        if (strcmp(dirent->d_name, ".") == 0
 //        		|| strcmp(dirent->d_name, "..") == 0) {
 //        	// do nothing
 //        } else if (IsDirectory(innerDirPath)) {
 //        	// TODO delete a.out and output.txt
 //        	// TODO also on subdirs
 //        }
 //    }

	// closedir(directory);
}

#define TIMEOUT_MESSAGE "TIMEOUT"

int ExecuteAllFilesFrom(int resultFileDescriptor, char *rootDirecroryPath, char *inputFilePath) {
	DIR *directory;
	struct dirent *dirent;
	char innerDirPath[MAX_FILE_NAME_LENGTH + 1];
	int result;

	directory = opendir(rootDirecroryPath);
	if(directory == NULL) {
		write(2, OPEN_DIR_ERROR_MESSAGE, strlen(OPEN_DIR_ERROR_MESSAGE));
		write(2, rootDirecroryPath, strlen(rootDirecroryPath));
		write(2, "\n", 1);
		exit(OPEN_DIR_ERROR);
	}

	while ((dirent = readdir(directory)) != NULL) {
        strcpy(innerDirPath, rootDirecroryPath);
        strcat(innerDirPath, "/");
        strcat(innerDirPath, dirent->d_name);
        if (strcmp(dirent->d_name, ".") == 0
        		|| strcmp(dirent->d_name, "..") == 0) {
        	// do nothing
        } else if (IsDirectory(innerDirPath)) {
        	if (conainsExecutableFile(innerDirPath)) {
        		printf("execute file functiion call\n");
	        	result = ExecuteFileFrom(innerDirPath, inputFilePath);
	        	if (result == TIMEOUT) {
	        		write(resultFileDescriptor, dirent->d_name,
        				strlen(dirent->d_name));
	        		write(resultFileDescriptor, ",0,", 3);
	        		write(resultFileDescriptor, TIMEOUT_MESSAGE,
	        				strlen(TIMEOUT_MESSAGE));
	        		write(resultFileDescriptor, "\r\n", 2);
	        	}
	        }
        	ExecuteAllFilesFrom(resultFileDescriptor, innerDirPath, inputFilePath);
        }
    }

    closedir(directory);
}

// int ExecuteFileFrom(char *personDirPath, char *inputFilePath) {
// 	char outputFilePath[MAX_FILE_NAME_LENGTH],
// 			executeFIlePath[MAX_FILE_NAME_LENGTH];
// 	int inputFileDescriptor, outputFileDescriptor, i, *status;
// 	int result = RUN_FINISH_CORRECTLY;
// 	pid_t pid, waitPid;

// 	if ((inputFileDescriptor = open(inputFilePath, O_RDONLY)) == -1) {
// 		write(2, OPEN_FILE_ERROR_MESSAGE, strlen(OPEN_FILE_ERROR_MESSAGE));
// 		write(2, inputFilePath, strlen(inputFilePath));
// 		write(2, "\n", 1);
// 		exit(OPEN_FILE_ERROR);
// 	}

// 	strcpy(outputFilePath, personDirPath);
// 	strcat(outputFilePath, "/output.txt");

// 	umask(022);
// 	if ((outputFileDescriptor = open(outputFilePath, O_RDWR
// 			| O_CREAT)) == -1) {
// 		write(2, OPEN_FILE_ERROR_MESSAGE, strlen(OPEN_FILE_ERROR_MESSAGE));
// 		write(2, outputFilePath, strlen(outputFilePath));
// 		write(2, "\n", 1);
// 		exit(OPEN_FILE_ERROR);
// 	}
// 	chmod(outputFileDescriptor, 0666);

// 	dup2(inputFileDescriptor, 0);
// 	dup2(outputFileDescriptor, 1);

// 	strcpy(executeFIlePath, personDirPath);
// 	strcat(executeFIlePath, "/a.out");

// 	pid = fork();
//     if(pid < 0) { // in case of error
//     	write(2, FORK_ERROR_MESSAGE, strlen(FORK_ERROR_MESSAGE));
//     	exit(FORK_ERROR);
//     } else if (pid == 0) { // child
//     	execv(executeFIlePath, NULL);
//     	write(2, EXECV_ERROR_MESSAGE, strlen(EXECV_ERROR_MESSAGE));
//     	exit(EXECV_ERROR);
//     } else { // parent
//     	waitPid = -1;
//     	for (i = 0; i < 5 && waitPid != -1; i++) {
//     		sleep(1);
//     		waitPid = waitpid(pid, status, WNOHANG);
//     	}
//     	if (waitPid == -1) {
//     		printf("timeout\n");
//     		result = TIMEOUT;
//     	}
//     	if (waitPid != pid) {
//     		write(2, ERROR_PID_MATCH_MESSAGE, strlen(ERROR_PID_MATCH_MESSAGE));
//     		exit(ERROR_PID_MATCH);
//     	}
//     }

//     close(inputFileDescriptor);
//     close(outputFileDescriptor);

//     return result;
// }


int ExecuteFileFrom(char *personDirPath, char *inputFilePath) {
	char outputFilePath[MAX_FILE_NAME_LENGTH],
			executeFilePath[MAX_FILE_NAME_LENGTH];
	int inputFileDescriptor, outputFileDescriptor, i, *status;
	pid_t pid, waitPid;
	int result = EXECUTION_FINISH_SUCCESS;

	if ((inputFileDescriptor = open(inputFilePath, O_RDONLY)) == -1) {
		write(2, OPEN_FILE_ERROR_MESSAGE, strlen(OPEN_FILE_ERROR_MESSAGE));
		write(2, inputFilePath, strlen(inputFilePath));
		write(2, "\n", 1);
		exit(OPEN_FILE_ERROR);
	}

	strcpy(outputFilePath, personDirPath);
	strcat(outputFilePath, "/output.txt");

	umask(022);
	if ((outputFileDescriptor = open(outputFilePath, O_RDWR
			| O_CREAT)) == -1) {
		write(2, OPEN_FILE_ERROR_MESSAGE, strlen(OPEN_FILE_ERROR_MESSAGE));
		write(2, outputFilePath, strlen(outputFilePath));
		write(2, "\n", 1);
		exit(OPEN_FILE_ERROR);
	}
	chmod(outputFilePath, 0666);

	strcpy(executeFilePath, personDirPath);
	strcat(executeFilePath, "/a.out");

	pid = fork();
	if(pid < 0) {
		write(2, FORK_ERROR_MESSAGE, strlen(FORK_ERROR_MESSAGE));
		exit(FORK_ERROR);
	} else if (pid == 0) { // child
		SwapProssesData(executeFilePath, inputFileDescriptor, outputFileDescriptor);
		exit(0); // we wont get here
	} else { // parent
		waitPid = -1;
    	for (i = 0; i < 5 && waitPid != -1; i++) {
    		sleep(1);
    		waitPid = waitpid(pid, status, WNOHANG);
    	}
    	if (waitPid == -1) {
    		printf("timeout\n");
    		result = TIMEOUT;
    	}
    	else if (waitPid != pid) {
    		write(2, ERROR_PID_MATCH_MESSAGE, strlen(ERROR_PID_MATCH_MESSAGE));
    		exit(ERROR_PID_MATCH);
    	}
	}

	close(inputFileDescriptor);
	close(outputFileDescriptor);

	return result;
}

int SwapProssesData(char *executeFilePath, int inputFileDescriptor, int outputFileDescriptor) {
	dup2(inputFileDescriptor, 0);
	dup2(outputFileDescriptor, 1);

	execv(executeFilePath, NULL);
	write(2, EXECV_ERROR_MESSAGE, strlen(EXECV_ERROR_MESSAGE));
	exit(EXECV_ERROR);	
}

int DeleteAllTemproryFiles(char *rootDirecroryPath) {
	DIR *directory;
	struct dirent *dirent;
	char innerDirPath[MAX_FILE_NAME_LENGTH + 1];

	directory = opendir(rootDirecroryPath);
	if(directory == NULL) {
		write(2, OPEN_DIR_ERROR_MESSAGE, strlen(OPEN_DIR_ERROR_MESSAGE));
		write(2, rootDirecroryPath, strlen(rootDirecroryPath));
		write(2, "\n", 1);
		exit(OPEN_DIR_ERROR);
	}

	while ((dirent = readdir(directory)) != NULL) {
        strcpy(innerDirPath, rootDirecroryPath);
        strcat(innerDirPath, "/");
        strcat(innerDirPath, dirent->d_name);
        if (strcmp(dirent->d_name, ".") == 0
        		|| strcmp(dirent->d_name, "..") == 0) {
        	// do nothing
        } else if (IsDirectory(innerDirPath)) {
        	DeleteTemproryFiles(innerDirPath);
        	DeleteAllTemproryFiles(innerDirPath);
        }
    }

    closedir(directory);
}

#define NO_C_FILE_MESSAGE "NO_C_FILE"
#define COMPILATION_ERROR_MESSAGE "COMPILATION_ERROR"

int CompileAllFiles(int resultFileDescriptor, char *rootDirecroryPath) {
	DIR *directory;
	struct dirent *dirent;
	char innerDirPath[MAX_FILE_NAME_LENGTH + 1];
	int result;

	directory = opendir(rootDirecroryPath);
	if(directory == NULL) {
		write(2, OPEN_DIR_ERROR_MESSAGE, strlen(OPEN_DIR_ERROR_MESSAGE));
		write(2, rootDirecroryPath, strlen(rootDirecroryPath));
		write(2, "\n", 1);
		exit(OPEN_DIR_ERROR);
	}

	while ((dirent = readdir(directory)) != NULL) {
        strcpy(innerDirPath, rootDirecroryPath);
        strcat(innerDirPath, "/");
        strcat(innerDirPath, dirent->d_name);
        if (strcmp(dirent->d_name, ".") == 0
        		|| strcmp(dirent->d_name, "..") == 0) {
        	// do nothing
        } else if (IsDirectory(innerDirPath)) {
        	result = CompileFiles(innerDirPath);
        	if (result == NO_C_FILE) {
        		printf("cccccccccccccccccccccc");
        		write(resultFileDescriptor, dirent->d_name,
        				strlen(dirent->d_name));
        		write(resultFileDescriptor, ",0,", 3);
        		write(resultFileDescriptor, NO_C_FILE_MESSAGE,
        				strlen(NO_C_FILE_MESSAGE));
        		write(resultFileDescriptor, "\r\n", 2);
        	} else if (result == COMPILATION_ERROR) {
        		write(resultFileDescriptor, dirent->d_name,
        				strlen(dirent->d_name));
        		write(resultFileDescriptor, ",0,", 3);
        		write(resultFileDescriptor, COMPILATION_ERROR_MESSAGE,
        				strlen(COMPILATION_ERROR_MESSAGE));
        		write(resultFileDescriptor, "\r\n", 2);
        	}
        	CompileAllFiles(resultFileDescriptor, innerDirPath);
        }
    }

    closedir(directory);
}

int DeleteTemproryFiles(char *personDirPath) {
	char cFilePath[MAX_FILE_NAME_LENGTH + 1],
			command[MAX_FILE_NAME_LENGTH + 1];
	int exeFileFound = 0, outpurFileFound = 0;
	DIR *directory;
	struct dirent *dirent;
	int result = 0;

	if ((directory = opendir(personDirPath)) == NULL) {
		write(2, OPEN_DIR_ERROR_MESSAGE, strlen(OPEN_DIR_ERROR_MESSAGE));
		exit(OPEN_DIR_ERROR);
	}

	while ((dirent = readdir(directory)) != NULL) {
		if (strcmp(dirent->d_name, "a.out") == 0) {
			exeFileFound = 1;
		} else if (strcmp(dirent->d_name, "output.txt") == 0) {
			outpurFileFound = 1;
		}
	}

	closedir(directory);

	if (exeFileFound) {
	    strcpy(command, "unlink ");
	    strcat(command, personDirPath);
	    strcat(command, "/a.out");
		// printf("%s\n", command);
	    result = system(command);

	    if (result != 0) {
    		write(2, SYSTEM_COMMAND_ERROR, strlen(SYSTEM_COMMAND_ERROR));
	    }
	}
	if (outpurFileFound) {
	    strcpy(command, "unlink ");
	    strcat(command, personDirPath);
	    strcat(command, "/output.txt");
		// printf("%s\n", command);
	    result = system(command);

	    if (result != 0) {
	    	write(2, SYSTEM_COMMAND_ERROR, strlen(SYSTEM_COMMAND_ERROR));
	    }
	}

    return DELETE_FINISH;
}

// int ExecuteFileFrom(char *personDirPath, char *inputFilePath) {
// 	char outputFilePath[MAX_FILE_NAME_LENGTH],
// 			executeFIlePath[MAX_FILE_NAME_LENGTH];
// 	int inputFileDescriptor, outputFileDescriptor, i, *status;
// 	int result = RUN_FINISH_CORRECTLY;
// 	pid_t pid, waitPid;

// 	if ((inputFileDescriptor = open(inputFilePath, O_RDONLY)) == -1) {
// 		write(2, OPEN_FILE_ERROR_MESSAGE, strlen(OPEN_FILE_ERROR_MESSAGE));
// 		write(2, inputFilePath, strlen(inputFilePath));
// 		write(2, "\n", 1);
// 		exit(OPEN_FILE_ERROR);
// 	}

// 	strcpy(outputFilePath, personDirPath);
// 	strcat(outputFilePath, "/output.txt");

// 	umask(022);
// 	if ((outputFileDescriptor = open(outputFilePath, O_RDWR
// 			| O_CREAT)) == -1) {
// 		write(2, OPEN_FILE_ERROR_MESSAGE, strlen(OPEN_FILE_ERROR_MESSAGE));
// 		write(2, outputFilePath, strlen(outputFilePath));
// 		write(2, "\n", 1);
// 		exit(OPEN_FILE_ERROR);
// 	}
// 	chmod(outputFileDescriptor, 0666);

// 	dup2(inputFileDescriptor, 0);
// 	dup2(outputFileDescriptor, 1);

// 	strcpy(executeFIlePath, personDirPath);
// 	strcat(executeFIlePath, "/a.out");

// 	pid = fork();
//     if(pid < 0) { // in case of error
//     	write(2, FORK_ERROR_MESSAGE, strlen(FORK_ERROR_MESSAGE));
//     	exit(FORK_ERROR);
//     } else if (pid == 0) { // child
//     	execv(executeFIlePath, NULL);
//     	write(2, EXECV_ERROR_MESSAGE, strlen(EXECV_ERROR_MESSAGE));
//     	exit(EXECV_ERROR);
//     } else { // parent
//     	waitPid = -1;
//     	for (i = 0; i < 5 && waitPid != -1; i++) {
//     		sleep(1);
//     		waitPid = waitpid(pid, status, WNOHANG);
//     	}
//     	if (waitPid == -1) {
//     		printf("timeout\n");
//     		result = TIMEOUT;
//     	}
//     	if (waitPid != pid) {
//     		write(2, ERROR_PID_MATCH_MESSAGE, strlen(ERROR_PID_MATCH_MESSAGE));
//     		exit(ERROR_PID_MATCH);
//     	}
//     }

//     close(inputFileDescriptor);
//     close(outputFileDescriptor);

//     return result;
// }

/*

*/
int CompileFiles(char *personDirPath) {
	char cFilePath[MAX_COFIG_LINE + MAX_FILE_NAME_LENGTH + 1],
			command[MAX_COFIG_LINE + 2 * MAX_FILE_NAME_LENGTH + 1];
	int result;

	if (GetCFile(personDirPath, cFilePath) == 0) {
		// printf("NO_C_FILE\n");
    	return NO_C_FILE;
    }

    strcpy(command, "gcc -o ");
    strcat(command, personDirPath);
    strcat(command, "/a.out ");
    strcat(command, cFilePath);
	// printf("%s\n", command);

    result = system(command);
    // printf("the value returned was: %d\n", result);

    if (result != 0) {
    	// printf("COMPILATION_ERROR\n");
    	return COMPILATION_ERROR;
    }
    return COMPILATION_FINISH_SUCCESS;
}

// int ComputeGrade(char *personDirPath, char *input, char *correctOutput) {
// 	char cFilePath[MAX_COFIG_LINE + MAX_FILE_NAME_LENGTH + 1],
// 			command[MAX_COFIG_LINE + 2 * MAX_FILE_NAME_LENGTH + 1];
// 	pid_t pid, waitPid;
// 	int result, *status, i;

// 	if (GetCFile(personDirPath, cFilePath) == 0) {
// 		printf("NO_C_FILE\n");
//     	return NO_C_FILE;
//     }

//     strcpy(command, "gcc -o ");
//     strcat(command, personDirPath);
//     strcat(command, "/a.out ");
//     strcat(command, cFilePath);
// 	printf("%s\n", command);

//     result = system(command);
//     printf("the value returned was: %d\n", result);

//     if (result != 0) {
//     	printf("COMPILATION_ERROR\n");
//     	return COMPILATION_ERROR;
//     }

//     pid = fork();
//     if(pid < 0) { // in case of error
//     	write(2, FORK_ERROR_MESSAGE, strlen(FORK_ERROR_MESSAGE));
//     	exit(FORK_ERROR);
//     } else if (pid == 0) { // child
//     	printf("Execute file from: %s\n", personDirPath);
//     	ExecuteFileFrom(personDirPath, input);
//     } else { // parent
//     	waitPid = -1;
//     	for (i = 0; i < 5; i++) {
//     		sleep(1);
//     		waitPid = waitpid(-1, status, WNOHANG);
//     	}
//     	if (waitPid != -1) {

//     	}
//     }

//     // printf("GREAT_JOB\n");
//     // return GREAT_JOB;
//     return WAITING;
//     // strcpy(command, "./");
//     // strcat(command, personDirPath);
//     // strcat(command, "/a.out ");

//     // printf("%s\n", command);

//     // pid = fork();
//     // if(pid < 0) { // in case of error

//     // } else if (pid == 0) { // child

//     // 	exit(0);
//     // } else { // parent

//     // }
// }


/*

*/
int GetCFile(char *personDirPath, char *cFilePath) {
	DIR *directory;
	struct dirent *dirent;
	int result = 0, len;

	if ((directory = opendir(personDirPath)) == NULL) {
		write(2, OPEN_DIR_ERROR_MESSAGE, strlen(OPEN_DIR_ERROR_MESSAGE));
		exit(OPEN_DIR_ERROR);
	}

	while ((dirent = readdir(directory)) != NULL) {
		len = strlen(dirent->d_name);
		if (len - 2 >= 0 && dirent->d_name[len - 2] == '.'
				&& dirent->d_name[len - 1] == 'c') {
			strcpy(cFilePath, personDirPath);
			strcat(cFilePath, "/");
			strcat(cFilePath, dirent->d_name);
			// printf("%s - 122223334344\n", cFilePath);
			result = 1;
			break;
		}
	}

	closedir(directory);
	return result;
}


int conainsExecutableFile(char *personDirPath) {
	DIR *directory;
	struct dirent *dirent;
	int result = 0;

	if ((directory = opendir(personDirPath)) == NULL) {
		write(2, OPEN_DIR_ERROR_MESSAGE, strlen(OPEN_DIR_ERROR_MESSAGE));
		exit(OPEN_DIR_ERROR);
	}

	while ((dirent = readdir(directory)) != NULL) {
		if (strcmp(dirent->d_name, "a.out")) {
			result = 1;
			break;
		}
	}

	closedir(directory);
	return result;
}

/*

*/
int IsDirectory(char *dirPath) {
	DIR *directory;
	int result = 1; 

	// printf("%s\n", dirPath);

	if ((directory = opendir(dirPath)) == NULL) {
		result = 0;
	}

	closedir(directory);
	return result;
}

/*

*/
// int IsCFile(char *filePath) {

// }

/*

*/
// void CompareFiles(char *firstFilePath, char *secondFilePath) {

// }

