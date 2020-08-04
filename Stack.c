#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 10
#define RANGE 100

/* pop 処理 */
void pop(int stack[]){
    int work[SIZE], i;
    
    /* スタックを作業用配列にコピー */
    for(i = 0; i < SIZE; i++){
        work[i] = stack[i];
    }

    printf("pop %d.\n", stack[0]);

    /* スタック内部をずらしていく */
    for (i = 0; i < SIZE-1; i++)
    {
        stack[i] = work[i+1];
    }
    
}

/* push 処理 */
void push(int stack[], int data){
    int work[SIZE], i;

    /* スタックを作業用配列にコピー */
    for(i = 0; i < SIZE; i++){
        work[i] = stack[i];
    }

    printf("push %d.\n", data);

    /* スタック内部をずらしていく */
    stack[0] = data;
    for (i = 0; i < SIZE-1; i++)
    {
        stack[i+1] = work[i];
    }
}

int main(void){

    int select, i;
    int stack[SIZE], data;
    int end;

    /* 乱数シードの初期化 */
    srand((unsigned)time(NULL));

    /* スタックの初期化 */
    for(i = 0; i < SIZE/2; i++){
        stack[i] = rand() % (RANGE+1);
    }

    /* 終端の初期化 */
    end = (SIZE/2)-1;

    /* pop か push の操作を求める */
    /* 終了条件は "スタックが空の状態でpopした時" か "スタックが満タンの状態でpushした時" */
    while(1){
        /* スタック内部の表示 */
        printf("stack:");
        for(i = 0; i <= end; i++){
            printf("%d ", stack[i]);
        }
        if(end <= -1){
            printf("empty");
        }
        printf("\n");

        do{ 
            printf("pop(0) or push(1)?\n");
            scanf("%d", &select);
        }while(select != 0 && select != 1);

        /* popを選択 */
        if(select == 0){
            if(end <= -1){               // スタックが空の状態でpop
                printf("Stack is empty.\n");
                break;
            }
            else{
                pop(stack);    // pop操作
                end--;
            }
        }
        /* pushを選択 */
        else if(select == 1){
            if(end >= SIZE-1){                // スタックが満タンの状態でpush
                printf("Stack is full.\n");
                break;
            }
            else{
                /* pushする数字を入力 */
                do{
                    printf("Enter a number(0~100)\n");
                    scanf("%d", &data);
                }while(data < 0 || data > 100);
                push(stack, data);   // push操作
                end++;
            }
        }
        printf("\n");
    }

    return 0;
}