#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#define MAX_ASSIGNMENTS 2
#define MAX_STUDENTS 10
#define MAX_CHAPTERS 3


/*calculate the average grade of each assignment */
void calculate_average(int chapter, int assignment, int grades[MAX_STUDENTS][MAX_CHAPTERS][MAX_ASSIGNMENTS]) {
    int sum = 0;
    for (int i = 0; i < MAX_STUDENTS; i++) {
        /* Sum the grades of all students for the current assignment */
        sum += grades[i][chapter][assignment];
    }
    /* Calculate the average grade */
    float average = (float)sum / MAX_STUDENTS;
    printf("Assignment %d - average = %.6f\n",++assignment, average);
}

int main(int argc, char *argv[]) {
    int grades[MAX_STUDENTS][MAX_CHAPTERS][MAX_ASSIGNMENTS];
    FILE *fp;
    fp = fopen("sample_in_grades.txt", "r");
    /* To check if the file was successfully opened */
    if (fp == NULL) {
        printf("Error opening file\n");
        exit(1);
    }
    /* Check grades from the file into the grades array */
    for (int i = 0; i < MAX_STUDENTS; i++) {
        for (int j = 0; j < MAX_CHAPTERS; j++) {
            for (int k = 0; k < MAX_ASSIGNMENTS; k++) {
                fscanf(fp, "%d", &grades[i][j][k]);
            }
        }
    }
    /* flies closes here */
    fclose(fp);

    pid_t pid;
    /* Loop of all chapters */
    for (int chapter = 0; chapter < MAX_CHAPTERS; chapter++) {
        /* Generate a GradTA process for each chapter */
        pid = fork();

        if (pid == 0) {
            for (int assignment = 0; assignment < MAX_ASSIGNMENTS; assignment++) {
                pid = fork();
                if (pid == 0) {
                    /* Calculate and print the average grade for the current assignment */
                    calculate_average(chapter, assignment, grades);
                    /* Exit the TA process */
                    exit(0);
                }
                /* Wait for the current TA process to complete */
                wait(NULL);
            }
            exit(0);
        }
        /* Wait for the current GradTA process to complete */
        wait(NULL);
    }

    return 0;
}
