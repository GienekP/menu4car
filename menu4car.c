
/*--------------------------------------------------------------------*/
/* menu4car                                                           */
/* by GienekP                                                         */
/* (c) 2023                                                           */
/*--------------------------------------------------------------------*/
#include <stdio.h>
/*--------------------------------------------------------------------*/
typedef unsigned char U8;
/*--------------------------------------------------------------------*/
#define CARMAX (1024*1024)
#define FLASHMAX (512*1024)
#define BANKSIZE (0x2000)
#define PATHLEN (0x400)
#define NAMELEN (0x30)
#define DELIM	('|')
/*--------------------------------------------------------------------*/
#include "menu4car.h"
/*--------------------------------------------------------------------*/
U8 ATASCII2Internal(U8 a)
{
	if (((a&0x7F)>=0x00) && ((a&0x7F)<=0x1F)) return a+0x40;
	if (((a&0x7F)>=0x20) && ((a&0x7F)<=0x5F)) return a-0x20;
	return a;
}
typedef struct {char * key; unsigned char val;} DictEntry;
// WARNING! This file must be in UTF-8 format
// and amd letters with ogonek's and acute's must be visible
DictEntry UTF8Trans[] = {
{"",0x20},
{" ",0x20}, {"!",0x21}, {"\"",0x22},{"#",0x23}, {"$",0x24}, {"%",0x25}, {"&",0x26}, {"'",0x27},
{"(",0x28}, {")",0x29}, {"*",0x2a}, {"+",0x2b}, {",",0x2c}, {"-",0x2d}, {".",0x2e}, {"/",0x2f},
{"0",0x30}, {"1",0x31}, {"2",0x32}, {"3",0x33}, {"4",0x34}, {"5",0x35}, {"6",0x36}, {"7",0x37},
{"8",0x38}, {"9",0x39}, {":",0x3a}, {";",0x3b}, {"<",0x3c}, {"=",0x3d}, {">",0x3e}, {"?",0x3f},
{"@",0x40}, {"A",0x41}, {"B",0x42}, {"C",0x43}, {"D",0x44}, {"E",0x45}, {"F",0x46}, {"G",0x47},
{"H",0x48}, {"I",0x49}, {"J",0x4a}, {"K",0x4b}, {"L",0x4c}, {"M",0x4d}, {"N",0x4e}, {"O",0x4f},
{"P",0x50}, {"Q",0x51}, {"R",0x52}, {"S",0x53}, {"T",0x54}, {"U",0x55}, {"V",0x56}, {"W",0x57},
{"X",0x58}, {"Y",0x59}, {"Z",0x5a}, {"[",0x5b}, {"\\",0x5c}, {"]",0x5d}, {"^",0x5e}, {"_",0x5f},
{"`",0x60}, {"a",0x61}, {"b",0x62}, {"c",0x63}, {"d",0x64}, {"e",0x65}, {"f",0x66}, {"g",0x67},
{"h",0x68}, {"i",0x69}, {"j",0x6a}, {"k",0x6b}, {"l",0x6c}, {"m",0x6d}, {"n",0x6e}, {"o",0x6f},
{"p",0x70}, {"q",0x71}, {"r",0x72}, {"s",0x73}, {"t",0x74}, {"u",0x75}, {"v",0x76}, {"w",0x77},
{"x",0x78}, {"y",0x79}, {"z",0x7a}, {"|",0x7c}, {"~",0x7d}, {"{",0x7e}, {"}",0x7f},
{"Ą",0x17}, {"Ć",0x16}, {"Ę",0x12}, {"Ł",0x0B}, {"Ń",0x0D}, {"Ó",0x10}, {"Ś",0x04}, {"Ź",0x18},
{"Ż",0x00}, {"ą",0x01}, {"ć",0x03}, {"ę",0x05}, {"ł",0x0C}, {"ń",0x0E}, {"ó",0x0F}, {"ś",0x13},
{"ź",0x02}, {"ż",0x1A}, {"Ä",0x19}, {"Ë",'E'}, {"Ö",0x07}, {"Ü",0x15}, {"ä",0x14}, {"ë",'e'},
{"ö",0x06}, {"ü",0x09}, {"ß",0x0A}, {"£",0x08}, {"±",0x1B}, {"←",0x1E}, {"↑",0x1C}, {"→",0x1F},
{"↓",0x1D}, {NULL,0}
};

#define GETW(b,i) (b[(i)]|(b[(i)+1])<<8)
unsigned int getUTF8(U8 **name)
{
			unsigned int utf8=(unsigned int)(U8)(**name);
			(*name)++;
			if (utf8&0x80)
				while (((**name)&0xc0)==0x80) {
					utf8<<=8;
					utf8|=(**name);
					(*name)++;
				}
			return utf8;
}
unsigned int np_getUTF8(U8 *name) {
	return getUTF8(& name);
}
#define UTF8(S) np_getUTF8((U8*)S)
/*--------------------------------------------------------------------*/
void fillATASCII(U8 *txt, const U8 *name, unsigned int limit)
{
	unsigned int i,j=0;
	U8 * nameptr=(U8 *)name;
	int endstring=0;
	for (i=0; i<limit; i++)
	{
		unsigned int c=getUTF8(&nameptr);
		if (c==0)
		{
			//c=0x20;
			break;
		}
		else
		{
			DictEntry * de=&UTF8Trans[0];
			while (de->key) {
				if (c==UTF8(de->key)) {c=de->val; break;}
				de++;
			}
		}

		txt[i]=ATASCII2Internal((U8)c);
	};
}
/*--------------------------------------------------------------------*/
unsigned int clearPos(U8 *data, unsigned int pos)
{
	unsigned int k=(32*4+16*32+32*pos);
	data[k+3]=0;
	data[k+4]=0;
	data[4*pos]|=0x80;
	data[4*(pos+1)]=data[4*pos];
	data[4*(pos+1)+1]=data[4*pos+1];
	data[4*(pos+1)+2]=data[4*pos+2];
	data[4*(pos+1)+3]=data[4*pos+3];
	return (pos+1);
}
/*--------------------------------------------------------------------*/
unsigned int insertPos(const char *name, U8 *data, unsigned int carsize, unsigned int pos,
					const U8 *buf, unsigned int size)
{
	unsigned int i,ret=0;
	unsigned int start,stop;
	if (pos==0) 
	{
		start=BANKSIZE;
		stop=BANKSIZE+size;
		data[4*pos+0]=0;
		data[4*pos+1]=1;
		data[4*pos+2]=0;
		data[4*pos+3]=0;	
	}
	else
	{
		unsigned int bh=data[4*pos];
		unsigned int bl=data[4*pos+1];
		unsigned int ah=data[4*pos+2];
		unsigned int al=data[4*pos+3];
		bh&=0x7F;
		data[4*pos]=bh;
		start=((((bh<<7)+bl)*BANKSIZE)|(((ah<<8)|al)&0x1FFF));
	};
	stop=(start+size);
	if (stop>carsize) 
	{
		clearPos(data,pos);
		ret=stop-carsize;
		stop=start;
	}
	else
	{
		for (i=0; i<size; i++) {data[start+i]=buf[i];};
		data[4*pos+4]=(0x80|(stop/BANKSIZE/0x80));
		data[4*pos+5]=((stop/BANKSIZE)&0x7F);
		data[4*pos+6]=((stop>>8)&0x1F);
		data[4*pos+7]=(stop&0xFF);

		data[32*4+16*32+32*pos+3]='A'+pos-0x20;
		data[32*4+16*32+32*pos+4]='.'-0x20;
		fillATASCII(&data[32*4+16*32+32*pos+6],(U8 *)name,24);
	};

	//printf("OFFSET: $%06x: file \"%s\", length $%04x bytes.\n",start,name,size);
	return ret;
}
/*--------------------------------------------------------------------*/
unsigned int loadFile(const char *path, U8 *buf, unsigned int sizebuf)
{
	unsigned int size=0;
	FILE *pf;
	pf=fopen(path,"rb");
	if (pf)
	{
		size=fread(buf,sizeof(U8),sizebuf,pf);
		fclose(pf);
	}
	else
	{
		printf("Load Error \"%s\".\n",path);
	};
	return size;
}
/*--------------------------------------------------------------------*/
/*void saveRAW(U8 *raw, unsigned int size)
{
	FILE *pf;
	pf=fopen("RAW.XEX","wb");
	fwrite(raw,1,size,pf);
	fclose(pf);
};*/
/*--------------------------------------------------------------------*/
unsigned int repairFile(U8 *buf, unsigned int size)
{
	unsigned int i=0,j,first=0xFFFF,run=0,init=0,ret=size;
	if (GETW(buf,0)==0xFFFF)
	{
		unsigned int a,b,start,stop;
		i+=2;
		while (i<ret)
		{
			if (GETW(buf,i)==0xFFFF)
			{
				for (j=i; j<ret; j++) {buf[j]=buf[j+2];};
				ret-=2;
			};
			start = GETW(buf,i);
			stop  = GETW(buf,i+2);
			if (start>stop) {ret=i;}
			else
			{
				i+=4;
				if (first==0xFFFF) {first=start;};
				if ((start<=0x02E0) && (stop>=0x02E1)) {run=1;};
				if ((start<=0x02E2) && (stop>=0x02E3)) {init=1;};
				i+=(1+stop-start);
			};
		};
		if (run==0)
		{
			if (init==0)
			{
				U8 runad[6]={0xE0, 0x02, 0xE1, 0x02, 0xFF, 0xFF};
				runad[4]=(first&0xFF);
				runad[5]=((first>>8)&0xFF);
				for (i=0; i<6; i++) {buf[ret+i]=runad[i];};
				ret+=6;
			};
		};
	}
	else
	{
		ret=0;
	};
	return ret;
}
/*--------------------------------------------------------------------*/
unsigned int addPos(U8 *data, unsigned int carsize, const char *name, const char *path, U8 status)
{
static unsigned int pos=0;
	U8 buf[FLASHMAX];
	int advance=0;
	if (status)
	{
		unsigned int size=loadFile(path,buf,sizeof(buf)-8192-6);
		size=repairFile(buf,size);
		//saveRAW(buf,size);
		if (size)
		{
			unsigned int over=insertPos(name,data,carsize,pos,buf,size);
			advance=1;
			if (over) {printf("Error: \"%s\", does not fit, need %i bytes.\n",name,over); advance=0;};
		}
		else {clearPos(data,pos);};
	}
	else {clearPos(data,pos);};
	pos+=advance;
	return pos;
}
/*--------------------------------------------------------------------*/
U8 readLine(FILE *pf,char *name, char *path)
{
	U8 status=0,rb;
	char b[1];
	unsigned int i;
	for (i=0; i<NAMELEN; i++) {name[i]=0;};
	for (i=0; i<NAMELEN; i++)
	{
		b[0]=0;
		if (feof(pf)) {i=NAMELEN;}
		else
		{
			fread(b,sizeof(U8),sizeof(b),pf);
			rb=b[0];
			if (rb==0x0D)
			{
				fread(b,sizeof(U8),sizeof(b),pf);
				i=NAMELEN;
			} else
			if (rb==0x0A) {i=NAMELEN;} else
			if (rb==DELIM) {i=NAMELEN; status=1;} else{name[i]=rb;};
		};
	};
	if (status==1)
	{
		for (i=0; i<PATHLEN; i++) {path[i]=0;};
		for (i=0; i<PATHLEN; i++)
		{
			b[0]=0;
			if (feof(pf)) {i=PATHLEN;}
			else
			{
				fread(b,sizeof(U8),sizeof(b),pf);
				rb=b[0];
				if (rb==0x0D)
				{
					fread(b,sizeof(U8),sizeof(b),pf);
					i=PATHLEN;
				} else
				if (rb==0x0A) {i=PATHLEN;} else {path[i]=rb;};	
			};
		};
	};
	return status;
}
/*--------------------------------------------------------------------*/
void addData(U8 *data, unsigned int carsize, const char *filemenu)
{
	char name[NAMELEN],path[PATHLEN];
	FILE *pf;
	unsigned int i;
	pf=fopen(filemenu,"rb");
	if (pf)
	{
		for (i=0; i<26; i++)
		{
			U8 status=readLine(pf,name,path);
			if (name[0]=='#') continue;
			addPos(data,carsize,name,path,status);
		};
		for (i=0; i<27; i++)
		{
			if (data[4*i]!=0xFF) {data[4*i+2]+=0xA0;};
		};
		fclose(pf);
	}
	else
	{
		printf("Open Error \"%s\".\n",filemenu);
	};
};
/*--------------------------------------------------------------------*/
U8 saveCAR(const char *filename, U8 *data, unsigned int carsize)
{
	U8 header[16]={0x43, 0x41, 0x52, 0x54, 0x00, 0x00, 0x00, 0x2A,
		           0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00};
	U8 ret=0;
	unsigned int i,sum=0;
	FILE *pf;
	for (i=0; i<carsize; i++) {sum+=data[i];};
	header[8]=((sum>>24)&0xFF);
	header[9]=((sum>>16)&0xFF);
	header[10]=((sum>>8)&0xFF);
	header[11]=(sum&0xFF);
	pf=fopen(filename,"wb");
	if (pf)
	{
		i=fwrite(header,sizeof(U8),16,pf);
		if (i==16)
		{
			i=fwrite(data,sizeof(U8),carsize,pf);
			if (i==carsize) {ret=1;};			
		};
		fclose(pf);
	};
	return ret;
}
/*--------------------------------------------------------------------*/
void addMenu(U8 *cardata, unsigned int size, 
             U8 *loader, unsigned int loadersize, unsigned int offset)
{
	unsigned int i,j;
	for (j=0; j<(size/BANKSIZE); j++)
	{
		for (i=0; i<offset; i++) {cardata[(j+1)*BANKSIZE-offset+i]=loader[loadersize-offset+i];};
		cardata[(j+1)*BANKSIZE-offset+6]=j;
	};
	for (i=0; i<loadersize; i++) {cardata[i]=loader[i];};
}
/*--------------------------------------------------------------------*/
void addLogo(U8 *cardata, const char *logofile, unsigned int size, unsigned int nop)
{
	FILE *pf;
	unsigned int i,j;
	if (logofile)
	{
		pf=fopen(logofile,"rb");
		if (pf)
		{
			for (i=0; i<(size/nop); i++)
			{
				U8 b=0,pix[1];
				for (j=0; j<nop; j++)
				{
					fread(pix,sizeof(U8),1,pf);
					b<<=(8/nop);
					b+=pix[0];
				};
				cardata[32*4+i]=b;
			};
			fclose(pf);
		};
	};
}
/*--------------------------------------------------------------------*/
void fillData(U8 *cardata, unsigned int size, U8 byte)
{
	unsigned int i;
	for (i=0; i<size; i++) {cardata[i]=byte;};
}
/*--------------------------------------------------------------------*/
void menu4car(const char *filemenu, const char *logo, const char *carname)
{
	U8 cardata[CARMAX];
	fillData(cardata, CARMAX, 0xFF);
	addMenu(cardata,CARMAX,menu4car_bin,menu4car_bin_len,19);
	addLogo(cardata,logo,256*16,8);
	addData(cardata,FLASHMAX,filemenu);
	saveCAR(carname,cardata,CARMAX);
}
/*--------------------------------------------------------------------*/
int main( int argc, char* argv[] )
{	
	printf("Menu4CAR - ver: %s\n",__DATE__);
	if (argc==3)
	{
		menu4car(argv[1],NULL,argv[2]);
	} else
	if (argc==4)
	{
		menu4car(argv[1],argv[2],argv[3]);
	}
	else
	{
		printf("(c) GienekP\n");
		printf("use:\nmenu4car menu.txt file.car\n");
	};
	printf("\n");
	return 0;
}
/*--------------------------------------------------------------------*/
