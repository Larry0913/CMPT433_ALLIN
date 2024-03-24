#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>

#include "joystick.h"
#include "threadController.h"

static pthread_t joystick_id;

static void runCommand(char* command);
static bool isPressed(enum joystick direction);

void joystick_init()
{
    runCommand("config-pin p8.14 gpio"); //up
    runCommand("config-pin p8.15 gpio"); //right
    runCommand("config-pin p8.16 gpio"); //down
    runCommand("config-pin p8.18 gpio"); //left
    runCommand("config-pin p8.17 gpio"); //in

    runCommand("echo in > /sys/class/gpio/gpio26/direction"); //up
    runCommand("echo in > /sys/class/gpio/gpio47/direction"); //right
    runCommand("echo in > /sys/class/gpio/gpio46/direction"); //down
    runCommand("echo in > /sys/class/gpio/gpio65/direction"); //left
    runCommand("echo in > /sys/class/gpio/gpio27/direction"); //in

    pthread_create(&joystick_id, NULL, joystickThread, NULL);
}

void joystick_wait()
{
    pthread_join(joystick_id, NULL);
}

void joystick_cleanup(void)
{

}

void *joystickThread(void *args)
{
    (void)args;

    while(running_flag)
    {
        if (isPressed(UP))
        {
           
        }
        else if (isPressed(RIGHT))
        {
            
        }
        else if (isPressed(DOWN))
        {
         
        }
        else if (isPressed(LEFT))
        {
            
        }
        else if (isPressed(IN))
        {
            
        }
    }
    
    pthread_exit(NULL);
    return NULL;
}

static bool isPressed(enum joystick direction)
{
    char filePath[128];
    snprintf(filePath,128,"/sys/class/gpio/gpio%d/value", direction);
    //read file
    FILE *pFile = fopen(filePath, "r");
    if (pFile == NULL) {
        printf("ERROR OPENING %s.\n", filePath);
        exit(-1);
    }
    //get value
    char value[128];
    fgets(value, 128, pFile);
    // close file
    fclose(pFile);
    return !atoi(value);
}

static void runCommand(char* command)
{
    // Execute the shell command (output into pipe)
    FILE *pipe = popen(command, "r");
    // Ignore output of the command; but consume it
    // so we don't get an error when closing the pipe.
    char buffer[1024];
    while (!feof(pipe) && !ferror(pipe)) {
        if (fgets(buffer, sizeof(buffer), pipe) == NULL)
            break;
    // printf("--> %s", buffer); // Uncomment for debugging
    }
    // Get the exit code from the pipe; non-zero is an error:
    int exitCode = WEXITSTATUS(pclose(pipe));
    if (exitCode != 0) {
        perror("Unable to execute command:");
        printf(" command: %s\n", command);
        printf(" exit code: %d\n", exitCode);
    }
}

