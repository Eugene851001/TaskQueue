#include "TaskQueue.h"

TaskQueue::TaskQueue()
{
	mutex = CreateMutexA(NULL, FALSE, NULL);
	root.next = nullptr;
}

void TaskQueue::addElement(Task newTask)
{
	DWORD dwResult;
	dwResult = WaitForSingleObject(mutex, INFINITE);
	if (dwResult != WAIT_OBJECT_0)
	{
		return;
	}
	TaskNode *currentNode = &root;
	while (currentNode->next != nullptr)
	{
		currentNode = currentNode->next;
	}
	TaskNode *newNode = new TaskNode;
	newNode->next = nullptr;
	newNode->task = newTask;

	currentNode->next = newNode;

	ReleaseMutex(mutex);
}

Task* TaskQueue::getElement()
{
	DWORD dwResult;
	Task* result = nullptr;
	dwResult = WaitForSingleObject(mutex, INFINITE);
	if (dwResult != WAIT_OBJECT_0)
	{
		return nullptr;
	}

	if (!isEmpty())
	{
		result = &root.task;
		root = *root.next;
	}

	ReleaseMutex(mutex);
	return result;
}

bool TaskQueue::isEmpty()
{
	return root.next == nullptr;
}
