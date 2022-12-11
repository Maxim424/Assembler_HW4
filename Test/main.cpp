#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fstream>
#include <ctime>

const int chefNumber = 1; // Количество поваров.

int potCapacity; // Вместимость горшка с едой.
int potValue; // Текущее количество еды в горшке.
bool isChefSleeping; // Хранит true, если повар спит. Иначе -  false.

sem_t sem; // Семафор для синхронизации потоков.

char **params; // Аргументы командной строки.
bool isFileIO = false;

// Функция потоков–производителей (поваров).
void *Chef(void *param) {
    int pNum = *((int *) param);
    while (1) {
        sem_wait(&sem); // Использование семафора.
        if (!isChefSleeping) {
            sleep(1); // Имитация времени приготовления еды.
            potValue = potCapacity; // Повар приготовил еду.
            if (isFileIO) {
                std::ofstream out;
                out.open(params[3], std::ios::app);
                out << "Chef " << pNum << " has just cooked the food. Pot currentAmount = " << potValue << "\n";
                out.close();
            }
            printf("Chef %d has just cooked the food. Pot currentAmount = %d\n", pNum, potValue);
            isChefSleeping = true; // Когда еда готова, повар идет спать.
        }
        sem_post(&sem); // Использование семафора.
    }
}

// Функция потоков–потребителей (каннибалов).
void *Cannibal(void *param) {
    int cNum = *((int *) param);
    while (1) {
        sem_wait(&sem); // Использование семафора.
        if (potValue > 0) {
            if (isChefSleeping) {
                --potValue; // Еды стало на единицу меньше.
                if (isFileIO) {
                    std::ofstream out;
                    out.open(params[3], std::ios::app);
                    out << "Cannibal " << cNum << " just ate food. Pot currentAmount = " << potValue << "\n";
                    out.close();
                }
                printf("Cannibal %d just ate food. Pot currentAmount = %d\n", cNum, potValue);
            }
        } else {
            isChefSleeping = false; // Каннибал будит повара, если в горшке не осталось еды.
        }
        sem_post(&sem); // Использование семафора.
        sleep(1);
    }
}

int main(int argc, char **argv) {
    int n, m;
    bool flag = false;
    params = argv;
    if (argc == 4) {
        if (*argv[1] == 'i') {
            try {
                n = std::stoi(argv[2]);
                m = std::stoi(argv[3]);
                if (n <= 0 || m <= 0) {
                    flag = true;
                }
            } catch (...) {
                std::cout << "Incorrect value\n";
                flag = true;
            }
        } else if (*argv[1] == 'f') {
            isFileIO = true;
        }
        else {
            std::cout << "Incorrect input\n";
            return 0;
        }
    } else if (argc == 2 && *argv[1] == 'r') {
        srand(unsigned(time(0)));
        n = rand()%20 + 1;
        m = rand()%20 + 1;
        std::cout << "Cannibal number: " << n << "\n";
        std::cout << "Pot capacity: " << m << "\n";
    } else {
        flag = true;
    }

    if (flag) {
        do {
            std::cout << "Enter the number of cannibals: ";
            std::cin >> n;
        } while (n <= 0);

        do {
            std::cout << "Enter pot capacity: ";
            std::cin >> m;
        } while (m <= 0);
    }
    if (isFileIO) {
        FILE *f = fopen(argv[2], "r");
        fscanf(f, "%d %d", &n, &m);
        fclose(f);
        std::ofstream out;
        out.open(params[3]);
        out << "";
        out.close();
    }


    potCapacity = m;
    isChefSleeping = false; // Так как еды в горшке нет, повар просыпается и готовит ее.

    sem_init(&sem, 0, 0); // Инициализируем семафор.

    // Запуск потоков-поваров.
    pthread_t threadP[chefNumber];
    int producers[chefNumber];
    for (int i = 0; i < chefNumber; i++) {
        producers[i] = i + 1;
        pthread_create(&threadP[i], nullptr, Chef, (void *) (producers + i));
    }

    // Запуск потоков-каннибалов.
    pthread_t threadC[n];
    int consumers[n];
    for (int i = 0; i < n; i++) {
        consumers[i] = i + 1;
        pthread_create(&threadC[i], nullptr, Cannibal, (void *) (consumers + i));
    }

    sem_post(&sem); // Подаем сигнал повару о том, что можно начинать готовить еду.

    // Закрываем потоки каннибалов.
    for (int i = 0; i < n; ++i) {
        pthread_join(threadC[i], NULL);
    }

    // Закрываем потоки поваров.
    for (int i = 0; i < chefNumber; ++i) {
        pthread_join(threadP[i], NULL);
    }

    sem_destroy(&sem); // Уничтожаем семафор.

    return 0;
}