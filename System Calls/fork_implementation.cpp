#include<iostream>
#include<unistd.h>    // for fork(), getpid(), getppid()
#include<sys/types.h>  // for pid_t
using namespace std;

int main() {
    cout << "Before fork()" << endl;

    pid_t pid = fork();   // fork() creates a new process

    if(pid < 0) {
        cerr << "Fork failed!" << endl;
        return 1;
    } else if(pid == 0) { // This block runs in the child process
        cout << "Child process: PID = " << getpid() << ", Parent PID = " << getppid() << endl;
    } else { // This block runs in the parent process
        cout << "Parent process: PID = " << getpid() << ", Child PID = " << pid << endl;
    }

    cout << "This line runs in both parent and child!" << endl;

    return 0;
}