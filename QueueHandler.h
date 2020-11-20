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
	//static ��� ����, ����� ����� ���� ������������ ��� �������� ������
	//lpParam - ��������� �� ������ ������� ������
	static DWORD WINAPI threadProc(LPVOID lpParam);
public:
	TaskQueue* queue;
	QueueHandler(TaskQueue* task);
	void start(int threadAmount);
};

