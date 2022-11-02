#include <math.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#define KEY 0xFF // 11111111 - for 8-bit value
const float precision = 0.000000000000001;
// structure of linear equation (ax+by+c=0), x - first coefficient, y - second,
// c - third
struct Line {
  float x;
  float y;
  float c;
};
// generates random float number
float GetRandom() {
  return (((float)rand()) / RAND_MAX * 200 - 100); //[-100; 100]
}
// sees if a is equal to b, if equal - true, else false
bool Eq(float a, float b) {
  if (fabs(a - b) > precision)
    return false;
  else
    return true;
}
// finds common dots for two lines
void FindCommonDot(struct Line line1, struct Line line2) {
  if (!Eq(line2.x, 0) && !Eq(line2.y, 0)) {
    if (Eq(line1.x / line2.x, line1.y / line2.y) &&
        Eq(line1.y / line2.y, line1.c / line2.c) &&
        Eq(line1.x / line2.x, line1.c / line2.c)) {
      printf("Любая точка будет точкой пересечения!\n");
      return;
    }
  }
  float X, Y;
  if (!Eq(line1.x * line2.y, line2.x * line1.y)) {
    X = (line1.y * line2.c - line2.y * line1.c) /
        (line1.x * line2.y - line1.y * line2.x);
    Y = (line1.c * line2.x - line1.x * line2.c) /
        (line1.x * line2.y - line1.y * line2.x);
    printf("(%f;%f)\n", X, Y);
    return;
  } else {
    printf("Нет общих точек!\n");
    return;
  } // line 50
}

// structure for task
struct Task {
  long mtype;
  struct Line line1, line2;
};

// func for CJT thread
void *CreateJob(void *queue_id) {
  for (;;) {
    int QM = *((int *)queue_id);
    struct Task t;
    t.mtype = 1;
    t.line1.x = GetRandom();
    t.line1.y = GetRandom();
    t.line1.c = GetRandom();
    t.line2.x = GetRandom();
    t.line2.y = GetRandom();
    t.line2.c = GetRandom();

    if (msgsnd(QM, &t, sizeof(struct Task) - sizeof(long), 0) == -1) {
      perror("Cannot send message\n");
      exit(EXIT_FAILURE);
    }
    sleep(1);
  }
  pthread_exit(0);
}

// func for SJT thread
void *SolveJob(void *queue_id) {
  for (;;) {
    int QM = *((int *)queue_id);
    struct Task t;
    int mtype = 0;
    sleep(1);
    if (msgrcv(QM, &t, sizeof(struct Task) - sizeof(long), mtype, 0) == -1) {
      perror("Cannot recive message\n");
      exit(EXIT_FAILURE);
    }
    printf("Уравнения:\n1)%fx%c%fy%c%f\n2)%fx%c%fy%c%f\n", t.line1.x,
           (t.line1.y > 0) ? '+' : '\0', t.line1.y,
           (t.line1.c > 0) ? '+' : '\0', t.line1.c, t.line2.x,
           (t.line2.y > 0) ? '+' : '\0', t.line2.y,
           (t.line2.c > 0) ? '+' : '\0', t.line2.c);
    FindCommonDot(t.line1, t.line2);
    struct msqid_ds info;
    if (msgctl(QM, IPC_STAT, &info) == -1) {
      perror("Cannot get info about message queue\n");
      exit(EXIT_FAILURE);
    }

    if (info.msg_qnum == 0) { // if number of messages in queue is zero
      if (msgctl(QM, IPC_RMID, NULL) == -1) {
        perror("Cannot remove the message queue\n");
        exit(EXIT_FAILURE);
      }
      printf("Message queue has been removed\n");
    }
  }
  pthread_exit(0);
}

/*-----------------main function--------------------*/
int main(int argc, char *const argv[]) {
  srand(time(NULL));

  key_t queue_key = ftok("lines", KEY);
  if (queue_key == -1) {
    perror("Can\'t create key\n");
    exit(EXIT_FAILURE);
  }
  int queue_id =
      msgget(queue_key,
             IPC_CREAT | 0660); // 0660 - octal number, represents 110 110 000,
                                // rwx for owner, owner's group, rest of ppl
  if (queue_id == -1) {
    perror("Cannot retrive queue\n");
    exit(EXIT_FAILURE);
  }

  pthread_t CJT_id,
      SJT_id; // id of thread (CJT - create job thread SJT - solve job thread)
  pthread_attr_t CJT_attr, SJT_attr; // attributes of thread

  pthread_attr_init(&CJT_attr); // set default thread atribute vals
  pthread_attr_init(&SJT_attr);

  pthread_create(&CJT_id, &CJT_attr, CreateJob, &queue_id);
  pthread_create(&SJT_id, &SJT_attr, SolveJob, &queue_id);

  pthread_join(CJT_id, NULL);
  pthread_join(SJT_id, NULL);

  return 0;
}