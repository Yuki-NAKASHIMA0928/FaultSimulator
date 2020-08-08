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

/* データを昇順に並び替える */
void MergeSort(int data[], int left, int right, int work[]){
    int i, j, k, mid;

    if(left < right){
        mid = (left + right) / 2;   // 中心を求める
    
        MergeSort(data, left, mid, work);     // 左半分を再帰処理
        MergeSort(data, mid+1, right, work);  // 右半分を再帰処理

        /* 作業用配列にデータをコピー */
        for(i = left; i <= right; i++){
            work[i] = data[i];
        }

        /* マージする配列の左端をそれぞれインデックスとして保存 */
        i = left;
        j = mid+1;

        /* 以下、結合操作 */
        for(k = left; k <= right; k++){
            if(i > mid){                    // 左の配列が全て結合済みの場合
                data[k] = work[j];
                j++;
            }
            else if(j > right){             // 右の配列が全て結合済みの場合
                data[k] = work[i];
                i++;
            }
            else{
                if(work[i] < work[j]){
                    data[k] = work[i];
                    i++;
                }
                else{
                    data[k] = work[j];
                    j++;
                }
            }
        }
    }
}

/* 渡された値を決められた出力線に渡す */
void PassOutLine(SigLine* sigLine, int index, int pointer[], int value, int count[]){
    sigLine[index].value[0] = value;

    int line = sigLine[index].outputLine - 1;       // 配列は"0"始まり、リストは"1"始まりなので-1する必要がある
    if(sigLine[index].outputNum == 1){  // 指定された出力線に値を受け渡す
        sigLine[line].value[count[line]] = value;
        //printf("%2d\n", sigLine[index].outputLine);   // 確認用
        //printf("%2d->%2d, %2d\n", index+1, line+1, sigLine[line].value[count[line]]);
        count[line]++;
    }
    else if(sigLine[index].outputNum >= 2){  // 指定された出力線ポインタに値を受け渡す
        for(int k = 0; k < sigLine[index].outputNum; k++){
            int p = pointer[line+k] - 1;
            sigLine[p].value[count[p]] = value;
            //printf("%2d\n", p+1);     // 確認用
            //printf("%2d->%2d, %2d\n", index+1, p+1, sigLine[p].value[count[p]]);
            count[p]++;
        }
    }
}

/* 渡されたリスト情報に従って論理計算を行い、入力線の値を決定する関数 */
void LogicCalc(SigLine* sigLine, int i, int j, int pointer[], int count[]){
    int flag;

    /* type:入力線(0) */
    if(sigLine[j].type == 0){        
        //sigLine[j].value[count[j]] = test[i][j];
        //printf("sigLine[%d].value = %d\n", j, sigLine[j].value[count[j]]);
        PassOutLine(sigLine, j, pointer, sigLine[j].value[0], count);     // 出力線に値を渡す
        count[j]++;        
    }

    /* type:OR(1) */
    else if(sigLine[j].type == 1){
        flag = 0;

        //printf("OR:");
        for(int k = 0; k < sigLine[j].inputNum; k++){
            //printf("%2d ", sigLine[j].value[k]);
            if(sigLine[j].value[k] == 1){       // OR論理では、1つでも"1"があったら出力は"1"
                flag = 1;                       // "1"を見つけたらフラグを立てる
                break;
            }
        }
        //printf("\n");
        
        PassOutLine(sigLine, j, pointer, flag, count);     // 出力線に値を渡す
    }

    /* type:AND(2) */
    else if(sigLine[j].type == 2){
        flag = 1;

        //printf("AND:");
        for(int k = 0; k < sigLine[j].inputNum; k++){
            //printf("%2d ", sigLine[j].value[k]);
            if(sigLine[j].value[k] == 0){       // AND論理では、1つでも"0"があったら出力は"0"
                flag = 0;                       // "0"を見つけたらフラグを降ろす
                break;
            }
        }
        //printf("\n");
        
        PassOutLine(sigLine, j, pointer, flag, count);     // 出力線に値を渡す
    }

    /* type:分岐の枝(3) */
    else if(sigLine[j].type == 3){
        int value = sigLine[j].value[0];
        PassOutLine(sigLine, j, pointer, value, count);   // 出力線に値を渡す
    }

    /* type:外部出力線(4) */
    else if(sigLine[j].type == 4){
        //printf("%d ", sigLine[j].value[0]);
        sigLine[sigLine[j].inputLine-1].value[0] = sigLine[j].value[0];
    }

    /* type:EXOR(5) */
    else if(sigLine[j].type == 5){
        flag = 1;

        //printf("EXOR:");
        if(sigLine[j].value[0] == sigLine[j].value[1]){         // EXOR論理では、2つの値が同値なら出力は"0"
            //printf("%2d ", sigLine[j].value[k]);
            flag = 0;                                           // 同値ならフラグを降ろす
        }
        //printf("\n");

        PassOutLine(sigLine, j, pointer, flag, count);
    }
    /* type:FF(6) */
    else if(sigLine[j].type == 6){

    }

    /* type:NAND(-1) */
    else if(sigLine[j].type == -1){
        flag = 0;

        //printf("NAND:");
        for(int k = 0; k < sigLine[j].inputNum; k++){
            //printf("%2d ", sigLine[j].value[k]);
            if(sigLine[j].value[k] == 0){       // NAND論理では、1つでも"0"があったら出力は"1"
                flag = 1;                       // "0"を見つけたらフラグを立てる
                break;
            }
        }
        //printf("\n");
        
        PassOutLine(sigLine, j, pointer, flag, count);     // 出力線に値を渡す
    }

    /* type:NOR(-2) */
    else if(sigLine[j].type == -2){
        flag = 1;

        //printf("NOR:");
        for(int k = 0; k < sigLine[j].inputNum; k++){
            //printf("%2d ", sigLine[j].value[k]);
            if(sigLine[j].value[k] == 1){           // NOR論理では、1つでも"1"があったら出力は"0"
                flag = 0;                           // "1"を見つけたらフラグを降ろす
                break;
            }
        }
        //printf("\n");
        
        PassOutLine(sigLine, j, pointer, flag, count);     // 出力線に値を渡す
    }

    /* type:NOT(-3) */
    else if(sigLine[j].type == -3){
        int line = sigLine[j].inputLine - 1;
        if(sigLine[line].value[0] == 0){
            PassOutLine(sigLine, j, pointer, 1, count);     // 出力線に値を渡す
        }
        else if(sigLine[line].value[0] == 1){
            PassOutLine(sigLine, j, pointer, 0, count);     // 出力線に値を渡す
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
    char table[64], pattern[64], answer[64];

    int sigNum, pNum, testNum, input, output, *inList, *outList;   // 信号線数、ポインタ数、テストパターン数、入力線数、外部出力線数、外部出力線リスト
    SigLine *sigLine;   // リスト1
    int *pointer;       // リスト2
    int **test;         // テストパターン
    int **exp, equal = 1;       // 正しいテスト結果とそれを判別するためのフラグ

    int *queue, *work, end = -1;  // キューと作業用配列とキューのポインタ
    int enqueued;       // 入力線の値を決められない時に真を取るフラグ


    if(argc < 2){
        printf("Not enough arguments.\n");
        return 1;
    }

    /* tableファイル、patternファイル、answerファイルの文字列を準備 */
    strcpy(table, "iscas85/Table/");
    strcat(table, argv[1]);
    strcat(table, ".tbl");

    strcpy(pattern, "iscas85/Pattern/");
    strcat(pattern, argv[1]);
    strcat(pattern, ".pat");

    strcpy(answer, "iscas85/Testcase/");
    strcat(answer, argv[1]);
    strcat(answer, ".ans");

    /* 回路テーブルのファイルをオープン */
    fp = fopen(table,"r");
    if(fp == NULL){
        printf("Cannot open '%s.tbl'.\n", argv[1]);
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
        fscanf(fp, "%d", &sigLine[i].type);         //printf("%2d ", sigLine[i].type);
        fscanf(fp, "%d", &sigLine[i].inputNum);     //printf("%2d ", sigLine[i].inputNum);
        fscanf(fp, "%d", &sigLine[i].inputLine);    //printf("%2d ", sigLine[i].inputLine);
        fscanf(fp, "%d", &sigLine[i].outputNum);    //printf("%2d ", sigLine[i].outputNum);
        fscanf(fp, "%d", &sigLine[i].outputLine);   //printf("%2d \n", sigLine[i].outputLine);
    }

    /* ポインタリストの数を取得 */
    fscanf(fp, "%d", &pNum);

    /* pNum個のpointerを確保 */
    pointer = (int*)malloc(pNum * sizeof(int));

    /* 読み込み処理 */
    for(int i = 0; i < pNum; i++){
        fscanf(fp, "%d", &pointer[i]);              //printf("%2d\n", pointer[i]);
    }

    /* 外部入力線数を取得 */
    fscanf(fp, "%d", &input);

    /* input個のinListを取得 */
    inList = (int*)malloc(input * sizeof(int));

    /* 読み込み処理 */
    for(int i = 0; i < input; i++){
        fscanf(fp, "%d", &inList[i]);
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

    /* 出力線リストを昇順に並べ替える */
    int *workM;
    workM = (int*)malloc(output * sizeof(int));
    MergeSort(outList, 0, output-1, workM);

    /* テストパターンのファイルをオープン */
    fp = fopen(pattern,"r");
    if(fp == NULL){
        printf("Cannot open '%s.pat'.\n", argv[1]);
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
            fscanf(fp, "%d", &test[i][j]);              //printf("%2d ", test[i][j]);
        }
        //printf("\n");
    }

    fclose(fp);

    /* テスト結果のファイルをオープン */
    fp = fopen(answer,"r");
    if(fp == NULL){
        printf("Cannot open '%s.ans'.\n", argv[1]);
        exit(1);
    }

    /* testNum * output個のexpを確保 */
    exp = malloc(testNum * sizeof(int*));
    for(int i = 0; i < testNum; i++){
        exp[i] = malloc(output * sizeof(int));
    }

    /* 読み込み処理 */
    for(int i = 0; i < testNum; i++){
        for(int j = 0; j < output; j++){
            fscanf(fp, "%d", &exp[i][j]);
        }
    }

    fclose(fp);       

    /* 信号線に、その信号線の入力線数だけ値を格納できるようにする */
    for(int i = 0; i < sigNum; i++){
        sigLine[i].value = (int*)malloc(sigLine[i].inputNum * sizeof(int));
    }

    /* 信号線の値の初期化 */
    for(int i = 0; i < sigNum; i++){
        for(int j = 0; j < sigLine[i].inputNum; j++){
            sigLine[i].value[j] = -1;
        }
    }

    /* 信号線に複数の値を格納するためのカウント */
    /* 信号線にいくつ値が格納されているかを示す */
    int *count;
    count = (int*)malloc(sigNum * sizeof(int));
    for(int i = 0; i < sigNum; i++){
        count[i] = 0;
    }

    /* 以下、論理回路のテスト処理 */
    for(int i = 0; i < testNum; i++){   // 全テストパターンの繰り返し(i=テスト番号-1)
        //printf("Test Pattern %2d\n", i+1);
        /* カウントの初期化 */
        for(int j = 0; j < sigNum; j++){
            count[j] = 0;
        }
        /* 信号線の値の初期化 */
        for(int j = 0; j <sigNum; j++){
            for(int k = 0; k < sigLine[j].inputNum; k++){
                sigLine[j].value[k] = -1;
            }
        }

        /* 外部入力線の値を決めていく */
        for(int j = 0; j < input; j++){
            sigLine[inList[j]-1].value[0] = test[i][j];
            //printf("sigLine[%d].value = %d\n", j, sigLine[j].value[count[j]]);
            //PassOutLine(sigLine, inList[j]-1, pointer, sigLine[j].value[count[j]], count);     // 出力線に値を渡す
            //count[j]++;
        }      

        for(int j = 0; j < sigNum; j++){    // 全信号線の繰り返し(j=信号線の番号-1)
            /* 計算されていない入力線が存在するかの確認 */
            enqueued = 0;
            if(sigLine[j].type != 0){               
                if(count[j] < sigLine[j].inputNum){
                   enqueued = 1;
                }              
            }
                        
            if(enqueued == 0){
                LogicCalc(sigLine, i, j, pointer, count);   // 信号線の値を決定する
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
            printf("%4d:end = %4d\n", j+1, end);
            */          
        }

        int finish = 0;     // 無限ループ判定のための変数
        int num;    // デキューした値を格納する変数
        
        /* キューの中身を見ていく */
        while(end >= 0)
        {   
            num = Dequeue(queue, work, end);     // 信号線番号を1つ取り出す
            end--;
            
            if(count[num] < sigLine[num].inputNum){
                end++;
                Enqueue(queue, end, num);
                //printf("end = %4d\n", end);
            }
            else{
                LogicCalc(sigLine, i, num, pointer, count);   // 信号線の値を決定する
                //printf("end = %4d\n", end);
            }
            
            /* 無限ループの確認(1万回で終了) */
            finish++;
            if(finish > 10000){
                printf("infinite loop error.\n");
                return 1;
            }
            
        }

        for(int j = 0; j < output; j++){
            //printf("%d ", sigLine[outList[j]-1].value[0]);
            if(sigLine[outList[j]-1].value[0] != exp[i][j]){    //テスト結果と期待値が同値であるかの確認
                equal = 0;
            }
        }        
        //printf("\n");
    }

    /* テスト結果と期待値が同値であるかの確認 */
    if(equal == 0){
        printf("Result does NOT agree with answer.");
    }
    else{
        printf("Result agrees with answer.");        
    }

    printf("\n");

    return 0;
}