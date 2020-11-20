#include "QueueHandler.h"
#include <Windows.h>

QueueHandler::QueueHandler(TaskQueue* queue)
{
	this->queue = queue;
}

DWORD WINAPI QueueHandler::threadProc(LPVOID lpParam)
{
	QueueHandler* handler = static_cast<QueueHandler*>(lpParam);
	Task* task = handler->queue->getElement();
	if (task == nullptr)
		return -1;
	PFNCSORTSTRINGS sortProc = (PFNCSORTSTRINGS)task->proc;
	SortStringsParams params = *((SortStringsParams*)task->data);
	char** strings = params.strings;
	int count = params.count;
	sortProc(strings, count);
	return 1;
}

void QueueHandler::start(int threadAmount)
{
	HANDLE* threads = new HANDLE[threadAmount];

	for (int i = 0; i < threadAmount; i++)
	{
		threads[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadProc, this, 0, NULL);
	}

	WaitForMultipleObjects(threadAmount, threads, TRUE, INFINITE);
}
