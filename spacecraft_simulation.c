/*
Nicholas Gibbs G01482635
CS 262, Project 1
This program accepts operator commands to keep the spacecraft alive.
*/
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

int getSeed() {
	printf("CubeSat Ground-Station Telemetry Console\nEnter integer seed: ");
	char buffer[100];
	int seed;
	fgets(buffer, 100, stdin);
	sscanf(buffer, "%d", &seed);
	printf("\n");
	srand(seed);
	return seed;
}

void printLine(int length, int* x, int t) {
	char buffer1[25];
        sprintf(buffer1, "%d", *x);
	if (t == 3) {
		if (*x == -1 || *x == 1) {
			strcat(buffer1, " degree");
		} else {	
			strcat(buffer1, " degrees");
		}
	} else if (t == 4) {
        	if (*x == 1) {
        	        strcpy(buffer1, "SAFE-MODE");
        	} else {
        	        strcpy(buffer1, "NORMAL");
        	}
	}
        int a = length - strlen(buffer1);
        printf("|");
        for (int i = 0; i < a / 2; i++) {
                printf(" ");
        }
        printf("%s", buffer1);
        for (int i = 0; i < a / 2 + (a % 2); i++) {
                printf(" ");
        }
}

void printTable(int* timestamp, int* temp, int* battery, int* orient_err, int* safe_mode) {
	printf("+---------+----------+----------+--------------------+------------+\n");
	printf("|   t(s)  |  temp C  | battery%% | orient err         |   mode     |\n");
	printf("+---------+----------+----------+--------------------+------------+\n");
	printLine(9, timestamp, 0);
	printLine(10, temp, 1);
	printLine(10, battery, 2);
	printLine(20, orient_err, 3);
	printLine(12, safe_mode, 4);
	printf("|\n");
	//printf("|    %d   |   %d     |   %d     |     %d degrees     |  %s        |\n", *timestamp, *temp, *battery, *orient_err, safe);
	printf("+---------+----------+----------+--------------------+------------+\n");
}

void packetGen(int* timestamp, int* temp, int* battery, int* orient_err, int* safe_mode, int* cticks) {
	*timestamp = *timestamp + 1;
	int clamp1 = (rand() % 10) - 5;
	*battery -= rand() % 3;
	int clamp2 = (rand() % 7) - 3;
	if (*safe_mode == 1) {
		if (clamp1 < -1) {
			clamp1 = -1;
		} else if (clamp1 > 1) {
			clamp1 = 1;
		}
		if (clamp2 < -1) {
			clamp2 = -1;
		} else if (clamp2 > 1) {
			clamp2 = 1;
		}
	}
	*temp += clamp1;
	if (*battery < 0) {
		*battery = 0;
	}
	*orient_err += clamp2;
	int rad_hits = rand() % 6;
	if (rad_hits == 5 && *safe_mode == 0) {
		*orient_err += 8;
	}
	if (*temp < -20) {
		printf("Mission failed: temperature exceeded -20C (current=%d)\n", *temp);
		exit(0);
	} else if (*temp > 90) {
		printf("Mission failed: temperature exceeded 90C (current=%d)\n", *temp);
		exit(0);
	} else if (*battery == 0) {
		printf("Mission failed: battery died (0%%)\n");
		exit(0);
	}
	if (abs(*orient_err) > 45) {
		*cticks = *cticks + 1;
		if (*cticks > 3) {
			printf("Mission failed: magnitude of orientation error > 45 (current=%d)\n", *orient_err);
                	exit(0);
		}
	} else {
		*cticks = 0;
	}
}

void commandMenu(int* timestamp, int* temp, int* battery, int* orient_err, int* safe_mode, int* cticks) {
	char buffer[3];
	printf("[L] Live-tick (process exactly 1 packet)\n");
	printf("[R] Run N ticks (simulate N packets)\n");
	printf("[T] Fire thruster (reduce |orient_err| by 5 degrees, costs 3%% battery)\n");
	printf("[S] Toggle SAFE-MODE (restricts commands and clamps drift)\n");
	printf("[Q] Quit mission\n");
	char option;
	printf("\n> ");
	fgets(buffer, 3, stdin);
	sscanf(buffer, "%s", &option);
	switch(option) {
		case 'l':
		case 'L':
			printf("\n");
			packetGen(timestamp, temp, battery, orient_err, safe_mode, cticks);
			printTable(timestamp, temp, battery, orient_err, safe_mode);
			commandMenu(timestamp, temp, battery, orient_err, safe_mode, cticks);
			break;
		case 'r':
		case 'R':
			;
			char number[100];
			int num;
			printf("Enter N (positive integer) ");
			fgets(number, 100, stdin);
			sscanf(number, "%d", &num);
			printf("\n");
			for (int i = 0; i < num; i++) {
				packetGen(timestamp, temp, battery, orient_err, safe_mode, cticks);
			}
			printTable(timestamp, temp, battery, orient_err, safe_mode);
			commandMenu(timestamp, temp, battery, orient_err, safe_mode, cticks);
			break;
		case 't':
		case 'T':
			if (*safe_mode == 1) {
				printf("Thrusters cannot be operated in SAFE-MODE\n\n");
				commandMenu(timestamp, temp, battery, orient_err, safe_mode, cticks);
				break;
			}
			if (*battery < 5) {
				printf("Thrusters not fired: battery below 5%%.\n\n");
				commandMenu(timestamp, temp, battery, orient_err, safe_mode, cticks);
				break;
			}
			if (*orient_err <= -5) {
				*orient_err += 5;
			} else if (*orient_err > -5 && *orient_err < 0) {
				*orient_err = 0;
			} else if (*orient_err > 0 && *orient_err <= 5) {
				*orient_err = 0;
			} else if (*orient_err > 5) {
				*orient_err -= 5;
			}
			*battery -= 3;
			printf("Thrusters fired.\n\n");
			commandMenu(timestamp, temp, battery, orient_err, safe_mode, cticks);
			break;
		case 's':
		case 'S':
			if (*safe_mode == 1) {
				*safe_mode = 0;
				printf("SAFE-MODE DISABLED.\n\n");
			} else {
				*safe_mode = 1;
				printf("SAFE-MODE ENABLED.\n\n");
			}
			commandMenu(timestamp, temp, battery, orient_err, safe_mode, cticks);
			break;
		case 'q':
		case 'Q':
			printTable(timestamp, temp, battery, orient_err, safe_mode);
			break;
		default:
			printf("Unrecognized command '%c'. Please choose L, R, T, S, or Q.\n\n", option);
			commandMenu(timestamp, temp, battery, orient_err, safe_mode, cticks);
	}
}

int main() {
	int a = 0;
	int* timestamp = &a;
	int b = 28;
	int* temp = &b;
	int c = 100;
	int* battery = &c;
	int d = 0;
	int* orient_err = &d;
	int e = 0;
	int* safe_mode = &e;
	int f = 0;
	int* cticks = &f;
	getSeed();
	commandMenu(timestamp, temp, battery, orient_err, safe_mode, cticks);
	return -1;
}
