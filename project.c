// PROJEKT: PROGRAMOWANIE HYBRYDOWE C/x86
// WERSJA 64-BIT
// (1) - obrót obrazka o 90 stopni (format BMP)
// @ Łukasz Staniszewski

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>

// ZALOZENIA:
// x) ŁADOWANY OBRAZ NIE MUSI MIEC ANI WYSOKOSCI ANI SZEROKOSCI PODZIELNEJ PRZEZ 8 :)
// x) Zakładam że kolor biały jest oznaczany przez bit o wartości 0 a czarny przez bit o wartości 1
// x) Pierwszy bajt siatki pixeli oznacza pierwszą ósemkę pixeli z dolnego lewego rogu obrazka
//// ZALOZENIA TE SĄ SPOWODOWANE PRZEZ ADOBE PHOTOSHOP (W KTORYM TWORZE PLIKI .BMP)
//// ADOBE PHOTOSHOP WYZNACZA WYSOKOSC >0 i USTALA WARTOSC 0 JAKO KOLOR BIALY

extern unsigned char* rotate_8x8(unsigned char* adress_start, unsigned char* ptr_output, int line_bytes_input, int line_bytes_output);

#pragma pack(push, 1)
typedef struct
{
	uint16_t bfType; 
	uint32_t  bfSize; 
	uint16_t bfReserved1; 
	uint16_t bfReserved2; 
	uint32_t  bfOffBits; 
	uint32_t  biSize; 
	int32_t  biWidth; 
	int32_t  biHeight; 
	int16_t biPlanes; 
	int16_t biBitCount; 
	uint32_t  biCompression; 
	uint32_t  biSizeImage; 
	int32_t biXPelsPerMeter; 
	int32_t biYPelsPerMeter; 
	uint32_t  biClrUsed; 
	uint32_t  biClrImportant;
	uint32_t  RGBQuad_0;
	uint32_t  RGBQuad_1;
} bmpHdr;
#pragma pack(pop)


typedef struct
{
	int widthINP, heightINP;			// szerokosc i wysokosc obrazu
	int widthINP_to8, heightINP_to8;	// szerokosc i wysokosc ale zaokraglone do liczby podzielnej przez 8
	unsigned char* pImg;				// wskazanie na początek danych pikselowych wejsciowych
	unsigned char* pImgOut;				// wskazanie na poczatek danych pikselowych wyjsciowych (z tym ze pierwsze kilka linijek to same 0 ktore dorobilem aby mozna bylo obracac obrazy niepodzielne przez 8)
	bmpHdr hdr;				// naglowek pliku
	int lineBytesInput;	// dlugosc w bajtach linii wejsciowej
	int lineBytesOutput;	// dlugosc w bajtach linii wyjsciowej
} imgInfo;

void* freeResources(FILE* pFile, void* pFirst, void* pSnd)
{
	if (pFile != 0)
		fclose(pFile);
	if (pFirst != 0)
		free(pFirst);
	if (pSnd !=0)
		free(pSnd);
	return 0;
}

imgInfo* readBMP(const char* fname)
{
	imgInfo* pInfo = 0;		// informacje o obrazie
	FILE* fbmp = 0;			// plik
	bmpHdr bmpHead;			// naglowek obrazka
	int lineBytes, y;
	unsigned long imageSize = 0;
	unsigned char* ptr;		// punkt na siatke pixeli
	pInfo = 0;
	fbmp = fopen(fname, "rb");
	if (fbmp == 0)
	{
		return 0;
	}
	fread((void *) &bmpHead, sizeof(bmpHead), 1, fbmp);	// wczytanie naglowka
	if (bmpHead.bfType != 0x4D42 || bmpHead.biPlanes != 1 ||
		bmpHead.biBitCount != 1 || (pInfo = (imgInfo *) malloc(sizeof(imgInfo))) == 0)
		return (imgInfo*) freeResources(fbmp, pInfo->pImg, pInfo);		// alokacja dla informacji o pliku
	pInfo->widthINP = bmpHead.biWidth;
	pInfo->heightINP = bmpHead.biHeight;
	int remainder_of_8 = pInfo->heightINP&7;
	int how_many_rows_to_8 = 0;
	if(remainder_of_8 != 0)
	{
		how_many_rows_to_8 = 8-(remainder_of_8);	// ile wierszy pozostalo do bycia podzielnym przez 8
	}
	pInfo->heightINP_to8 = pInfo->heightINP+how_many_rows_to_8;
	lineBytes = ((pInfo->widthINP + 31) >> 5) << 2;
	pInfo->lineBytesInput = lineBytes;
	imageSize = lineBytes * (pInfo->heightINP_to8);
	if ((pInfo->pImg = (unsigned char*) malloc(imageSize)) == 0)	// alokacja dla siatki pixeli
	{
		return (imgInfo*) freeResources(fbmp, pInfo->pImg, pInfo);
	}
	ptr = pInfo->pImg;
	if (fseek(fbmp, bmpHead.bfOffBits, SEEK_SET) != 0)				
	{
		return (imgInfo*) freeResources(fbmp, pInfo->pImg, pInfo);
	}
	memset(ptr, 0x00, imageSize);									// na poczatku siatka pixeli cala na 0x00
	for (y=0; y<pInfo->heightINP; ++y)
	{
		fread(ptr, 1, abs(lineBytes), fbmp);						// wczytanie siatki pixeli
		ptr += lineBytes;
	}
	fclose(fbmp);
	pInfo->hdr = bmpHead;
	return pInfo;													// zwracamy informacje o obrazie
}

int saveBMP(const imgInfo* pInfo, const char* fname)
{
	int lineBytes = pInfo->lineBytesOutput;
	FILE * fbmp;
	unsigned char *ptr;
	int y;
	if ((fbmp = fopen(fname, "wb")) == 0)
		return -1;
	if (fwrite(&pInfo->hdr, sizeof(bmpHdr), 1, fbmp) != 1)			// wrzucamy zmieniony naglowek do pliku
	{
		fclose(fbmp);
		return -2;
	}
	ptr = pInfo->pImgOut;
	ptr += (pInfo->widthINP_to8 - pInfo->widthINP) * lineBytes; 		// przeskakujemy puste wiersze
	for (y=0; y < pInfo->widthINP; y++, ptr += lineBytes)				// wrzucamy siatke pixeli (ze wskaznika na wyjscie)
		if (fwrite(ptr, sizeof(unsigned char), lineBytes, fbmp) != lineBytes)
		{
			fclose(fbmp);
			return -3;
		}
	fclose(fbmp);
	return 0;
}

void allocate_for_output(imgInfo* pInfo)
{
	int lineBytesOutput = ((pInfo->heightINP + 31) >> 5)<<2;
	pInfo->lineBytesOutput = lineBytesOutput;
	int remainder_by_8 = pInfo->widthINP & 7;
	int missing_width_to_8 = 0;
	if(remainder_by_8 != 0)
	{
		missing_width_to_8 = 8-(pInfo->widthINP & 7);
	}
	pInfo->widthINP_to8 = missing_width_to_8 + pInfo->widthINP;
	unsigned long new_size_of_bitmap = lineBytesOutput * (pInfo->widthINP_to8);
	pInfo->pImgOut = (unsigned char*) malloc(new_size_of_bitmap);
	memset(pInfo->pImgOut, 0x00, new_size_of_bitmap);						// najpierw zaalokowana pamiec to same zera (kolor bialy)
}

void change_headder(imgInfo* pInfo)
{
	unsigned long size_bitmap = pInfo->lineBytesOutput * pInfo->widthINP;	
	pInfo->hdr.biWidth = pInfo->heightINP;									// musimy zamienic wysokosc z szerokoscia
	pInfo->hdr.biHeight = pInfo->widthINP;									// szerokosc z wysokoscia
	pInfo->hdr.biSizeImage = size_bitmap;									// rozmiar siatki pixeli
	size_bitmap += sizeof(bmpHdr);
	pInfo->hdr.bfSize = size_bitmap;										// calkowity rozmiar pliku
}


void make_rotation(imgInfo* pInfo)
{
	unsigned char* ptr_input;
	unsigned char* ptr_output;
	unsigned char* start_of_output;
	ptr_input = pInfo->pImg;
	ptr_output = pInfo->pImgOut;
	start_of_output = pInfo->pImgOut+(pInfo->lineBytesOutput*(pInfo->widthINP_to8 - 1));
	int iter_height, iter_width;
	int height_inp_bytes = pInfo->heightINP_to8>>3;
	int width_inp_bytes = pInfo->widthINP_to8>>3;
	for(iter_height=0;iter_height<height_inp_bytes;iter_height++)
	{
		ptr_output=start_of_output + iter_height;
		for(iter_width=0;iter_width<width_inp_bytes;iter_width++)
		{		
		    ptr_output = rotate_8x8(pInfo->pImg+(iter_height<<3)*pInfo->lineBytesInput+iter_width, ptr_output, pInfo->lineBytesInput, pInfo->lineBytesOutput);
		}
	}
}

int main(int argc, char* argv[])
{
	imgInfo* pInfo;
	if (sizeof(bmpHdr) != 62)
	{
		printf("Change compilation options so as bmpHdr struct size is 62 bytes.\n");
		return 1;
	}
	if((pInfo = readBMP("eiti.bmp"))!=0)							// wczytujemy plik
	{
		allocate_for_output(pInfo);									// alokujemy dla wyjsciowej siatki i ustalamy ta pamiec na 0
		change_headder(pInfo);										// zamieniamy naglowek
		make_rotation(pInfo);										// rotujemy poprzez edycje bitow na siatce wyjsciowej
		int result = saveBMP(pInfo, "result.bmp");					// zapisujemy edycje do pliku
		free(pInfo->pImg);
		free(pInfo->pImgOut);
		free(pInfo);
	}
	else
	{
		printf("Read of BMP failed!\n");
	}
	return 0;
}

