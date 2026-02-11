#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <conio.h>

#define MAX_QUESTIONS 50
#define TIME_LIMIT 15

struct Question {
    char text[200];
    char a[100], b[100], c[100], d[100];
    char correct;
};

int loadQuestions(struct Question *qList, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Cannot open file.\n");
        return 0;
    }

    int count = 0;
    while (fgets(qList[count].text, sizeof(qList[count].text), file)) {
        fgets(qList[count].a, sizeof(qList[count].a), file);
        fgets(qList[count].b, sizeof(qList[count].b), file);
        fgets(qList[count].c, sizeof(qList[count].c), file);
        fgets(qList[count].d, sizeof(qList[count].d), file);
        fscanf(file, " %c\n", &qList[count].correct);
        qList[count].text[strcspn(qList[count].text, "\n")] = '\0';
        qList[count].a[strcspn(qList[count].a, "\n")] = '\0';
        qList[count].b[strcspn(qList[count].b, "\n")] = '\0';
        qList[count].c[strcspn(qList[count].c, "\n")] = '\0';
        qList[count].d[strcspn(qList[count].d, "\n")] = '\0';

        count++;
        if (count >= MAX_QUESTIONS) break;
    }

    fclose(file);
    return count;
}

void shuffleQuestions(struct Question *qList, int count) {
    int i, j;
    struct Question temp;
    srand(time(NULL));
    for (i = count - 1; i > 0; i--) {
        j = rand() % (i + 1);
        temp = qList[i];
        qList[i] = qList[j];
        qList[j] = temp;
    }
}

void ask(struct Question q, int *score, int *attempted) {
    int i;
    char ans = '\0';
    int timeSpent = 0;

    printf("\n%s\n", q.text);
    printf("%s\n%s\n%s\n%s\n", q.a, q.b, q.c, q.d);
    printf("You have 15 seconds to answer...\n");

    for (i = 0; i < TIME_LIMIT; i++) {
        if (_kbhit()) {
            ans = _getch();
            if (ans >= 'a' && ans <= 'z') ans -= 32;  // Convert to uppercase
            break;
        }
        Sleep(1000);
        timeSpent++;
    }

    (*attempted)++;

    if (ans == '\0') {
        printf("Time's up! You didn't answer. Correct answer: %c\n", q.correct);
    } else if (ans == q.correct) {
        printf("Correct!\n");
        (*score)++;
    } else {
        printf("Wrong! You answered: %c | Correct: %c\n", ans, q.correct);
    }
}

void saveScore(const char *name, float percentage) {
    FILE *file = fopen("scores.txt", "a");
    if (file != NULL) {
        fprintf(file, "%s %.2f%%\n", name, percentage);
        fclose(file);
    } else {
        printf("Could not save score.\n");
    }
}

int main() {
    struct Question quiz[MAX_QUESTIONS];
    int totalQuestions, chosenQuestions, score = 0, attempted = 0;
    char name[50];
    int i;

    totalQuestions = loadQuestions(quiz, "questions.txt");
    if (totalQuestions == 0) return 1;

    printf("Welcome to the Quiz Game!\n");
    printf("Enter your name: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = '\0';

    do {
        printf("How many questions would you like to attempt? (4-50): ");
        scanf("%d", &chosenQuestions);
    } while (chosenQuestions < 4 || chosenQuestions > totalQuestions);

    shuffleQuestions(quiz, totalQuestions);

    printf("\nStarting quiz for %s...", name);
    printf("\n");
    for (i = 0; i < chosenQuestions; i++) {
        printf("\nQuestion %d: ",1+i);
        ask(quiz[i], &score, &attempted);
    }

    float percentage = (float)score / chosenQuestions * 100;
    printf("\nQuiz Over!\n%s, you scored %.2f%% (%d out of %d)\n", name, percentage, score, chosenQuestions);

    saveScore(name, percentage);

    printf("Your score has been saved in 'scores.txt'\n");
    return 0;
}