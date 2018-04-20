#include <stdio.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define swap( x, y ) { int temp; temp=*(x); *(x)=*(y); *(y)=temp; }
#define BLOK_BOYUTU 20*1024*1204
#define SEED 1159241
#define JUMP 101

struct hash {
    unsigned char *kelime;
    unsigned int uzunluk;
    unsigned int frekans;
    unsigned int kod;
}typedef Hash;

Hash* hashSet;
unsigned int addrInTH;
unsigned int zeroNode;
unsigned int *positionInTH;
unsigned int hashElemanSayisi;
int hashBuyuklugu = 0;

long sikistirilmamisDosyaBoyutu=0;
long kelimeSayisi = 0;
long toplamKelimeSayisi;
char alfaNum[256];
unsigned int zSayaci = 0;
unsigned int fBoyutu = 0, sBoyutu = 0, oBoyutu = 0, s1Boyutu = 0, s2Boyutu = 0;
unsigned int *zDizisi;
unsigned char *fDizisi;
unsigned short *sDizisi;
unsigned char *oDizisi;
unsigned char *s1Dizisi;
unsigned char *s2Dizisi;
unsigned int maxKelimeUzunlugu = 0;
unsigned char kacisBoyutu = 0;      //Sözlük ve S3 dosyası kelimelerini ayıran boyutları saklayan kaçış (5level3the)
unsigned int outputBoyutu;
unsigned long long bitMask[65] = { 0, 0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80, 0x100, 0x200, 0x400, 0x800, 0x1000, 0x2000, 0x4000, 0x8000, 0x10000, 0x20000, 0x40000, 0x80000, 0x100000, 0x200000, 0x400000, 0x800000, 0x1000000, 0x2000000, 0x4000000, 0x8000000, 0x10000000, 0x20000000, 0x40000000, 0x80000000, 0x100000000, 0x200000000, 0x400000000, 0x800000000, 0x1000000000, 0x2000000000, 0x4000000000, 0x8000000000, 0x10000000000, 0x20000000000, 0x40000000000, 0x80000000000, 0x100000000000, 0x200000000000, 0x400000000000, 0x800000000000, 0x1000000000000, 0x2000000000000, 0x4000000000000, 0x8000000000000, 0x10000000000000, 0x20000000000000, 0x40000000000000, 0x80000000000000, 0x100000000000000, 0x200000000000000, 0x400000000000000, 0x800000000000000, 0x1000000000000000, 0x2000000000000000, 0x4000000000000000, 0x8000000000000000 };

void alfaNumDoldur(char *alfaNum)
{
    for (int i = 0; i < 256; i++)
        alfaNum[i] = isalnum(i);
}

unsigned int kelimeSayisiTahmini(const long dosyaBoyutu)
{
    if (dosyaBoyutu<15 * 1024 * 1024)
        return 500000;
    else if (dosyaBoyutu<30 * 1024 * 1024)
        return 1000000;
    else
        return 5000000;
}

unsigned int NearestPrime(const unsigned int n)
{
    unsigned int position;
    unsigned int index;
    
    for (position = n; ; position++)
    {
        for (index = 2; index <= (unsigned long)sqrt((double)position) && position % index != 0; index++);
        if (position % index != 0)
            break;
    }
    return position;
}

unsigned int insertElement(Hash *hashSet, unsigned char *kelime, const unsigned int uzunluk, unsigned int *addr)
{
    hashSet[*addr].kelime = (unsigned char *)malloc(uzunluk * sizeof(unsigned char));
    strncpy((char *)hashSet[*addr].kelime, (char *)kelime, uzunluk);
    hashSet[*addr].kelime[uzunluk] = '\0';
    hashSet[*addr].frekans = 0;
    hashElemanSayisi++;
    return *addr;
}

unsigned int hashFunction(const unsigned char *key, const unsigned int len)   //MURMUR HASH
{
    static const unsigned int c1 = 0xcc9e2d51;
    static const unsigned int c2 = 0x1b873593;
    static const unsigned int r1 = 15;
    static const unsigned int r2 = 13;
    static const unsigned int m = 5;
    static const unsigned int n = 0xe6546b64;
    
    unsigned int hash = SEED;
    
    const int nblocks = len / 4;
    const unsigned int *blocks = (const unsigned int *)key;
    int i;
    unsigned int k;
    for (i = 0; i < nblocks; i++) {
        k = blocks[i];
        k *= c1;
        k = (k << r1) | (k >> (32 - r1));
        k *= c2;
        
        hash ^= k;
        hash = ((hash << r2) | (hash >> (32 - r2)))*m + n;
    }
    
    const unsigned char *tail = (const unsigned char *)(key + nblocks * 4);
    unsigned int k1 = 0;
    
    switch (len & 3) {
        case 3:
            k1 ^= tail[2] << 16;
        case 2:
            k1 ^= tail[1] << 8;
        case 1:
            k1 ^= tail[0];
            
            k1 *= c1;
            k1 = (k1 << r1) | (k1 >> (32 - r1));
            k1 *= c2;
            hash ^= k1;
    }
    
    hash ^= len;
    hash ^= (hash >> 16);
    hash *= 0x85ebca6b;
    hash ^= (hash >> 13);
    hash *= 0xc2b2ae35;
    hash ^= (hash >> 16);
    
    return hash % hashBuyuklugu;
}

int strcomp(const unsigned char *s1, const unsigned char *s2, const unsigned int ws1, const unsigned int ws2)
{
    if (ws1 != ws2)
        return -1;
    unsigned int iters = 1;
    while (iters < ws1 && *s1 == *s2)
    {
        s1++;
        s2++;
        iters++;
    }
    return(*s1 - *s2);
}

unsigned int search(Hash *hashSet, const unsigned char *kelime, const unsigned int uzunluk, unsigned int *returnedAddr)
{
    
    unsigned int addr;
    addr = hashFunction(kelime, uzunluk);
    while ((hashSet[addr].kelime != NULL) && strcomp(hashSet[addr].kelime, kelime, hashSet[addr].uzunluk, uzunluk))
        addr = ((addr + JUMP) % hashBuyuklugu);
    *returnedAddr = addr;
    if (hashSet[addr].kelime == NULL)
        return hashElemanSayisi;
    return 0;
}

void e_strcat(unsigned char *anaDizi, const unsigned char *parca, const unsigned int konum, const unsigned int boyut)
{
    for (unsigned int i = konum; i < konum + boyut; i++)
        anaDizi[i] = parca[i - konum];
}

unsigned char bitVer(const unsigned long long deger, unsigned int bitIndex)
{
    if (deger&bitMask[bitIndex])
        return 1;
    else return 0;
}


int frekansListesiKarsilastir(const void *a, const void *b)
{
    unsigned int *left, *right;
    left = (unsigned int *)a;
    right = (unsigned int *)b;
    if (hashSet[*left].frekans < hashSet[*right].frekans)
        return 1;
    if (hashSet[*left].frekans > hashSet[*right].frekans)
        return -1;
    return 0;
}

void sozlukOlustur(char kacisBoyutu)
{
    unsigned int toplamUzunluk1 = 0;
    unsigned int toplamUzunluk2 = 0;
    for (int i = 0; i<255; i++)
    {
        if (positionInTH[i] == -1)
            break;
        toplamUzunluk1 += hashSet[positionInTH[i]].uzunluk+kacisBoyutu; //Her bir kelimenin uzunluğu için başına + kaçış
    }
    for (int i = 255; i<255 + 65536; i++)
    {
        if (positionInTH[i] == -1)
            break;
        toplamUzunluk2 += hashSet[positionInTH[i]].uzunluk+kacisBoyutu; //Her bir kelimenin uzunluğu için başına + kaçış
    }
    s1Dizisi = (unsigned char*)calloc(1, toplamUzunluk1+1);
    s2Dizisi = (unsigned char*)calloc(1, toplamUzunluk2);
    s1Dizisi[0] = kacisBoyutu;
    s1Boyutu++;
    for (int i = 0; i < 255; i++)
    {
        if (positionInTH[i] == -1)
            break;
        if(kacisBoyutu==1)
        {
            s1Dizisi[s1Boyutu] = hashSet[positionInTH[i]].uzunluk;
            s1Boyutu++;
        }
        else if(kacisBoyutu==2)
        {
            char *parca = (char *)&hashSet[positionInTH[i]].uzunluk;
            s1Dizisi[s1Boyutu] = parca[0];
            s1Dizisi[s1Boyutu+1]= parca[1];
            s1Boyutu+=2;
        }
        e_strcat(s1Dizisi, hashSet[positionInTH[i]].kelime, s1Boyutu, hashSet[positionInTH[i]].uzunluk);
        s1Boyutu += hashSet[positionInTH[i]].uzunluk;
    }
    for (int i = 255; i < 65536 + 255; i++)
    {
        if (positionInTH[i] == -1)
            break;
        if(kacisBoyutu==1)
        {
            s2Dizisi[s2Boyutu] = hashSet[positionInTH[i]].uzunluk;
            s2Boyutu++;
        }
        else if(kacisBoyutu==2)
        {
            char *parca = (char *)&hashSet[positionInTH[i]].uzunluk;
            s2Dizisi[s2Boyutu] = parca[0];
            s2Dizisi[s2Boyutu+1]= parca[1];
            s2Boyutu+=2;
        }
        e_strcat(s2Dizisi, hashSet[positionInTH[i]].kelime, s2Boyutu, hashSet[positionInTH[i]].uzunluk);
        s2Boyutu += hashSet[positionInTH[i]].uzunluk;
    }
    free(hashSet);
}

void kodla(unsigned char *b, size_t input_length)
{
    unsigned int intSayisi = (unsigned int)ceil((float)toplamKelimeSayisi / 32);	//int BV len
    zDizisi = (unsigned int *)calloc(1, intSayisi * sizeof(unsigned int));
    fDizisi = (unsigned char *)calloc(1, BLOK_BOYUTU);
    sDizisi = (unsigned short *)calloc(1, BLOK_BOYUTU);
    oDizisi = (unsigned char *)calloc(1, BLOK_BOYUTU);
    unsigned char *p, *son;
    unsigned int boyut = 0, kod = 0;
    p = b;
    son = p + input_length;
    unsigned char *kelime;
    
    while (p < son)
    {
        boyut = 0;
        kelime = p;
        if (alfaNum[*p])
        {
            while (alfaNum[*p] && p < son)
            {
                boyut++;
                p++;
            }
        }
        else
        {
            if (*p != ' ')
            {
                while (!alfaNum[*p] && p < son)
                {
                    boyut++;
                    p++;
                }
            }
            else
            {
                p++;
                if (alfaNum[*p])
                {
                    kelime = p;
                    while (alfaNum[*p] && p < son)
                    {
                        boyut++;
                        p++;
                    }
                }
                else
                {
                    boyut++;
                    while (!alfaNum[*p] && p < son)
                    {
                        boyut++;
                        p++;
                    }
                }
            }
        }
        if (p != son)
        {
            search(hashSet, (unsigned char *)kelime, boyut, &addrInTH);
            kod = hashSet[addrInTH].kod;
            if (kod > 0 && kod < 256) //S1
            {
                zDizisi[zSayaci] &= ~(bitMask[kelimeSayisi % 32 + 1]);
                fDizisi[fBoyutu++] = kod;
            }
            else if (kod >= 256 && kod < 65536 + 256)	//s
            {
                kod -= 256;
                zDizisi[zSayaci] |= bitMask[kelimeSayisi % 32 + 1];
                sDizisi[sBoyutu++] = kod;
            }
            else					//S3
            {
                kod = 0;
                zDizisi[zSayaci] &= ~(bitMask[kelimeSayisi % 32 + 1]);
                fDizisi[fBoyutu++] = kod;
                if (kacisBoyutu == 1)
                    oDizisi[oBoyutu++] = boyut;
                else if (kacisBoyutu == 2)
                {
                    char *parca = (char *)&boyut;
                    oDizisi[oBoyutu++] = parca[0];
                    oDizisi[oBoyutu++] = parca[1];
                }
                e_strcat(oDizisi, kelime, oBoyutu, boyut);
                oBoyutu += boyut;
            }
            ++kelimeSayisi;
            if (kelimeSayisi != 0 && !(kelimeSayisi % 32))++zSayaci;
        }
    }
    search(hashSet, (unsigned char *)kelime, boyut, &addrInTH);
    kod = hashSet[addrInTH].kod;
    if (kod > 0 && kod < 256) //f
    {
        zDizisi[zSayaci] &= ~(bitMask[kelimeSayisi % 32 + 1]);
        fDizisi[fBoyutu++] = kod;
    }
    else if (kod >= 256 && kod < 65536 + 256)	//S2
    {
        kod -= 256;
        zDizisi[zSayaci] |= bitMask[kelimeSayisi % 32 + 1];
        sDizisi[sBoyutu++] = kod;
    }
    else					//S3
    {
        kod = 0;
        zDizisi[zSayaci] &= ~(bitMask[kelimeSayisi % 32 + 1]);
        if (kacisBoyutu == 1)
            oDizisi[oBoyutu++] = boyut;
        else if (kacisBoyutu == 2)
        {
            char *parca = (char *)&boyut;
            oDizisi[oBoyutu++] = parca[0];
            oDizisi[oBoyutu++] = parca[1];
        }
        e_strcat(oDizisi, kelime, oBoyutu, boyut);
        oBoyutu += boyut;
    }
    ++kelimeSayisi;
    if (kelimeSayisi != 0 && !(kelimeSayisi % 32))++zSayaci;
}

void kodAta()
{
    for (int i = 0; i < hashBuyuklugu; i++)
        hashSet[i].kod = -1;
    for (int i = 0; i < 255 + 65536; i++)
    {
        if (positionInTH[i] == -1)
            break;
        hashSet[positionInTH[i]].kod = i + 1;
    }
}
void frekansSirala()
{
    unsigned int k;
    long ii;
    long kk;
    kk = zeroNode - 1;
    ii = 0;
    while (ii < kk)
    {
        while ((hashSet[positionInTH[ii]].frekans != 1) && (ii < kk)) { ii++; }
        while ((hashSet[positionInTH[kk]].frekans == 1) && (ii < kk)) { kk--; }
        if (ii < kk) {
            swap(&positionInTH[ii], &positionInTH[kk]);
            kk--;
            ii++;
        }
    }
    k = ii + 1;
    qsort(positionInTH, k, sizeof(unsigned int), frekansListesiKarsilastir);
}
void kelimeCikar(unsigned char *b, size_t input_length)	//SPACELESS WORD MODEL
{
    unsigned char *p, *son;
    unsigned int boyut = 0, j = 0;
    p = b;
    son = p + input_length;
    unsigned char *kelime;
    while (p < son)
    {
        boyut = 0;
        kelime = p;
        if (alfaNum[*p])
        {
            while (alfaNum[*p] && p < son)
            {
                boyut++;
                p++;
            }
        }
        else
        {
            if (*p != ' ')
            {
                while (!alfaNum[*p] && p < son)
                {
                    boyut++;
                    p++;
                }
            }
            else
            {
                p++;
                if (alfaNum[*p])
                {
                    kelime = p;
                    while (alfaNum[*p] && p < son)
                    {
                        boyut++;
                        p++;
                    }
                }
                else
                {
                    boyut++;
                    while (!alfaNum[*p] && p < son)
                    {
                        boyut++;
                        p++;
                    }
                }
            }
        }
        if (p != son)
        {
            toplamKelimeSayisi++;
            j = search(hashSet, (unsigned char *)kelime, boyut, &addrInTH);
            if (j == zeroNode)
            {
                insertElement(hashSet, (unsigned char *)kelime, boyut, &addrInTH);
                hashSet[addrInTH].uzunluk = boyut;
                positionInTH[zeroNode] = addrInTH;
                zeroNode++;
            }
            hashSet[addrInTH].frekans += 1;
        }
        if (boyut>maxKelimeUzunlugu)maxKelimeUzunlugu = boyut;
    }
    toplamKelimeSayisi++;
    j = search(hashSet, (unsigned char *)kelime, boyut, &addrInTH);
    if (j == zeroNode)
    {
        insertElement(hashSet, (unsigned char *)kelime, boyut, &addrInTH);
        hashSet[addrInTH].uzunluk = boyut;
        positionInTH[zeroNode] = addrInTH;
        zeroNode++;
    }
    hashSet[addrInTH].frekans += 1;
    if (boyut>maxKelimeUzunlugu)maxKelimeUzunlugu = boyut;
}
void hashSetAc(const unsigned int kelimeSayisi)
{
    hashBuyuklugu = NearestPrime(kelimeSayisi);
    hashSet = (Hash *)calloc(hashBuyuklugu, sizeof(Hash));
    positionInTH = (unsigned int*)malloc(hashBuyuklugu * sizeof(unsigned int));
    if (positionInTH)
        for (unsigned int i = 0; i < hashBuyuklugu; i++)
            positionInTH[i] = -1;
    else
        printf("HashSet acilamadi...");
}
void ikinciGecis(unsigned char *input, size_t input_length,const char *dosyaAdi)
{
    if (maxKelimeUzunlugu<256)kacisBoyutu = 1;
    else if (maxKelimeUzunlugu<65536)kacisBoyutu = 2;
    
    kodla(input, input_length);
    sozlukOlustur(kacisBoyutu);
    
    zSayaci++;
    unsigned char artikBitSayisi = 32 - (kelimeSayisi % 32);
    
    //BV
    char *tempName = (char *)calloc(1, strlen(dosyaAdi) + 3);
    strcat(tempName, dosyaAdi);
    strcat(tempName, ".z");
    FILE *fpOut = fopen(tempName, "wb");
    fwrite(&artikBitSayisi,1,1,fpOut);
    fwrite(&sikistirilmamisDosyaBoyutu,4,1,fpOut);
    fwrite(zDizisi, 4, zSayaci, fpOut);
    fclose(fpOut);
    free(tempName);
    //S1
    tempName = (char *)calloc(1, strlen(dosyaAdi) + 3);
    strcat(tempName, dosyaAdi);
    strcat(tempName, ".f");
    fpOut = fopen(tempName, "wb");
    fwrite(fDizisi, 1, fBoyutu, fpOut);
    fclose(fpOut);
    free(tempName);
    //S2
    tempName = (char *)calloc(1, strlen(dosyaAdi) + 3);
    strcat(tempName, dosyaAdi);
    strcat(tempName, ".s");
    fpOut = fopen(tempName, "wb");
    fwrite(sDizisi, 2, sBoyutu, fpOut);
    fclose(fpOut);
    free(tempName);
    //S3
    tempName = (char *)calloc(1, strlen(dosyaAdi) + 3);
    strcat(tempName, dosyaAdi);
    strcat(tempName, ".o");
    fpOut = fopen(tempName, "wb");
    fwrite(oDizisi, 1, oBoyutu, fpOut);
    fclose(fpOut);
    free(tempName);
    //D1
    tempName = (char *)calloc(1, strlen(dosyaAdi) + 4);
    strcat(tempName, dosyaAdi);
    strcat(tempName, ".s1");
    fpOut = fopen(tempName, "wb");
    fwrite(s1Dizisi, 1, s1Boyutu, fpOut);
    fclose(fpOut);
    free(tempName);
    //D2
    tempName = (char *)calloc(1, strlen(dosyaAdi) + 4);
    strcat(tempName, dosyaAdi);
    strcat(tempName, ".s2");
    fpOut = fopen(tempName, "wb");
    fwrite(s2Dizisi, 1, s2Boyutu, fpOut);
    fclose(fpOut);
    free(tempName);
}
void birinciGecis(unsigned char *input, size_t input_length)
{
    hashSetAc(kelimeSayisiTahmini(input_length));
    kelimeCikar(input, input_length);
    frekansSirala();
    kodAta();
}
void MWCA_Compress(unsigned char* input, size_t input_length,const char *dosyaAdi)
{
    alfaNumDoldur(alfaNum);
    birinciGecis(input, input_length);
    ikinciGecis(input, input_length,dosyaAdi);
}
int main(int argc, const char * argv[])
{
    unsigned char *input;
    FILE *fp = fopen(argv[1], "rb");
    fseek(fp, 0L, SEEK_END);
    sikistirilmamisDosyaBoyutu = ftell(fp);
    input = (unsigned char *)malloc(sikistirilmamisDosyaBoyutu);
    fseek(fp, 0L, SEEK_SET);
    fread(input, sikistirilmamisDosyaBoyutu, 1, fp);
    fclose(fp);
    
    MWCA_Compress(input,  sikistirilmamisDosyaBoyutu,argv[1]);
    return 0;
}
