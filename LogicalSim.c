#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* リスト1:信号線の情報のリスト */
typedef struct sigLineTable
{
    int type;       // タイプ 0:外部入力線 １:OR 2:AND 3:分岐の枝 4:外部出力線 5:EXOR 6:FF -1:NAND -2:NOR -3:NOT 9:unused
    int inputNum;   // 入力数
    int inputLine;  // ポインタまたは入力線
    int outputNum;  // 出力数
    int outputLine; // ポインタまたは出力線
    int *value;
}SigLine;

/* 渡された値を決められた出力線に渡す */
void PassOutLine(SigLine* sigLine, int index, int pointer[], int value, int cnt[]){
    int line = sigLine[index].outputLine - 1;       // 配列は"0"始まり、リストは"1"始まりなので-1する必要がある
    if(sigLine[index].outputNum == 1){  // 指定された出力線に値を受け渡す
        sigLine[line].value[cnt[line]] = value;
        //printf("%2d\n", sigLine[index].outputLine);   // 確認用
        //printf("%2d, %2d\n", line+1, sigLine[line].value[cnt[line]]);
        cnt[line]++;
    }
    else if(sigLine[index].outputNum >= 2){  // 指定された出力線ポインタに値を受け渡す
        for(int k = 0; k < sigLine[index].outputNum; k++){
            int p = pointer[line+k] - 1;
            sigLine[p].value[cnt[p]] = value;
            //printf("%2d\n", p+1);     // 確認用
            //printf("%2d, %2d\n", p+1, sigLine[p].value[cnt[p]]);
            cnt[p]++;
        }
    }
}

/* 渡されたリスト情報に従って論理計算を行い、入力線の値を決定する関数 */
void LogicCalc(SigLine* sigLine, int i, int j, int pointer[], int cnt[], int **test){
    /* type:入力線(0) */
    if(sigLine[j].type == 0){
        sigLine[j].value[cnt[j]] = test[i][j];
        //printf("sigLine[%d].value = %d\n", j, sigLine[j].value[cnt[j]]);
        PassOutLine(sigLine, j, pointer, sigLine[j].value[cnt[j]], cnt);     // 出力線に値を渡す
        cnt[j]++;
    }

    /* type:OR(1) */
    else if(sigLine[j].type == 1){
        int flag = 0;

        //printf("OR:");
        for(int k = 0; k < sigLine[j].inputNum; k++){
            //printf("%2d ", sigLine[j].value[k]);
            if(sigLine[j].value[k] == 1){       // OR論理では、1つでも"1"があったら出力は"1"
                flag = 1;                       // "1"を見つけたらフラグを立てる
                break;
            }
        }
        //printf("\n");
        
        PassOutLine(sigLine, j, pointer, flag, cnt);     // 出力線に値を渡す
    }

    /* type:AND(2) */
    else if(sigLine[j].type == 2){
        int flag = 1;

        //printf("AND:");
        for(int k = 0; k < sigLine[j].inputNum; k++){
            //printf("%2d ", sigLine[j].value[k]);
            if(sigLine[j].value[k] == 0){       // AND論理では、1つでも"0"があったら出力は"0"
                flag = 0;                       // "0"を見つけたらフラグを降ろす
                break;
            }
        }
        //printf("\n");
        
        PassOutLine(sigLine, j, pointer, flag, cnt);     // 出力線に値を渡す
    }

    /* type:分岐の枝(3) */
    else if(sigLine[j].type == 3){
        int value = sigLine[j].value[0];
        PassOutLine(sigLine, j, pointer, value, cnt);   // 出力線に値を渡す
    }

    /* type:外部出力線(4) */
    else if(sigLine[j].type == 4){
        //printf("%d ", sigLine[j].value[0]);
        sigLine[sigLine[j].inputLine-1].value[0] = sigLine[j].value[0];
    }

    /* type:EXOR(5) */
    else if(sigLine[j].type == 5){
        int flag = 1;

        //printf("EXOR:");
        if(sigLine[j].value[0] == sigLine[j].value[1]){         // EXOR論理では、2つの値が同値なら出力は"0"
            //printf("%2d ", sigLine[j].value[k]);
            flag = 0;                                           // 同値ならフラグを降ろす
        }
        //printf("\n");

        PassOutLine(sigLine, j, pointer, flag, cnt);
    }
    /* type:FF(6) */
    else if(sigLine[j].type == 6){

    }

    /* type:NAND(-1) */
    else if(sigLine[j].type == -1){
        int flag = 0;

        //printf("NAND:");
        for(int k = 0; k < sigLine[j].inputNum; k++){
            //printf("%2d ", sigLine[j].value[k]);
            if(sigLine[j].value[k] == 0){       // NAND論理では、1つでも"0"があったら出力は"1"
                flag = 1;                       // "0"を見つけたらフラグを立てる
                break;
            }
        }
        //printf("\n");
        
        PassOutLine(sigLine, j, pointer, flag, cnt);     // 出力線に値を渡す
    }

    /* type:NOR(-2) */
    else if(sigLine[j].type == -2){
        int flag = 1;

        //printf("NOR:");
        for(int k = 0; k < sigLine[j].inputNum; k++){
            //printf("%2d ", sigLine[j].value[k]);
            if(sigLine[j].value[k] == 1){           // NOR論理では、1つでも"1"があったら出力は"0"
                flag = 0;                           // "1"を見つけたらフラグを降ろす
                break;
            }
        }
        //printf("\n");
        
        PassOutLine(sigLine, j, pointer, flag, cnt);     // 出力線に値を渡す
    }

    /* type:NOT(-3) */
    else if(sigLine[j].type == -3){
        int line = sigLine[j].inputLine - 1;
        if(sigLine[line].value[0] == 0){
            PassOutLine(sigLine, j, pointer, 1, cnt);     // 出力線に値を渡す
        }
        else if(sigLine[line].value[0] == 1){
            PassOutLine(sigLine, j, pointer, 0, cnt);     // 出力線に値を渡す
        }
    }

    /* type:unused(9) */
    else if(sigLine[j].type == 9){
        /* do nothing */
    }
}

/* デキュー */
int Dequeue(int queue[], int work[], int end){
    int num;

    /* キューの中身を作業用配列にコピー */
    for(int i = 0; i <= end; i++){
        work[i] = queue[i];
    }

    /* 要素をずらしていく */
    num = queue[0];
    for(int i = 0; i < end; i++){
        queue[i] = work[i+1];
    }    

    return num;

}

/* エンキュー */
void Enqueue(int queue[], int end, int num){
    queue[end] = num;
}

int main(int argc, char *argv[]){
    FILE *fp;
    int sigNum, pNum, testNum, input, output, *outList;   // 信号線数、ポインタ数、テストパターン数、入力線数、外部出力線数、外部出力線リスト
    SigLine *sigLine;   // リスト1
    int *pointer;       // リスト2
    int **test;         // テストパターン
    int *queue, *work, end = -1;  // キューと作業用配列とキューのポインタ
    int enqueued;       // 入力線の値を決められない時に真を取るフラグ
    char table[64], pattern[64];

    if(argc < 3){
        printf("Not enough arguments.\n");
        return 1;
    }

    /* tableファイルと、patternファイルの文字列を準備 */
    strcpy(table, "iscas85/Table/");
    strcpy(pattern, "iscas85/Pattern/");
    strcat(table, argv[1]);
    strcat(pattern, argv[2]);

    /* 回路テーブルのファイルをオープン */
    fp = fopen(table,"r");
    if(fp == NULL){
        printf("Cannot open %s.\n", argv[1]);
        exit(1);
    }

    printf("filename:%s\n", argv[1]);

    /* 信号線の数を取得 */
    fscanf(fp, "%d", &sigNum);
    //printf("number of signal lines:%d\n", sigNum);

    /* sigNum個のSigLine、queue、workを確保 */
    sigLine = (SigLine*)malloc(sigNum * sizeof(SigLine));
    queue = (int*)malloc(sigNum * sizeof(int));
    work = (int*)malloc(sigNum * sizeof(int));

    /* 読み込み処理 */
    for(int i = 0; i < sigNum; i++){
        fscanf(fp, "%d", &sigLine[i].type);
        fscanf(fp, "%d", &sigLine[i].inputNum);
        fscanf(fp, "%d", &sigLine[i].inputLine);
        fscanf(fp, "%d", &sigLine[i].outputNum);
        fscanf(fp, "%d", &sigLine[i].outputLine);
    }

    /* 読み取り確認用 */
    /*
    for(int i = 0; i < sigNum; i++){
        printf("%2d ", sigLine[i].type);
        printf("%2d ", sigLine[i].inputNum);
        printf("%2d ", sigLine[i].inputLine);
        printf("%2d ", sigLine[i].outputNum);
        printf("%2d \n", sigLine[i].outputLine);
    }
    */

    /* ポインタリストの数を取得 */
    fscanf(fp, "%d", &pNum);

    /* pNum個のpointerを確保 */
    pointer = (int*)malloc(pNum * sizeof(int));

    /* 読み込み処理 */
    for(int i = 0; i < pNum; i++){
        fscanf(fp, "%d", &pointer[i]);
    }

    /* 読み取り確認用 */
    /*
    for(int i = 0; i < pNum; i++){
        printf("%2d\n", pointer[i]);
    }
    */

    /* 外部入力線数を取得 */
    fscanf(fp, "%d", &input);

    /* 外部出力線数まで読み飛ばし */
    int skip;
    for(int i = 0; i < input; i++){
        fscanf(fp, "%d", &skip);
    }

    /* 外部出力線数を取得 */
    fscanf(fp, "%d", &output);

    /* output個のoutListを確保 */
    outList = (int*)malloc(output * sizeof(int));

    /* 読み込み処理 */
    for(int i = 0; i < output; i++){
        fscanf(fp, "%d", &outList[i]);
    }

    fclose(fp);

    /* テストパターンのファイルをオープン */
    fp = fopen(pattern,"r");
    if(fp == NULL){
        printf("Cannot open %s.\n", argv[2]);
        exit(1);
    }

    /* テストパターン数を取得 */
    fscanf(fp, "%d", &testNum);
    //printf("number of test patterns:%d\n", testNum);

    /* testNum * input個のtestを確保 */
    test = malloc(testNum * sizeof(int*));
    for(int i = 0; i < testNum; i++){
        test[i] = malloc(input * sizeof(int));
    }

    /* 読み込み処理 */
    for(int i = 0; i < testNum; i++){
        for(int j = 0; j < input; j++){
            fscanf(fp, "%d", &test[i][j]);
        }
    }

    fclose(fp);

    /* 読み取り確認用 */
    /*
    for(int i = 0; i < testNum; i++){
        for(int j = 0; j < input; j++){
            printf("%2d ", test[i][j]);
        }
        printf("\n");
    }
    */

    for(int i = 0; i < sigNum; i++){
        sigLine[i].value = (int*)malloc(sigLine[i].inputNum * sizeof(int));
    }

    for(int i = 0; i < sigNum; i++){
        for(int j = 0; j < sigLine[i].inputNum; j++){
            sigLine[i].value[j] = -1;
        }
    }

    /* 信号線に複数の値を格納するためのカウント */
    /* 信号線にいくつ値が格納されているかを示す */
    int *cnt;
    cnt = (int*)malloc(sigNum * sizeof(int));
    for(int i = 0; i < sigNum; i++){
        cnt[i] = 0;
    }

    /* 以下、論理回路のテスト処理 */
    for(int i = 0; i < testNum; i++){   // 全テストパターンの繰り返し(i=テスト番号-1)
        //printf("Test Pattern %2d\n", i+1);
        /* カウントの初期化 */
        for(int j = 0; j < sigNum; j++){
            cnt[j] = 0;
        }
        /* 入力線の値の初期化 */
        for(int j = 0; j <sigNum; j++){
            for(int k = 0; k < sigLine[j].inputNum; k++){
                sigLine[j].value[k] = -1;
            }
        }

        for(int j = 0; j < sigNum; j++){    // 全信号線の繰り返し(j=信号線の番号-1)
            /* 計算されていない入力線が存在するかの確認 */
            enqueued = 0;
            if(sigLine[j].type != 0){               
                if(cnt[j] < sigLine[j].inputNum){
                   enqueued = 1;
                }              
            }
            
            
            if(enqueued == 0){
                LogicCalc(sigLine, i, j, pointer, cnt, test);   // 信号線の値を決定する
            }
            else{
                end++;
                Enqueue(queue, end, j);
            }
            /*
            for(int k = 0; k <= end; k++){
                printf("%2d ", queue[k]);
            }
            printf("\n");
            printf("end=%2d\n", end);
            */
        }

        int finish = 0;     // 無限ループ回避のための変数

        /* キューの中身を見ていく */
        while(end >= 0)
        {   
            int num;    // デキューした値を格納する変数
            num = Dequeue(queue, work, end);     // 信号線番号を1つ取り出す
            end--;

            /*
            int inputline = sigLine[num].inputLine - 1;
            if(sigLine[num].inputNum == 1){  
                if(sigLine[inputline].value[0] == -1){
                    end++;
                    Enqueue(queue, end, num);
                }
                else{
                    LogicCalc(sigLine, i, num, pointer, cnt, test);   // 信号線の値を決定する
                }
            }
            else if(sigLine[num].inputNum >= 2){  
                for(int k = 0; k < sigLine[num].inputNum; k++){
                    int p = pointer[inputline+k] - 1;
                    if(sigLine[p].value[0] == -1){
                        end++;
                        Enqueue(queue, end, num);                     
                        break;
                    }
                    else if(sigLine[p].value[0] != -1 && k == sigLine[num].inputNum - 1){
                        LogicCalc(sigLine, i, num, pointer, cnt, test);   // 信号線の値を決定する
                    }
                }
            }
            */
            
            if(cnt[num] < sigLine[num].inputNum){
                end++;
                Enqueue(queue, end, num);
            }
            else{
                LogicCalc(sigLine, i, num, pointer, cnt, test);   // 信号線の値を決定する
            }
            

            finish++;
            /*
            if(finish > 100000){
                printf("infinite loop error.\n");
                return 1;
            }
            */
        }

        for(int j = 0; j < output; j++){
            printf("%d ", sigLine[outList[j]-1].value[0]);
        }
        
        printf("\n");
    }

    printf("\n");

    return 0;
}