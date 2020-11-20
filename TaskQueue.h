#pragma once
#include <Windows.h>


struct Task
{
	void* proc;
	void* data;
};

struct  TaskNode
{
	Task task;
	TaskNode* next;
};

class TaskQueue
{
	HANDLE mutex;
	TaskNode root;
public:
	TaskQueue();
	void addElement(Task newTask);
	Task* getElement();
	bool isEmpty();
};

