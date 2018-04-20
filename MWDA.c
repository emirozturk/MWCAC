#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#define MAX_KELIME_UZUNLUGU (65536)
#define BLOK_BOYUTU 15*1024*1204

struct sozluk
{
    unsigned char* kelime;
    unsigned short uzunluk;
}typedef Sozluk;

char alfaNum[256];
char dil[2];

unsigned long int yazilanBoyut = 0,sikistirilmamisDosyaBoyutu=0;
unsigned char artikBitSayisi = 0;
unsigned char onceki = 0, suAnki = 0; //1 ise alfanümerik 0 ise değil. önceki 1 ve su anki 1 ise boşluk at.
Sozluk sozluk1[256];
Sozluk sozluk2[65536];

unsigned int fBoyutu = 0, sBoyutu = 0, zBoyutu = 0, oBoyutu = 0, s1Boyutu = 0, s2Boyutu = 0;
unsigned int kacisBoyutu = 2;
unsigned char *fDizisi;
unsigned short *sDizisi;
unsigned int *zDizisi;
unsigned char *oDizisi;
unsigned char *s1Dizisi;
unsigned char *s2Dizisi;

char *yazmaBufferi;
unsigned int yazmaBufferSayaci = 0;

unsigned long long bitMask[65] = { 0, 0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80, 0x100, 0x200, 0x400, 0x800, 0x1000, 0x2000, 0x4000, 0x8000, 0x10000, 0x20000, 0x40000, 0x80000, 0x100000, 0x200000, 0x400000, 0x800000, 0x1000000, 0x2000000, 0x4000000, 0x8000000, 0x10000000, 0x20000000, 0x40000000, 0x80000000, 0x100000000, 0x200000000, 0x400000000, 0x800000000, 0x1000000000, 0x2000000000, 0x4000000000, 0x8000000000, 0x10000000000, 0x20000000000, 0x40000000000, 0x80000000000, 0x100000000000, 0x200000000000, 0x400000000000, 0x800000000000, 0x1000000000000, 0x2000000000000, 0x4000000000000, 0x8000000000000, 0x10000000000000, 0x20000000000000, 0x40000000000000, 0x80000000000000, 0x100000000000000, 0x200000000000000, 0x400000000000000, 0x800000000000000, 0x1000000000000000, 0x2000000000000000, 0x4000000000000000, 0x8000000000000000 };


unsigned char bitVer(const unsigned long long deger, unsigned int bitIndex)
{
    if (deger&bitMask[bitIndex])
        return 1;
    else return 0;
}

void alfaNumDoldur()
{
    for (int i = 0; i < 256; i++)
        alfaNum[i] = isalnum(i);
}

void sozlukOlustur()
{
    unsigned int kelimeSayaci = 0;
    unsigned int boyutSayaci = 0;
    unsigned short uzunluk = 0;
    
    while (boyutSayaci < s1Boyutu)
    {
        if(kacisBoyutu==1)
            uzunluk = s1Dizisi[boyutSayaci++];
        else if(kacisBoyutu==2)
        {
            char* parca = (char *)&uzunluk;
            parca[0] = s1Dizisi[boyutSayaci++];
            parca[1] = s1Dizisi[boyutSayaci++];
        }
        unsigned char *buffer = (unsigned char *)calloc(1, uzunluk + 1);
        memcpy(buffer, s1Dizisi + boyutSayaci, uzunluk);
        (sozluk1 + kelimeSayaci + 1)->kelime = buffer;
        (sozluk1 + kelimeSayaci + 1)->uzunluk = uzunluk;
        boyutSayaci += uzunluk;
        ++kelimeSayaci;
    }
    boyutSayaci = 0;
    kelimeSayaci = 0;
    while (boyutSayaci < s2Boyutu)
    {
        if(kacisBoyutu==1)
            uzunluk = s2Dizisi[boyutSayaci++];
        else if(kacisBoyutu==2)
        {
            char* parca = (char *)&uzunluk;
            parca[0] = s2Dizisi[boyutSayaci++];
            parca[1] = s2Dizisi[boyutSayaci++];
        }
        
        unsigned char *buffer = (unsigned char *)calloc(1, uzunluk + 1);
        memcpy(buffer, s2Dizisi + boyutSayaci, uzunluk);
        (sozluk2 + kelimeSayaci)->kelime = buffer;
        (sozluk2 + kelimeSayaci)->uzunluk = uzunluk;
        boyutSayaci += uzunluk;
        ++kelimeSayaci;
    }
}
void kodAc()
{
    unsigned int kelimeSayaci = 0;
    unsigned int zSayaci = 0, fSayaci = 0, sSayaci = 0, oSayaci = 0;
    unsigned short sKod = 0;
    unsigned char fKod = 0;
    unsigned char zBiti;
    unsigned char* parcaPointer;
    unsigned char* kelime;
    unsigned char bosluk = ' ';
    unsigned int kelimeSayisi = zBoyutu * 8 - artikBitSayisi;
    unsigned int otuzIkiSayaci = 0;
    yazmaBufferi = (char *)calloc(BLOK_BOYUTU, 1);
    
    while (kelimeSayaci < kelimeSayisi)
    {
        sKod = -1;
        if (kelimeSayaci != 0 && kelimeSayaci % 32 == 0)
        {
            zSayaci++;
            otuzIkiSayaci = 0;
        }
        zBiti = bitVer(zDizisi[zSayaci], otuzIkiSayaci + 1);
        if (zBiti == 0)
        {
            fKod = fDizisi[fSayaci];
            fSayaci++;
            if (fKod == 0)
            {
                unsigned short uzunluk;
                if (kacisBoyutu == 1)
                {
                    uzunluk = oDizisi[oSayaci];
                    oSayaci++;
                }
                else
                {
                    parcaPointer = (unsigned char *)&uzunluk;
                    *(parcaPointer) = oDizisi[oSayaci];
                    oSayaci++;
                    *(parcaPointer + 1) = oDizisi[oSayaci];
                    oSayaci++;
                }
                kelime = (unsigned char *)malloc(uzunluk);
                memcpy(kelime, oDizisi + oSayaci, uzunluk);
                oSayaci += uzunluk;
                suAnki = alfaNum[kelime[0]];
                if (onceki && suAnki  && yazilanBoyut != 0)
                {
                    yazmaBufferi[yazmaBufferSayaci] = bosluk;
                    ++yazmaBufferSayaci;
                    ++yazilanBoyut;
                }
                memcpy(yazmaBufferi + yazmaBufferSayaci, kelime, uzunluk);
                yazmaBufferSayaci += uzunluk;
                yazilanBoyut += uzunluk;
            }
            else
            {
                kelime = sozluk1[fKod].kelime;
                suAnki = alfaNum[kelime[0]];
                if (onceki && suAnki && yazilanBoyut != 0)
                {
                    yazmaBufferi[yazmaBufferSayaci] = bosluk;
                    ++yazmaBufferSayaci;
                    ++yazilanBoyut;
                }
                memcpy(yazmaBufferi + yazmaBufferSayaci, kelime, sozluk1[fKod].uzunluk);
                yazmaBufferSayaci += sozluk1[fKod].uzunluk;
                yazilanBoyut += sozluk1[fKod].uzunluk;
            }
        }
        else if (zBiti == 1)
        {
            sKod = sDizisi[sSayaci];
            sSayaci++;
            kelime = sozluk2[sKod].kelime;
            suAnki = alfaNum[kelime[0]];
            if (onceki && suAnki && yazilanBoyut != 0)
            {
                yazmaBufferi[yazmaBufferSayaci] = bosluk;
                ++yazmaBufferSayaci;
                ++yazilanBoyut;
            }
            memcpy(yazmaBufferi + yazmaBufferSayaci, kelime, sozluk2[sKod].uzunluk);
            yazmaBufferSayaci += sozluk2[sKod].uzunluk;
            yazilanBoyut += sozluk2[sKod].uzunluk;
        }
        onceki = suAnki;
        ++kelimeSayaci;
        ++otuzIkiSayaci;
    }
}
void WBCA_Uncompress(const char* dosyaAdi)
{
    long dosyaBoyutu = 0;
    alfaNumDoldur();
    //artik,oKacis,z,f,s,o,s1,s2 boyutları + z,f,s,o,s1,s2
    
    
    //Z dosyası
    char *tempName = (char *)calloc(1, strlen(dosyaAdi) + 3);
    strcat(tempName, dosyaAdi);
    strcat(tempName, ".z");
    FILE *fp = fopen(tempName, "rb");
    fseek(fp, 0L, SEEK_END);
    zBoyutu = ftell(fp)-4-sizeof(int);
    fseek(fp, 0L, SEEK_SET);
    fread(dil,2,1,fp);
    fread(&artikBitSayisi, 1, 1, fp);
    fread(&kacisBoyutu,1,1,fp);
    fread(&sikistirilmamisDosyaBoyutu,sizeof(int),1,fp);
    zDizisi = (unsigned int *)malloc(zBoyutu);
    fread(zDizisi,1,zBoyutu,fp);
    fclose(fp);
    free(tempName);
    //F dosyası
    tempName = (char *)calloc(1, strlen(dosyaAdi) + 3);
    strcat(tempName, dosyaAdi);
    strcat(tempName, ".f");
    fp = fopen(tempName, "rb");
    fseek(fp, 0L, SEEK_END);
    fBoyutu = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    fDizisi = (unsigned char *)malloc(fBoyutu);
    fread(fDizisi,1,fBoyutu,fp);
    fclose(fp);
    free(tempName);
    //S dosyası
    tempName = (char *)calloc(1, strlen(dosyaAdi) + 3);
    strcat(tempName, dosyaAdi);
    strcat(tempName, ".s");
    fp = fopen(tempName, "rb");
    fseek(fp, 0L, SEEK_END);
    sBoyutu = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    sDizisi = (unsigned short *)malloc(sBoyutu);
    fread(sDizisi,1,sBoyutu,fp);
    fclose(fp);
    free(tempName);
    //O dosyası
    tempName = (char *)calloc(1, strlen(dosyaAdi) + 3);
    strcat(tempName, dosyaAdi);
    strcat(tempName, ".o");
    fp = fopen(tempName, "rb");
    fseek(fp, 0L, SEEK_END);
    oBoyutu = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    oDizisi = (unsigned char *)malloc(oBoyutu);
    fread(oDizisi,1,oBoyutu,fp);
    fclose(fp);
    free(tempName);
    //S1 ve S2 dosyası
    tempName = (char *)calloc(1, 6);
    strncat(tempName, dil ,2);
    strncat(tempName, "_s1",3);
    fp = fopen(tempName, "rb");
    fseek(fp, 0L, SEEK_END);
    int sozlukBoyutu = ftell(fp);
    s1Boyutu = sozlukBoyutu;
    fseek(fp, 0L, SEEK_SET);
    s1Dizisi = (unsigned char *)calloc(1,sozlukBoyutu);
    fread(s1Dizisi, sozlukBoyutu, 1, fp);
    fclose(fp);
    free(tempName);
    
    tempName = (char *)calloc(1, 6);
    strncat(tempName, dil ,2);
    strncat(tempName, "_s2",3);
    fp = fopen(tempName, "rb");
    fseek(fp, 0L, SEEK_END);
    sozlukBoyutu = ftell(fp);
    s2Boyutu = sozlukBoyutu;
    fseek(fp, 0L, SEEK_SET);
    s2Dizisi = (unsigned char *)calloc(1,sozlukBoyutu);
    fread(s2Dizisi, sozlukBoyutu, 1, fp);
    fclose(fp);
    free(tempName);
    
    sozlukOlustur();
    kodAc();
}
int main(int argc, char *argv[])
{
    WBCA_Uncompress(argv[1]);
    char *tempName = (char *)calloc(1, strlen(argv[1]) + 4);
    strcat(tempName, argv[1]);
    strcat(tempName, ".wba");
    FILE *fpOut = fopen(tempName, "wb");
    fwrite(yazmaBufferi, 1, yazmaBufferSayaci, fpOut);
    fclose(fpOut);
    return 0;
}
