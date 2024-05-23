//NOT:Eger sonuc dogru degilse 37.satirdaki +1 i silin/ekleyin.Cunku dosyalardan matrisi okurken satir sonundaki bosluk kodu etkileyebilir.

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

int tempSum;

typedef struct __myarg_t { //Thread argumanlari. a ve b den aldigim ve carpacagim sayilar.
 int a;
 int b;
} myarg_t;

typedef struct __myret_t { //Thread carpim sonucunu basacagim sayi.
int c;
} myret_t;

void* mythread(void *arg) { //Bu ise Threadim. a ile b yi carpiyor ardindan c ye yaziyor.
  myarg_t *m = (myarg_t *) arg;
  myret_t *r = malloc(sizeof(myret_t)); //Allocated in heap.Stackte olusturmak tehlikeli!!!Kaybolabilir.
  r->c = m->a * m->b;
  return (void *)r;
}


int* matrisBoyutBul(char dosya[]){ //Bu fonksiyondaki amacim olusturacagim matrisin boyutunu bulmak.
  int satirSayisi=0;
  char satirBilgi[20];
  int* boyutlar = malloc(2 * sizeof(int)); //Boyutlari matris olustururken kullanacagim.
  FILE *d = fopen(dosya, "r");
  while (fgets(satirBilgi, 20, d) != NULL) {
    satirSayisi++; //Her satirda bir atar. Boylece satir sayimi bulurum.
  }
  fclose(d);
  int sutunSayisi = strlen(satirBilgi)/2+1;  //Satirin uzunlugu stunumdur.(2 ye böldüm çünkü sayılar arası bosluk var ve 1 ekledim).
  //Or:4 sayi varsa 3 aralik olur.(7/2)=3+1=4));
  boyutlar[0] = satirSayisi; 
  boyutlar[1] = sutunSayisi;
  return boyutlar;
}

int** matrisDoldur(char dosya[],int satir,int sutun,int **matris){ //Bu fonksiyonda amacim olusturdugum matrise dosyadaki degerleri yazmak.
  FILE *d = fopen(dosya, "r");
  for (int i = 0; i < satir; i++) {
    for (int j = 0; j < sutun; j++) {
      int sayi;
      fscanf(d, "%d", &sayi); //Verilen dosya akısindan karakter okuyor ve okuduğu karakterleri sayi degiskenine atıyorum.
      matris[i][j] = sayi; //Sayi degiskenindeki degeri matrisime koyuyorum.
    }
  }
  fclose(d);
  return matris; //Doldurdugum matrisi geriye donduruyorum.
}

void matrisBastir(int **matris,int satir,int sutun){ //matrisC'yi ekrana basmak icin kullaniyorum dilerseniz parametre olarak farkli matris verip ekrana basabilirsiniz.
  for (int i = 0; i < satir; i++) {
    for (int j = 0; j < sutun; j++) {
      printf("%d ", matris[i][j]);
    }
    printf("\n");
  }
}


int main() {
  
  int* boyutlarA = matrisBoyutBul("inputA.txt"); //"input.A" dosyasını okuyorum ve matrisA icin boyut belirliyorum.
  int satirSayisiA = boyutlarA[0];
  int sutunSayisiA = boyutlarA[1];
  int** matrisA = malloc(satirSayisiA * sizeof(int*)); //matrisA icin satir sayim kadar bellekten yer istiyorum.
  for (int i = 0; i < satirSayisiA; i++) { //Burada ise her satir icin sutun sayisi kadar yer istiyorum.
    matrisA[i] = malloc(sutunSayisiA * sizeof(int));
  }
  
  //matrisA icin yaptiklarimin aynisini matrisB icin yapiyorum.
  int* boyutlarB = matrisBoyutBul("inputB.txt");
  int satirSayisiB = boyutlarB[0];
  int sutunSayisiB = boyutlarB[1];
  int** matrisB = malloc(satirSayisiB * sizeof(int*));
  for (int i = 0; i < satirSayisiB; i++) {
    matrisB[i] = malloc(sutunSayisiB * sizeof(int));
  }
  
  //Burada matrisDoldur fonksiyonum ile a ve b matrislerimi dosyaya uygun dolduruyorum.
  matrisDoldur("inputA.txt",satirSayisiA,sutunSayisiA,matrisA);
  matrisDoldur("inputB.txt",satirSayisiB,sutunSayisiB,matrisB);

  //matrisA ve b yi kontrol etmek isterseniz alt 2 satiri yorumdan cikarabilirsiniz.
  //matrisBastir(matrisA,satirSayisiA,sutunSayisiA);
  //matrisBastir(matrisB,satirSayisiB,sutunSayisiB);

  //Matris carpiminda matrisA'nin satiri ile matrisB'nin sutunu yeni matrisi olusturur.3x5*5x2=3x2 gibi
  int satirSayisiC = satirSayisiA;
  int sutunSayisiC = sutunSayisiB;
  
int** matrisC = malloc(satirSayisiC * sizeof(int*)); //matrisC icin satir sayim kadar bellekten yer istiyorum.
    for (int i = 0; i < satirSayisiC; i++) { //Burada ise her satir icin sutun sayisi kadar yer istiyorum.
        matrisC[i] = malloc(sutunSayisiC * sizeof(int));
    }
  
  // Matris carpimini yapalimtigim yer.
  for (int i = 0; i < satirSayisiA; i++) {
    for (int j = 0; j < sutunSayisiB; j++) {
      tempSum =0; //istenilen tempSum burada kullanildi.
      for (int k = 0; k < satirSayisiB; k++) {
        //matrisc[i][j] += matrisa[i][k] * matrisb[k][j]; normal matris carpimi yapsaydım bu kodu kullanacktım.Biz çarpimi threadin icinde yaptik.Sonra toplama yaparak yeni oluşturdugum matrise yazdim.
      	
      	//Thread olusturdum.
      	int rc;
      	pthread_t p;
        myarg_t args;
        //Arguman olarak carpacagim degerleri verdim.
        args.a = matrisA[i][k];
        args.b = matrisB[k][j];
	myret_t *r;
        rc = pthread_create(&p, NULL, (void *)mythread, &args);
        pthread_join(p, (void *)&r);
        tempSum=tempSum + r->c;//Critical section.
        matrisC[i][j]=tempSum;//Critical section.
        free(r); //Heap bellekten carpım sonucunu silme.
      }
    }
  }
  //Matris c yazdirma.
  matrisBastir(matrisC,satirSayisiC,sutunSayisiC);
  
  //Matrislerimi free() ile heap bellekten kaldiriyorum.
  free(matrisA);
  free(matrisB);
  free(matrisC);
  return 0;
}
