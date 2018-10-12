#include "stdio.h"
#include "boolean.h"

#define IdxType int
#define MAX_STATES 200
#define MIN_STATES 1
#define MAX_SYMBOLS 200
#define MIN_SYMBOLS 1
#define BLANK ' '
#define ElUndef 999
#define EOP 9999


int main(){
    /***TIPE BENTUKAN***/
    typedef struct{
        int TabInt[MAX_STATES + 1][MAX_SYMBOLS + 1]; //container buat tabel transisi atau papan tic-tac-toe
        int NBrsEff;
        int NKolEff;
    }MATRIKS;

    typedef struct{
        char TabChar[3][3];
        int NBrsEff;
        int NKolEff;
    }MPAPAN;

    /***KAMUS GLOBAL***/
    FILE *File_Transition;      //File Transisi
    FILE *File_Start_States;    //File State Awal
    FILE *File_Win_States;      //File State Menang
    FILE *File_Draw_States;     //File State Seri
    FILE *File_PutOOn;          //File yang berisi konfigurasi untuk dimana menaruh 'O' pada state tertentu
    MATRIKS TRANS;              //Matriks untuk File_Transition
    MATRIKS SSTATES;            //Matriks untuk File_Start_States
    MATRIKS PUTOON;             //Matriks untuk File_PutOOn
    MPAPAN PAPAN;               //Matriks untuk Papan
    MATRIKS WINSTATES;          //Matriks untuk File_Win_States
    MATRIKS DRAWSTATES;         //Matriks untuk File_Draw_States
    MATRIKS STATEHIS;           //Matriks untuk menyimpan state2 yang telah dilewati
    IdxType iHis, jHis;         //Indeks untuk matriks STATEHIS
    int Turn;                   //Sudah berapa turn / berapa kotak yang keisi
    int PChoice;                //Dipakai untuk semua input player
    int CSTATE;                 //Current State

    
    /***FUNGSI DAN PROSEDUR***/
    /**SELEKTOR**/
    #define Elmt(M,i,j) (M).TabInt[(i)][(j)]        //Elemen matriks
    #define ElmtP(P,i,j) (P).TabChar[(i)][(j)]      //Elemen papan
    #define NBrsEff(M) (M).NBrsEff
    #define NKolEff(M) (M).NKolEff
    #define FirstIdxBrs(M) MIN_STATES
    #define LastIdxBrs(M) MIN_STATES+NBrsEff(M)-1
    #define FirstIdxKol(M) MIN_SYMBOLS
    #define LastIdxKol(M) MIN_SYMBOLS+NKolEff(M)-1

    /**KONSTRUKTOR**/
    void Make_Papan(MPAPAN *M, int NB, int NK){     //Membuat sebuah papan blank
        /*KAMUS*/
        IdxType i, j;
        /*ALGORITMA*/
        NBrsEff(*M)=NB;
        NKolEff(*M)=NK;
        for (i=1;i<=NB;i++){                        //Membuat semua elemen di matiks papan menjadi BLANK
            for (j=1;j<=NK;j++){
                ElmtP(*M,i,j)=BLANK;
            }
        }
    }

    /**DESTRUKTOR**/
    void Tutup_File(){
        //Menutup semua file yang digunakan
        fclose(File_Transition);
        fclose(File_Start_States);
        fclose(File_PutOOn);
        fclose(File_Win_States);
        fclose(File_Draw_States);
    }

    /**BERHUBUNGAN DENGAN I/O**/
    void Tulis_Papan(MPAPAN M){                         //Menampilkan kondisi papan
       /*KAMUS*/
       IdxType i, j;
       /*ALGORITMA*/
       for (i=1;i<=3;i++){
           printf("X-----------X\n");                   //Dekorasi
           for (j=1;j<=3;j++){
               printf("|");
               if (j!=3){
                   printf(" %c ", ElmtP(M,i,j));
               }else{                                   //kolom terakhir
                   printf(" %c |\n", ElmtP(M,i,j));
               }
           }
       }
       printf("X-----------X\n");
    }

    void Tulis_Matriks(MATRIKS M){                      //Dipakai untuk debugging dan menampilkan matriks State History
        /*KAMUS*/
        IdxType i, j;

        /*ALGORITMA*/
        for (i=FirstIdxBrs(M);i<=LastIdxBrs(M);i++){
           for (j=FirstIdxKol(M);j<=LastIdxKol(M);j++){
               if (j!=LastIdxKol(M)){
                   printf("%d ", Elmt(M,i,j));
               }else{                                   //kolom terakhir
                   printf("%d\n", Elmt(M,i,j));
               }
           }
       }
    }

    void Baca_File(MATRIKS *TRANS, MATRIKS *SSTATES, MATRIKS *WINSTATES, MATRIKS *DRAWSTATES, MATRIKS *PUTOON){
        /*KAMUS*/
        IdxType i, j;
        int CI;                                         //Integer yang sedang dibaca sekarang, file .txt isinya angka semua
        int retval;                                     //Dipakai untuk fscanf file2

        /*ALGORITMA*/
        //File disiapkan untuk dibaca
        File_Transition=fopen("Transition.txt", "r");
        File_Start_States=fopen("StartStates.txt", "r");
        File_Win_States=fopen("WinStates.txt", "r");
        File_Draw_States=fopen("DrawStates.txt", "r");
        File_PutOOn=fopen("PutOOn.txt", "r");

        //Baca File Transition
        retval=fscanf(File_Transition, "%d", &CI);
        i=1;
        while (CI!=EOP){
            for (j=1;j<=9;j++){
                Elmt(*TRANS,i,j)=CI;
                retval=fscanf(File_Transition, "%d", &CI);
            }
            
            i++;
        }
        NBrsEff(*TRANS)=i-1;
        NKolEff(*TRANS)=9;
        if (NKolEff(*TRANS)>1){
            printf(">>File Transition telah dibaca\n");
        }
        

        //Baca File Start States
        retval=fscanf(File_Start_States, "%d", &CI);
        j=1;
        NBrsEff(*SSTATES)=1;
        while (CI!=EOP){
            Elmt(*SSTATES,1,j)=CI;
            retval=fscanf(File_Start_States, "%d", &CI);
            j++;
        }
        NKolEff(*SSTATES)=j-1;
        if (NKolEff(*SSTATES)>1){
            printf(">>File Start States telah dibaca\n");
        }

        

        //Baca File Win States
        retval=fscanf(File_Win_States, "%d", &CI);
        j=1;
        NBrsEff(*WINSTATES)=1;
        while (CI!=EOP){
            Elmt(*WINSTATES,1,j)=CI;
            retval=fscanf(File_Win_States, "%d", &CI);
            j++;
        }
        NKolEff(*WINSTATES)=j-1;
        if (NKolEff(*WINSTATES)>1){
            printf(">>File Win States telah dibaca\n");
        }
        

        //Baca File Draw States
        retval=fscanf(File_Draw_States, "%d", &CI);
        j=1;
        NBrsEff(*DRAWSTATES)=1;
        while (CI!=EOP){
            Elmt(*DRAWSTATES,1,j)=CI;
            retval=fscanf(File_Draw_States, "%d", &CI);
            j++;
        }
        NKolEff(*DRAWSTATES)=j-1;
        if (NKolEff(*DRAWSTATES)>1){
            printf(">>File Draw States telah dibaca\n");
        }

        //Baca File Put O On
        retval=fscanf(File_PutOOn, "%d", &CI);
        i=1;
        while (CI!=EOP){
            for (j=1;j<=9;j++){
                Elmt(*PUTOON,i,j)=CI;
                retval=fscanf(File_PutOOn, "%d", &CI);
            }
            
            i++;
        }
        NBrsEff(*PUTOON)=i-1;
        NKolEff(*PUTOON)=9;
        if (NKolEff(*PUTOON)>1){
            printf(">>File Put O On telah dibaca\n");
        } 
    }

    int DidWin(int CSTATE, MATRIKS WINSTATES, MATRIKS DRAWSTATES){      //fungsi untuk menentukan apakah Hasilnya menang, seri, atau belum berakhir
        /*KAMUS*/
        IdxType j;
        int Win=0;                                                       //jika nggak ketemu Win==0(belum menang atau seri)

        /*ALGORITMA*/
        //Komputer Menang
        for (j=FirstIdxKol(WINSTATES);j<=LastIdxKol(WINSTATES);j++){
            if (CSTATE == Elmt(WINSTATES,1,j)){
                Win=1;
                break;
            }
        }

        //Komputer Seri
        
        for (j=FirstIdxKol(DRAWSTATES);j<=LastIdxKol(DRAWSTATES);j++){
            if (CSTATE == Elmt(DRAWSTATES,1,j)){
                Win=2;
                break;
            }
        }
        
       
        return(Win);
    }

    /**PRIMITIF VALIDASI**/
    boolean IsValid(int PChoice, int a, int b){     //input player berada pada range yang terdefinisi
        return (PChoice>=a && PChoice<=b);
    }

    boolean IsEff(int PChoice, MPAPAN PAPAN){       //player menginput nomor kotak yang masih "kosong"
        char El;
        switch(PChoice){
            case 1:
                El=ElmtP(PAPAN,1,1);
                break;
            case 2:
                El=ElmtP(PAPAN,1,2);
                break;
            case 3:
                El=ElmtP(PAPAN,1,3);
                break;
            case 4:
                El=ElmtP(PAPAN,2,1);
                break;
            case 5:
                El=ElmtP(PAPAN,2,2);
                break;
            case 6:
                El=ElmtP(PAPAN,2,3);
                break;
            case 7:
                El=ElmtP(PAPAN,3,1);
                break;
            case 8:
                El=ElmtP(PAPAN,3,2);
                break;
            case 9:
                El=ElmtP(PAPAN,3,3);
                break;
        }
        return(El==BLANK);                             //kotak di papan masih kosong dan masih bisa diisi
    }

    /**PRIMITIF LAIN**/
    void PutOOn (int CSTATE, MATRIKS PUTOON, MPAPAN *PAPAN){
        /*KAMUS*/
        IdxType i, j;

        /*ALGORITMA*/
        //mencari CSTATE di matriks PUTOON
        for (i=FirstIdxBrs(PUTOON);i<=LastIdxBrs(PUTOON);i++){
            for (j=FirstIdxKol(PUTOON);j<=LastIdxKol(PUTOON);j++){
                if (Elmt(PUTOON,i,j)==CSTATE){                  //Current State ketemu di PUTOON
                    switch(j){
                        case 1:
                            ElmtP(*PAPAN,1,1)='O';
                            break;
                        case 2:
                            ElmtP(*PAPAN,1,2)='O';
                            break;
                        case 3:
                            ElmtP(*PAPAN,1,3)='O';
                            break;
                        case 4:
                            ElmtP(*PAPAN,2,1)='O';
                            break;
                        case 5:
                            ElmtP(*PAPAN,2,2)='O';
                            break;
                        case 6:
                            ElmtP(*PAPAN,2,3)='O';
                            break;
                        case 7:
                            ElmtP(*PAPAN,3,1)='O';
                            break;
                        case 8:
                            ElmtP(*PAPAN,3,2)='O';
                            break;
                        case 9:
                            ElmtP(*PAPAN,3,3)='O';
                            break;
                    }
                   break;
                }
            }
        }

    }

    void PutXOn (MPAPAN *PAPAN, int PChoice){
        switch(PChoice){
            case 1:
                ElmtP(*PAPAN,1,1)='X';
                break;
            case 2:
                ElmtP(*PAPAN,1,2)='X';
                break;
            case 3:
                ElmtP(*PAPAN,1,3)='X';
                break;
            case 4:
                ElmtP(*PAPAN,2,1)='X';
                break;
            case 5:
                ElmtP(*PAPAN,2,2)='X';
                break;
            case 6:
                ElmtP(*PAPAN,2,3)='X';
                break;
            case 7:
                ElmtP(*PAPAN,3,1)='X';
                break;
            case 8:
                ElmtP(*PAPAN,3,2)='X';
                break;
            case 9:
                ElmtP(*PAPAN,3,3)='X';
                break;
        }
    }

    /***ALGORITMA***/
    Baca_File(&TRANS, &SSTATES, &WINSTATES, &DRAWSTATES, &PUTOON);
    Tutup_File();
    

    printf("\n1 jika Comp dulu, 2 jika Player dulu\n");    
    scanf("%d", &PChoice);
    //Mengecek apakah input valid atau tidak
    while (!IsValid(PChoice, 1, 2)){
        printf("Input tidak valid, masukkan ulang : ");
        scanf("%d", &PChoice);
    }
    //Inisialisasi Papan
    Make_Papan(&PAPAN, 4, 4);

    //Inisialisasi state history
    NBrsEff(STATEHIS)=1;
    NKolEff(STATEHIS)=1;
    iHis=1;
    jHis=1;


    //Komputer duluan
    if (PChoice==1){  
        CSTATE=Elmt(SSTATES,1,1); 
        Turn=1;
        Elmt(STATEHIS,iHis,jHis)=0; 
        PutOOn(CSTATE, PUTOON, &PAPAN);
        
    }
    //Player duluan
    else{
        CSTATE=Elmt(SSTATES,1,2);
        Turn=2;
        Elmt(STATEHIS,iHis,jHis)=32;
        PutXOn(&PAPAN, 5);
        PutOOn(CSTATE, PUTOON, &PAPAN);
        
    }
    
    //papan ditulis/dioutput ke layar
    Tulis_Papan(PAPAN);         
    while (Turn<=9 && DidWin(CSTATE, WINSTATES, DRAWSTATES)==0){
        
        printf("Pilih No. Kotak : ");
        scanf("%d", &PChoice);
        //Mengecek apakah input valid atau tidak
        while (!IsValid(PChoice, 1, 9) || !IsEff(PChoice, PAPAN)){
            printf("Input tidak valid, masukkan kembali! : ");
            scanf("%d", &PChoice);
        }
        //Pemain meletakan X di Papan
        PutXOn(&PAPAN, PChoice);
        //Current State berpindah sesuai tabel transisi
        CSTATE=Elmt(TRANS,CSTATE+1,PChoice);
        //O diletakan di papan sesuai Current State dan tabel PUTOON
        PutOOn(CSTATE, PUTOON, &PAPAN);
        //Papan ditulis ulang
        Tulis_Papan(PAPAN);

        //Update State History
        jHis++;
        Elmt(STATEHIS,iHis,jHis)=CSTATE;
        NKolEff(STATEHIS)++;
        

        Turn++;
    }

    if (DidWin(CSTATE, WINSTATES, DRAWSTATES)==1){
        printf("Komputer Menang\n");
    }else{
        printf("Seri\n");
    }
    printf("State History : ");
    Tulis_Matriks(STATEHIS);

    return 0;
}