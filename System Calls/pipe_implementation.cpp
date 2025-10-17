#include<iostream>
#include<unistd.h>    // for pipe(), fork(), read(), write(), close()
#include<cstring>      // for strlen()
using namespace std;

int main() {
    int fd[2];   // fd[0] = read end, fd[1] = write end

    if(pipe(fd) == -1) {
        cerr << "Pipe creation failed\n";
        return 1;
    }

    pid_t pid = fork();
    if(pid < 0) {
        cerr << "Fork failed\n";
        return 1;
    }

    if(pid == 0) {
        // ---- CHILD PROCESS ---- 
        close(fd[1]);    // child doesn't write while reading, so close write end

        char buffer[100];
        ssize_t bytesRead = read(fd[0], buffer, sizeof(buffer) - 1);
        buffer[bytesRead] = '\0';   // null-terminate

        cout << "Child received: " << buffer << endl;

        close(fd[0]);   // done reading
    } else {
        // ---- PARENT PROCESS ----
        close(fd[0]);    // parent doesn't read while writing, so close read end

        const char* msg = "Hello from the parent process!";
        write(fd[1], msg, strlen(msg));

        close(fd[1]);   // done writing
    }

    return 0;
}