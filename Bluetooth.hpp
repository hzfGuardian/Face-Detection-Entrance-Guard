/*************************************************************************
	> File Name: Bluetooth.hpp
	> Author: Huang Zhuofei
	> Mail: huangzhuofei@zju.edu.cn
	> Created Time: Tue Jun  7 17:02:26 2016
 ************************************************************************/

#include <wiringPi.h>
#include <wiringSerial.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

class Bluetooth {

public:
    Bluetooth(string path = "/dev/ttyAMA0") {
        fd = serialOpen(path.c_str(), 9600);
        if (fd == -1) {
            printf("Bluetooth open error!\n");
            exit(1);
        }
    }

    ~Bluetooth() {
        serialClose(fd);
    }

    void sendStr(char *str) {
        serialPuts(fd, str);        
    }

private:
    int fd;
}


