#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define LENGTH 100
#define RANGE 1000

/* 指定されたファイルへ渡された数列を書き込む */
void WriteNumber(char filename[], int data[]){
    FILE *fp;
    int i;

    /* 書き込み用ファイルをオープン */
    fp = fopen(filename,"w");
    if(fp == NULL){
        printf("Cannot open %s.\n", filename);
        exit(1);
    }

    /* ファイルへの書き込み */
    for(i = 0; i < LENGTH; i++){
        fprintf(fp, "%d\n", data[i]);
    }

    fclose(fp);
}

/* データを昇順に並び替える */
void MergeSort(int data[], int left, int right){
    int i, j, k, mid;
    int work[LENGTH];   // 作業用

    if(left < right){
        mid = (left + right) / 2;   // 中心を求める
    
        MergeSort(data, left, mid);     // 左半分を再帰処理
        MergeSort(data, mid+1, right);  // 右半分を再帰処理

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

int main(int argc, char *argv[]){
    int data[LENGTH], i, j;

    if(argc < 3){
        printf("Not enough arguments.\n");
        return 1;
    }

    /* 3.1.1 1000以下の重複しない100個の乱数を発生させ、それらをファイルに書き込む */
    srand((unsigned)time(NULL));    // 乱数シードの初期化
    if((RANGE+1) - LENGTH < 0){   // 発生させる乱数の個数に対して範囲が狭い時のエラー処理
        printf("Error\n");
        exit(1);
    }

    /* 重複しない乱数の生成 */
    for(i = 0; i < LENGTH; i++){
        data[i] = rand() % (RANGE+1);
        for(j = 0; j < i; j++){
            if(data[j] == data[i]){     // 重複するものがあればデクリメントする
                i--;
                break;
            }
        }
    }

    WriteNumber(argv[1], data); // ファイルへ書き込み

    /* 3.1.2 の読み出すデータは既にdata配列に格納されてある */

    /* 3.1.3 データを昇順でソートする */
    MergeSort(data, 0, LENGTH-1);

    /* 3.1.4 昇順にしたデータを別のファイルに書き込む */
    WriteNumber(argv[2], data);

    return 0;
}