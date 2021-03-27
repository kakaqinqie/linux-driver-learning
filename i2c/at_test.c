#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<fcntl.h>
#include<unistd.h>

int write_size;
int main()
{
    int fd;
    int result;
    int i;
    scanf("%d", &write_size);

    char *buf = malloc(write_size);
    for(i = 0; i < write_size; i++) {
        buf[i] = i + 1;
    }
    fd = open("/sys/bus/i2c/devices/0-0050/e2prom", O_RDWR);
    result = write(fd, buf, write_size);
    printf("write: %d\n", result);
    memset(buf, 0, write_size);
    lseek(fd, 0, SEEK_SET);
    result = read(fd, buf, write_size);
    printf("read: %d\n", result);
    printf("read buf: ");
    for(i = 0; i < write_size; i++) {
       printf("%d ", buf[i]);
    }
    printf("\n");
    close(fd);
    return 0;
}
