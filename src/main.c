#include <signal.h>  // Must be first for struct sigaction
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

volatile sig_atomic_t actor_ready = 0;
volatile sig_atomic_t director_ready = 0;

void Director_handler(int sig) {
  director_ready = 1;
  printf("Director (PID: %d): Received SIGUSR2 from the Actor\n", getpid());
}

void Actor_handler(int sig) {
  actor_ready = 1;
  printf("Actor (PID: %d): Received SIGUSR1 from the Director\n", getpid());
}

int main() {
  struct sigaction sa_director;
  struct sigaction sa_actor;
  pid_t pid;

  pid = fork();

  if (pid > 0) {  // Parent (Director)
    sa_director.sa_handler = Director_handler;
    sigemptyset(&sa_director.sa_mask);
    sa_director.sa_flags = 0;
    sigaction(SIGUSR2, &sa_director, NULL);

    printf("Director (PID: %d): The play is about to begin\n", getpid());
    sleep(1);

    printf("Director (PID: %d): I'll send SIGUSR1 to the actor to start\n",
           getpid());
    kill(pid, SIGUSR1);  // Send SIGUSR1 to child

    pause();  // Wait for SIGUSR2

    sleep(1);
    printf("Director (PID: %d): okay, we're ending the play\n", getpid());

    // Wait for child to avoid zombie
    waitpid(pid, NULL, 0);
  } else if (pid == 0) {  // Child (Actor)
    sa_actor.sa_handler = Actor_handler;
    sigemptyset(&sa_actor.sa_mask);
    sa_actor.sa_flags = 0;
    sigaction(SIGUSR1, &sa_actor, NULL);

    printf("Actor (PID: %d): I'm ready.\n", getpid());
    pause();  // Wait for SIGUSR1

    // After receiving SIGUSR1
    sleep(1);
    printf(
        "Actor (PID: %d): To be, or not to be, that is the question: Whether "
        "'tis nobler in the mind to suffer the slings and arrows of outrageous "
        "fortune, or to take arms against a sea of troubles, and by opposing "
        "end them.\n",
        getpid());
    printf("Actor (PID: %d): I'm finished, sending SIGUSR2 to the director\n",
           getpid());
    kill(getppid(), SIGUSR2);  // Send SIGUSR2 to parent
  }

  return 0;
}
