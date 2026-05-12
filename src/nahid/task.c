#include "task.h"
#include <stdio.h>

#define TASK_FILE "data/tasks.dat"

int view_tasks(Workspace *workspace) {

    FILE *fp = fopen(TASK_FILE, "rb");

    if(fp == NULL) {

        printf("\nNo tasks found!\n");
        return 0;
    }

    Task task;

    int found = 0;

    printf("\n========== TASK LIST ==========\n");

    while(fread(&task,
                sizeof(Task),
                1,
                fp)) {

        if(task.workspace_id ==
           workspace->id) {

            found = 1;

            printf("\nTask ID: %d\n",
                   task.task_id);

            printf("Title: %s\n",
                   task.title);

            printf("Description: %s\n",
                   task.description);

            printf("Due Date: %s\n",
                   task.due_date);

            printf("Assigned To: %d\n",
                   task.assigned_to);

            printf("Status: ");

            if(task.status == 0)
                printf("Pending\n");

            else if(task.status == 1)
                printf("In Progress\n");

            else
                printf("Completed\n");

            printf("Priority: ");

            if(task.priority == 0)
                printf("Low\n");

            else if(task.priority == 1)
                printf("Medium\n");

            else
                printf("High\n");

            printf("----------------------------\n");
        }
    }

    fclose(fp);

    if(!found) {

        printf("\nNo tasks in this workspace.\n");
        return 0;
    }

    return 1;
}


int update_task(Workspace *workspace) {

    FILE *fp = fopen(TASK_FILE, "rb+");

    if(fp == NULL) {

        printf("\nTask file error!\n");
        return 0;
    }

    int task_id;

    printf("\nEnter Task ID To Update: ");
    scanf("%d", &task_id);

    Task task;

    while(fread(&task,
                sizeof(Task),
                1,
                fp)) {

        if(task.task_id == task_id &&
           task.workspace_id ==
           workspace->id) {

            getchar();

            printf("\nEnter New Title: ");

            fgets(task.title,
                  sizeof(task.title),
                  stdin);

            task.title[strcspn(task.title,
                               "\n")] = 0;

            printf("Enter New Description: ");

            fgets(task.description,
                  sizeof(task.description),
                  stdin);

            task.description[strcspn(task.description,
                                     "\n")] = 0;

            printf("Enter New Status: ");
            scanf("%d",
                  &task.status);

            printf("Enter New Priority: ");
            scanf("%d",
                  &task.priority);

            fseek(fp,
                  -sizeof(Task),
                  SEEK_CUR);          

            fwrite(&task,
                   sizeof(Task),
                   1,
                   fp);

            fclose(fp);

            printf("\nTask Updated Successfully!\n");

            return 1;
        }
    }

    fclose(fp);

    printf("\nTask Not Found!\n");

    return 0;
}