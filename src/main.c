#include "../include/system.h"
#include "../include/task.h"

int main(void)
{
    if (system_init() != 0)
        return 1;
    return system_start();
}
