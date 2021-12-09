#include <iostream>
#include <process.h>
#include <Windows.h>

const size_t ARRAY_SIZE = 110;
const size_t THREADS = 4;

using INFORM = struct data {
	int* a;
	size_t left, right;
	int result = 1;
};

//заполняем массив случайными числами 
void initRandomArray(int* arr) {
	for (size_t i = 0; i < ARRAY_SIZE; i++)
		arr[i] = rand() % 100;
	arr[ARRAY_SIZE - 1] = -100; //мин четное число
}

//ф-я поиска минимального из четных элементов
int findMinEven(int* arr) {
	int result = 1;
	for (size_t i = 0; i < ARRAY_SIZE; i++) {
		if (arr[i] % 2 == 0 && (result == 1 || arr[i] < result)) {
			result = arr[i];
		}
	}
	return result;
}

unsigned __stdcall minEven(void* arg) {
	INFORM* inform = (INFORM*)arg;
	inform->result = 1;
	for (size_t i = inform->left; i < inform->right; i++)
		if (inform->a[i] % 2 == 0 && (inform->result == 1 || inform->a[i] < inform->result)) {
			inform->result = inform->a[i];
		}
	_endthreadex(0);
	return 0;
}

int findMinEvenParallel(int* arr) {
	HANDLE t[THREADS];
	INFORM informs[THREADS];
	size_t n = ARRAY_SIZE / THREADS;
	for (size_t i = 0; i < THREADS; i++) {
		informs[i].a = arr;
		informs[i].left = n * i;
		informs[i].result = 1;
		if (i == THREADS - 1)
			informs[i].right = ARRAY_SIZE;
		else
			informs[i].right = n * (i + 1);
		t[i] = (HANDLE)_beginthreadex(nullptr, 0, &minEven, &informs[i], 0, nullptr);
		/*std::cout << std::endl << "-------=" << i << "=-------"  << std::endl;
		std::cout << "informs[i].a = " << informs[i].a << std::endl;
		std::cout << "informs[i].left = " << informs[i].left << std::endl;
		std::cout << "informs[i].right = " << informs[i].right << std::endl;
		std::cout << "informs[i].result = " << informs[i].result << std::endl;
		*/
	}
	WaitForMultipleObjects(THREADS, t, true, INFINITE);

	int globalMinEven = informs[0].result;
	for (size_t i = 0; i < THREADS; i++) {
		if (informs[i].result < globalMinEven)
			globalMinEven = informs[i].result;
	}

	for (size_t i = 0; i < THREADS; i++)
		CloseHandle(t[i]);

	return globalMinEven;
}

int main() {
	int arr[ARRAY_SIZE];
	srand(GetTickCount64());
	initRandomArray(arr);
	
	std::cout << "Min even number without parallel: " << findMinEven(arr) << std::endl;
	std::cout << "Min even number with parallel: " << findMinEvenParallel(arr) << std::endl;
	return 0;
}                    