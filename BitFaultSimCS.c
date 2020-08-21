#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* リスト1:信号線の情報のリスト */
typedef struct sigLineTable
{
    int type;       // タイプ 0:外部入力線 １:OR 2:AND 3:分岐の枝 4:外部出力線 5:EXOR 6:FF -1:NAND -2:NOR -3:NOT 9:unused
    int inputNum;   // 入力数
    int inputLine;  // ポインタまたは入力線
    int outputNum;  // 出力数
    int outputLine; // ポインタまたは出力線
    unsigned int *value;
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
void PassOutLine(SigLine* sigLine, int index, int pointer[], unsigned int value, int count[]){
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
void LogicCalc(SigLine* sigLine, int j, int pointer[], int count[]){
    unsigned int flag;

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
        unsigned int value = sigLine[j].value[0];
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

/* 渡されたリスト情報に従って論理計算を行い、更にマスクを通して入力線の値を決定する関数 */
void BitLogicCalc(SigLine* sigLine, int j, int pointer[], int count[], unsigned int mask0[], unsigned int mask1[]){
    unsigned int flag;

    /* type:入力線(0) */
    if(sigLine[j].type == 0){        
        sigLine[j].value[0] |= mask0[j];
        sigLine[j].value[0] &= mask1[j];
        PassOutLine(sigLine, j, pointer, sigLine[j].value[0], count);     // 出力線に値を渡す
        count[j]++;        
    }

    /* type:OR(1) */
    else if(sigLine[j].type == 1){
        flag = flag ^ flag;

        //printf("OR:");
        for(int k = 0; k < sigLine[j].inputNum; k++){
            //printf("%2d ", sigLine[j].value[k]);
            flag = flag | sigLine[j].value[k];
        }
        //printf("\n");

        flag |= mask0[j];
        flag &= mask1[j];
        PassOutLine(sigLine, j, pointer, flag, count);     // 出力線に値を渡す
    }

    /* type:AND(2) */
    else if(sigLine[j].type == 2){
        flag = flag ^ flag;
        flag = ~flag;

        //printf("AND:");
        for(int k = 0; k < sigLine[j].inputNum; k++){
            //printf("%2d ", sigLine[j].value[k]);
            flag = flag & sigLine[j].value[k];           
        }
        //printf("\n");
        
        flag |= mask0[j];
        flag &= mask1[j];
        PassOutLine(sigLine, j, pointer, flag, count);     // 出力線に値を渡す
    }

    /* type:分岐の枝(3) */
    else if(sigLine[j].type == 3){
        unsigned int value = sigLine[j].value[0];
        value |= mask0[j];
        value &= mask1[j];
        PassOutLine(sigLine, j, pointer, value, count);   // 出力線に値を渡す
    }

    /* type:外部出力線(4) */
    else if(sigLine[j].type == 4){
        //printf("%d ", sigLine[j].value[0]);
        sigLine[sigLine[j].inputLine-1].value[0] = sigLine[j].value[0];
    }

    /* type:EXOR(5) */
    else if(sigLine[j].type == 5){
        //flag = 1;
        flag = flag ^ flag;

        //printf("EXOR:");
        for(int k = 0; k < sigLine[j].inputNum; k++){
            flag = flag ^ sigLine[j].value[k];           
        }        

        flag |= mask0[j];
        flag &= mask1[j];
        PassOutLine(sigLine, j, pointer, flag, count);
        //printf("\n");
    }
    /* type:FF(6) */
    else if(sigLine[j].type == 6){

    }

    /* type:NAND(-1) */
    else if(sigLine[j].type == -1){
        //flag = 0;
        flag = flag ^ flag;
        flag = ~flag;

        //printf("NAND:");
        for(int k = 0; k < sigLine[j].inputNum; k++){
            //printf("%2d ", sigLine[j].value[k]);
            flag = flag & sigLine[j].value[k];
        }
        //printf("\n");
        flag = ~flag;
        
        flag |= mask0[j];
        flag &= mask1[j];
        PassOutLine(sigLine, j, pointer, flag, count);     // 出力線に値を渡す
    }

    /* type:NOR(-2) */
    else if(sigLine[j].type == -2){
        //flag = 1;
        flag = flag ^ flag;

        //printf("NOR:");
        for(int k = 0; k < sigLine[j].inputNum; k++){
            //printf("%2d ", sigLine[j].value[k]);
            flag = flag | sigLine[j].value[k];
        }
        //printf("\n");
        flag = ~flag;
 
        flag |= mask0[j];
        flag &= mask1[j];   
        PassOutLine(sigLine, j, pointer, flag, count);     // 出力線に値を渡す
    }

    /* type:NOT(-3) */
    else if(sigLine[j].type == -3){
        int line = sigLine[j].inputLine - 1;

        flag = flag ^ flag;
        flag |= sigLine[line].value[0];
        flag = ~flag;
        flag |= mask0[j];
        flag &= mask1[j];
        PassOutLine(sigLine, j, pointer, flag, count);
    }

    /* type:unused(9) */
    else if(sigLine[j].type == 9){
        /* do nothing */
    }
}

/* デキュー */
int Dequeue(int queue[], int *head, int length){
    unsigned int num;

    num = queue[*head];

    if( *head == length ){
        *head = 0;
    }
    else{
        *head += 1;
    }

    return num;

}

/* エンキュー */
void Enqueue(int queue[], int *tail, unsigned int num, int length){
    queue[*tail] = num;

    if( *tail == length ){
        *tail = 0;
    }
    else{
        *tail += 1;
    }
}

int main(int argc, char *argv[]){
    FILE *fp;
    char table[64], pattern[64], faultset[64];

    int sigNum, pNum, testNum, input, output, *inList, *outList;   // 信号線数、ポインタ数、テストパターン数、入力線数、外部出力線数、外部出力線リスト
    SigLine *sigLine;   // リスト1
    int *pointer;       // リスト2
    unsigned int **test;         // テストパターン
    unsigned int **exp;          // 正しいテスト結果
    int faultNum;        // 故障数と故障リスト
    unsigned int fault[100000][2];
    unsigned int *mask0, *mask1;

    int *queue, head = 0, tail = 0;  // キューとキューのポインタ
    int enqueued;       // 入力線の値を決められない時に真を取るフラグ


    if(argc < 2){
        printf("Not enough arguments.\n");
        return 1;
    }

    /* .tblファイル、.patファイル、f.repファイルの文字列を準備 */
    strcpy(table, "iscas89_cs/Table/");
    strcat(table, argv[1]);
    strcat(table, ".tbl");

    strcpy(pattern, "iscas89_cs/Pattern/");
    strcat(pattern, argv[1]);
    strcat(pattern, ".pat");

    strcpy(faultset, "iscas89_cs/Faultset/");
    strcat(faultset, argv[1]);
    strcat(faultset, "f.rep");

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

    /* sigNum個のSigLine、queue、maskを確保 */
    sigLine = (SigLine*)malloc(sigNum * sizeof(SigLine));
    if(sigLine == NULL){
        printf("Failed malloc sigLine.\n");
        return 1;
    }
    queue = (int*)malloc(sigNum * sizeof(int));
    if(queue == NULL){
        printf("Failed malloc queue.\n");
        return 1;
    }
    mask0 = malloc(sigNum * sizeof(unsigned int));
    if(mask0 == NULL){
        printf("Failed malloc mask0.\n");
        return 1;
    }
    mask1 = malloc(sigNum * sizeof(unsigned int));
    if(mask1 == NULL){
        printf("Failed malloc mask1.\n");
        return 1;
    }

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
    if(pointer == NULL){
        printf("Failed malloc pointer.\n");
        return 1;
    }

    /* 読み込み処理 */
    for(int i = 0; i < pNum; i++){
        fscanf(fp, "%d", &pointer[i]);              //printf("%2d\n", pointer[i]);
    }

    /* 外部入力線数を取得 */
    fscanf(fp, "%d", &input);

    /* input個のinListを取得 */
    inList = (int*)malloc(input * sizeof(int));
    if(inList == NULL){
        printf("Failed malloc inList.\n");
        return 1;
    }

    /* 読み込み処理 */
    for(int i = 0; i < input; i++){
        fscanf(fp, "%d", &inList[i]);
    }

    /* 外部出力線数を取得 */
    fscanf(fp, "%d", &output);

    /* output個のoutListを確保 */
    outList = (int*)malloc(output * sizeof(int));
    if(outList == NULL){
        printf("Failed malloc outList.\n");
        return 1;
    }

    /* 読み込み処理 */
    for(int i = 0; i < output; i++){
        fscanf(fp, "%d", &outList[i]);
    }

    fclose(fp);

    /* 出力線リストを昇順に並べ替える */
    int *workM;
    workM = (int*)malloc(output * sizeof(int));
    if(workM == NULL){
        printf("Failed malloc workM.\n");
        return 1;
    }
    MergeSort(outList, 0, output-1, workM);

    free(workM);

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
    test = malloc(testNum * sizeof(unsigned int*));
    if(test == NULL){
        printf("Failed malloc test.\n");
        return 1;
    }
    for(int i = 0; i < testNum; i++){
        test[i] = malloc(input * sizeof(unsigned int));
        if(test[i] == NULL){
            printf("Failed malloc test[%d].\n", i);
            return 1;
        }
    }

    /* 読み込み処理 */
    for(int i = 0; i < testNum; i++){
        for(int j = 0; j < input; j++){           
            fscanf(fp, "%d", &test[i][j]);              //printf("%2d ", test[i][j]);
        }
        //printf("\n");
    }

    fclose(fp);

    /* 故障リストのファイルをオープン */
    fp = fopen(faultset,"r");
    if(fp == NULL){
        printf("Cannot open '%sf.rep'.\n", argv[1]);
        exit(1);
    }
    /* 故障数を取得 */
    fscanf(fp, "%d", &faultNum);
    /* faultNum * 2個のfaultを確保 */
    /*
    fault = malloc(faultNum * sizeof(unsigned int*));
    if(fault == NULL){
        printf("Failed malloc fault.\n");
        return 1;
    }
    for(int i = 0; i < faultNum; i++){
        fault[i] = malloc(2 * sizeof(unsigned int));
        if(fault[i] == NULL){
            printf("Failed malloc fault[%d].\n", i);
            return 1;
        }
    }
    */


    /* 読み込み処理 */
    for(int i = 0; i < faultNum; i++){
        for(int j = 0; j < 2; j++){           
            fscanf(fp, "%d", &fault[i][j]);              //printf("%2d ", fault[i][j]);
        }
        //printf("\n");
    }

    fclose(fp);    

    /* testNum * output個のexpを確保 */
    exp = malloc(testNum * sizeof(unsigned int*));
    if(exp == NULL){
        printf("Failed malloc queue.\n");
        return 1;
    }
    for(int i = 0; i < testNum; i++){
        exp[i] = malloc(output * sizeof(unsigned int));
        if(exp[i] == NULL){
            printf("Failed malloc exp[%d].\n", i);
            return 1;
        }
    }    

    /* 信号線に、その信号線の入力線数だけ値を格納できるようにする */
    for(int i = 0; i < sigNum; i++){
        sigLine[i].value = malloc(sigLine[i].inputNum * sizeof(unsigned int));
        if(sigLine[i].value == NULL){
            printf("Failed malloc sigLine[%d].value.\n", i);
            return 1;
        }
    }

    /* 信号線に複数の値を格納するためのカウント */
    /* 信号線にいくつ値が格納されているかを示す */
    int *count;
    count = (int*)malloc(sigNum * sizeof(int));
    if(count == NULL){
        printf("Failed malloc count.\n");
        return 1;
    }
    for(int i = 0; i < sigNum; i++){
        count[i] = 0;
    }

    int *turn;
    turn = malloc(sigNum * sizeof(int));
    if(turn == NULL){
        printf("Failed malloc turn.\n");
        return 1;
    }
    int tCount = 0;

    clock_t start, end;
    start = clock();
    /* 以下、論理回路のテスト処理 */
    for(int i = 0; i < testNum; i++){   // 全テストパターンの繰り返し(i=テスト番号-1)
        /* カウントの初期化 */
        for(int j = 0; j < sigNum; j++){
            count[j] = 0;
        }

        /* 外部入力線の値を決めていく */
        for(int j = 0; j < input; j++){
            sigLine[inList[j]-1].value[0] = test[i][j];
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
                LogicCalc(sigLine, j, pointer, count);   // 信号線の値を決定する
                if(tCount < sigNum){
                    turn[tCount] = j;
                    tCount++;
                    //printf("%2d ", turn[tCount-1]);
                }               
            }
            else{
                Enqueue(queue, &tail, j, sigNum-1);
            }     
        }

        unsigned int num;    // デキューした値を格納する変数
        
        /* キューの中身を見ていく */
        while(head != tail)
        {   
            num = Dequeue(queue, &head, sigNum-1);     // 信号線番号を1つ取り出す
            
            if(count[num] < sigLine[num].inputNum){
                Enqueue(queue, &tail, num, sigNum-1);
            }
            else{
                LogicCalc(sigLine, num, pointer, count);   // 信号線の値を決定する
                if(tCount < sigNum){
                    turn[tCount] = num;
                    tCount++;
                    //printf("%2d ", turn[tCount-1]);
                }            
            }            
        }

        for(int j = 0; j < output; j++){
            //printf("%u ", sigLine[outList[j]-1].value[0]);
            exp[i][j] = sigLine[outList[j]-1].value[0];
            //printf("%u ", exp[i][j]);
        }        
        //printf("\n");
    }

    int fCount = 0;       // 故障検出数
    unsigned int judge = judge ^ judge;
    int size = sizeof(unsigned int) * 8;
    //printf("size = %d\n", size);
    /* 以下、故障回路のテスト処理 */
    int l = 0;
    while(l < faultNum){
        judge = judge ^ judge;

        /* マスクの初期化 */
        for(int i = 0; i < sigNum; i++){
            mask0[i] = mask0[i] ^ mask0[i];
            mask1[i] = mask1[i] ^ mask1[i];
        }

        /* マスクの設定 */
        for(int i = 0; i < sigNum; i++){
            int j = l;
            while(j < l + size && j < faultNum){                   
                if(i == fault[j][0] - 1){
                    mask0[i] |= (fault[j][1] << j-l);
                    mask1[i] |= (fault[j][1] << j-l);
                }
                else{
                    mask0[i] |= (0 << j-l);
                    mask1[i] |= (1 << j-l);
                }
                j++;
            }
            //printf("%2d:%u\n", i+1, mask0[i]);
            //printf("%2d:%u\n", i+1, mask1[i]);
        }

        for(int i = 0; i < testNum; i++){   // 全テストパターンの繰り返し(i=テスト番号-1)
            /* カウントの初期化 */
            for(int j = 0; j < sigNum; j++){
                count[j] = 0;
            }

            /* 信号線の値の初期化 */
            for(int j = 0; j < sigNum; j++){
                for(int k = 0; k < sigLine[j].inputNum; k++){
                    sigLine[j].value[k] = sigLine[j].value[k] ^ sigLine[j].value[k];
                }
            }

            /* 外部入力線の値を決めていく */
            for(int j = 0; j < input; j++){
                //sigLine[inList[j]-1].value[0] = test[i][j];
                int k = 0;
                while(k < size && k < faultNum - l){
                    //sigLine[inList[j]-1].value[0] << 1;
                    sigLine[inList[j]-1].value[0] |= (test[i][j] << k);
                    k++;
                }
                //printf("%u ", sigLine[inList[j]-1].value[0]);
            }
            //printf("\n");

            int j = 0;
            while( j < sigNum ){
                BitLogicCalc(sigLine, turn[j], pointer, count, mask0, mask1);   // 信号線の値を決定する
                j++;
            }

            /* 故障の確認 */
            for(int j = 0; j < output; j++){
                //printf("%u ", sigLine[outList[j]-1].value[0]);
                int k = 0;

                if(exp[i][j] == 0){
                    while(k < size && k < faultNum - l){
                        exp[i][j] |= (0 << k);
                        k++;
                    }
                }
                else{
                    while(k < size && k < faultNum - l){
                        exp[i][j] |= (1 << k);
                        k++;
                    } 
                }
                //printf("%u\n", exp[i][j]);

                judge |= (exp[i][j] ^ sigLine[outList[j]-1].value[0]);     
            }        
            //printf("\n");               
        }

        //printf("%d\n", judge);
        int i = 0;
        while( i < size && i < faultNum - l){
            if(judge & (1 << i)){
                fCount++;
                //isFault = 1;
            }
            i++;
        }
        l += size;
    }

    end = clock();

    printf("fault:%2d, detection:%2d, percentage:%2.2f%%, time:%2.2f", faultNum, fCount, ((double)(fCount)/faultNum)*100, (double)(end-start)/CLOCKS_PER_SEC);

    printf("\n");

    free(sigLine);
    free(queue);
    free(mask0);
    free(mask1);

    free(test);
    //free(fault);
    free(exp);
    free(count);
    free(turn);

    return 0;
}