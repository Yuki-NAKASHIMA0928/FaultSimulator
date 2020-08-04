#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 10
#define RANGE 100

/* dequeue 処理 */
void dequeue(int queue[], int end){
    int i;

    printf("dequeue %d.\n", queue[end]);

}

/* enqueue 処理 */
void enqueue(int queue[], int data, int end){
    int work[SIZE], i;

    /* キューを作業用配列にコピー */
    for(i = 0; i < SIZE; i++){
        work[i] = queue[i];
    }

    printf("enqueue %d.\n", data);
    queue[0] = data;

    /* キュー内部をずらしていく */
    for (i = 0; i < SIZE-1; i++)
    {
        queue[i+1] = work[i];
    }

}

int main(void){

    int select, i;
    int queue[SIZE], data;
    int end;

    /* 乱数シードの初期化 */
    srand((unsigned)time(NULL));

    /* キューの初期化 */
    for(i = 0; i < SIZE/2; i++){
        queue[i] = rand() % (RANGE+1);
    }

    /* 終端の初期化 */
    end = (SIZE/2)-1;

    /* enqueue か dequeue の操作を求める */
    /* 終了条件は "キューが空の状態でdequeueした時" か "キューが満タンの状態でenqueueした時" */
    while(1){
        /* キュー内部の表示 */
        printf("queue:");
        for(i = 0; i <= end; i++){
            printf("%d ", queue[i]);
        }
        if(end <= -1){
            printf("empty");
        }
        printf("\n");

        do{ 
            printf("dequeue(0) or enqueue(1)?\n");
            scanf("%d", &select);
        }while(select != 0 && select != 1);

        /* dequeueを選択 */
        if(select == 0){
            if(end <= -1){               // キューが空の状態でdequeue
                printf("Queue is empty.\n");
                break;
            }
            else{
                dequeue(queue, end);    // dequeue操作
                end--;
            }
        }
        /* enqueueを選択 */
        else if(select == 1){
            if(end >= SIZE-1){                // キューが満タンの状態でenqueue
                printf("Queue is full.\n");
                break;
            }
            else{
                /* enqueueする数字を入力 */
                do{
                    printf("Enter a number(0~100)\n");
                    scanf("%d", &data);
                }while(data < 0 || data > 100);
                enqueue(queue, data, end);   // enqueue操作
                end++;
            }
        }
        printf("\n");
    }

    return 0;
}