#include "task.h"
#include <stdio.h>

#define TASK_FILE "data/tasks.dat"

int delete_task(Workspace *workspace) {

    FILE *fp = fopen(TASK_FILE, "rb");
    FILE *temp = fopen("data/temp.dat", "wb");

    if(fp == NULL || temp == NULL) {

        printf("\nFile open error!\n");
        return 0;
    }

    int task_id;

    printf("\nEnter Task ID To Delete: ");
    scanf("%d", &task_id);

    Task task;
    int found = 0;

    while(fread(&task, sizeof(Task), 1, fp)) {

        
        if(task.workspace_id == workspace->id &&
           task.task_id == task_id) {

            found = 1;
            continue;  
        }

        fwrite(&task, sizeof(Task), 1, temp);
    }

    fclose(fp);
    fclose(temp);

   
    remove(TASK_FILE);


    rename("data/temp.dat", TASK_FILE);

    if(found) {

        printf("\nTask Deleted Successfully!\n");
        return 1;
    }

    printf("\nTask Not Found!\n");
    return 0;
}