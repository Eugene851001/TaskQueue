#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string>
#include <vector> 
#include <time.h>

#include "TaskQueue.h"
#include "QueueHandler.h"

#define MIN_THREAD_AMOUNT 1
#define MAX_THREAD_AMOUNT 256

typedef void (*PFNCHELLOUSER)(const char* username);
typedef void (*PFNCPRINTMESSAGE)();

TaskQueue* queue;

char** merge(char** strings1, int count1, char** strings2, int count2);

int compare(const char* str1, const char* str2)
{
	return strcmp(str1, str2);
}

char** merge(char** strings1, int count1, char** strings2, int count2)
{
	int countResult = count1 + count2;
	char** result = new char*[countResult];
	int j = 0, k = 0;
	for (int i = 0; i < countResult && j < count1 && k < count2; i++)
	{
		if (compare(strings1[j], strings2[k]) > 0)
		{
			result[i] = strings2[k];
			k++;
		}
		else
		{
			result[i] = strings1[j];
			j++;
		}
	}

	for (int i = j; i < count1; i++)
	{
		result[i + count2] = strings1[i];
	}

	for (int i = k; i < count2; i++)
	{
		result[i + count1] = strings2[i];
	}

	return result;
}

void sortStrings(char** strings, int count)
{
	for (int i = 0; i < count; i++)
	{
		for (int j = i; j < count; j++)
		{
			if (compare(strings[i], strings[j]) > 0)
			{
				char* temp = strings[i];
				strings[i] = strings[j];
				strings[j] = temp;
			}
		}
	}
}

char* readFile(const char* filename, int* size)
{
	FILE* f;
	f = fopen(filename, "r");
	if (f == nullptr)
	{
		return nullptr;
	}
	fseek(f, 0, SEEK_END);
	int length = ftell(f);
	char* buffer = new char[length];
	fseek(f, 0, SEEK_SET);
	fread(buffer, 1, length, f);
	fclose(f);
	*size = length;
	return buffer;
}

void writeFile(const char* filename, char** strings, int count)
{
	FILE* f;
	f = fopen(filename, "w");
	for (int i = 0; i < count; i++)
	{
		fwrite(strings[i], 1, strlen(strings[i]), f);
		fputc(10, f);
	}
	fclose(f);
}

bool isWhitespace(char c)
{
	return c == ' ' || c == 10 || c == 13;
}

char** parseText(char* source, int size, int* wordsAmount)
{
	std::vector<char*> *strings = new std::vector<char*>();
	int start = 0;

	for (int i = 0; i < size; i++)
	{
		if (isWhitespace(source[i]))
		{
			source[i] = 0;
			while (i < size && isWhitespace(source[i + 1]))
			{
				i++;
			}
			//запоминаем адрес начала строки
			strings->push_back(source + start);
			
			start = i + 1;
		}
	}

	//возвращаем кол-во слов
	*wordsAmount = strings->size();

	return &(*strings)[0];
}

int main()
{
	char filename[20];
	printf("Enter the file name: ");
	scanf("%s", filename);
	int size;
	char* text = readFile(filename, &size);
	if (text == nullptr)
	{
		printf("Please, check the file\n");
		return -1;
	}

	int totalWordsAmount;
	char** strings = parseText(text, size, &totalWordsAmount);
	int threadAmount;
	printf("Enter the threads amount: ");
	scanf("%d", &threadAmount);
	if (threadAmount < MIN_THREAD_AMOUNT || threadAmount > MAX_THREAD_AMOUNT)
	{
		printf("Threads amount should be from %d to %d\n", MIN_THREAD_AMOUNT, MAX_THREAD_AMOUNT);
		return -1;
	}

	int averageWordsAmount = totalWordsAmount / threadAmount;
	int delta = totalWordsAmount % threadAmount;

	//помещаем задания в очередь
	queue = new TaskQueue();
	Task task;
	task.proc = sortStrings;
	for (int i = 0; i < threadAmount; i++)
	{
		SortStringsParams* params = new SortStringsParams();
		params->strings = strings + i * averageWordsAmount;
		params->count = averageWordsAmount;
		if (i == threadAmount - 1)
		{
			params->count += delta;
		}
		task.data = params;
		queue->addElement(task);
	}

	int tmStart = clock();

	//создаём и запускаем обработчик очереди
	QueueHandler* qHandler = new QueueHandler(queue);
	qHandler->start(threadAmount);

	//обединение отсортированных частей методом сортирующего слияния
	char** sortedWords = strings;
	for (int i = 1; i <= threadAmount - 1; i++)
	{
		if (i == threadAmount - 1)
			sortedWords = merge(sortedWords, averageWordsAmount * i, strings + averageWordsAmount * i, averageWordsAmount + delta);
		else
			sortedWords = merge(sortedWords, averageWordsAmount * i, strings + averageWordsAmount * i, averageWordsAmount);
	}
	
	int tmEnd = clock();
	printf("Elapsed time: %d ms\n", (tmEnd - tmStart) * 1000 / CLOCKS_PER_SEC);

	writeFile("output.txt", sortedWords, totalWordsAmount);
	
	fflush(stdin);
	getchar();
	return 0;
}