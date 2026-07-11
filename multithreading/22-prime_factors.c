#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "multithreading.h"

/**
 * create_task - creates a new task
 * @entry: pointer to the entry function
 * @param: parameter to pass to the entry function
 *
 * Return: pointer to the created task, or NULL on failure
 */
task_t *create_task(task_entry_t entry, void *param)
{
	task_t *task;

	task = malloc(sizeof(task_t));
	if (!task)
		return (NULL);
	task->entry = entry;
	task->param = param;
	task->status = PENDING;
	task->result = NULL;
	pthread_mutex_init(&task->lock, NULL);
	return (task);
}

/**
 * destroy_task - destroys a task
 * @task: pointer to the task to destroy
 */
void destroy_task(task_t *task)
{
	if (!task)
		return;
	pthread_mutex_destroy(&task->lock);
	free(task);
}

/**
 * exec_tasks - thread entry to execute tasks from a list
 * @tasks: pointer to the list of tasks
 *
 * Return: NULL
 */
void *exec_tasks(list_t const *tasks)
{
	node_t *node;
	task_t *task;
	size_t idx;

	idx = 0;
	node = tasks->head;
	while (node)
	{
		task = (task_t *)node->content;
		pthread_mutex_lock(&task->lock);
		if (task->status == PENDING)
		{
			task->status = STARTED;
			tprintf("[%02lu] Started\n", idx);
			pthread_mutex_unlock(&task->lock);
			task->result = task->entry(task->param);
			pthread_mutex_lock(&task->lock);
			task->status = SUCCESS;
			tprintf("[%02lu] Success\n", idx);
			pthread_mutex_unlock(&task->lock);
		}
		else
		{
			pthread_mutex_unlock(&task->lock);
		}
		node = node->next;
		idx++;
	}
	return (NULL);
}
