#pragma once

#include "TaskQueue.h"

typedef void (*PFNCSORTSTRINGS)(char** strings, int count);

typedef struct _SortStringsParams
{
	char** strings;
	int count;
} SortStringsParams;

class QueueHandler
{
	//static дл€ того, чтобы можно было использовать дл€ создани€ потока
	//lpParam - указатель на объект данного класса
	static DWORD WINAPI threadProc(LPVOID lpParam);
public:
	TaskQueue* queue;
	QueueHandler(TaskQueue* task);
	void start(int threadAmount);
};

