
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
#define BANKSIZE (8192)
#define PATHLEN (1024)
/*--------------------------------------------------------------------*/
#include "menu4car.h"
/*--------------------------------------------------------------------*/
U8 ATASCII2Internal(U8 a)
{
	U8 i=0;
	if (((a>=0x00) && (a<=0x1F)) || ((a>=0x80) && (a<=0x9F))) {i=(a+0x40);} else
	if (((a>=0x20) && (a<=0x5F)) || ((a>=0xa0) && (a<=0xDF))) {i=(a-0x20);} else
	{i=a;};
	return i;
}

/*--------------------------------------------------------------------*/
void fillATASCII(U8 *txt, const U8 *name, unsigned int limit)
{
	unsigned int i,j=0;
	for (i=0; i<limit; i++)
	{
		U8 c=name[j];
		j++;
		if (c==0)
		{
			c=0x20;
			i=24;
		} else
		if ((c>=' ') && (c<0x80))
		{
			switch (c)
			{
				case '|': {c=0x7C;} break;
				case '~': {c=0x7D;} break;
				case '{': {c=0x7E;} break;
				case '}': {c=0x7F;} break;
			}
		} else
		if ((c==0xC2) || (c==0xC3) || (c==0xC4) || (c==0xC5)) // UTF-8
		{
			unsigned int utf8=c;
			utf8<<=8;
			utf8|=name[j];
			j++;
			switch (utf8)
			{
				case 0xC484: {c=0x17;} break; // Ą
				case 0xC486: {c=0x16;} break; // Ć
				case 0xC498: {c=0x12;} break; // Ę
				case 0xC581: {c=0x0B;} break; // Ł
				case 0xC583: {c=0x0D;} break; // Ń
				case 0xC393: {c=0x10;} break; // Ó
				case 0xC59A: {c=0x04;} break; // Ś
				case 0xC5B9: {c=0x18;} break; // Ź
				case 0xC5BB: {c=0x00;} break; // Ż
				case 0xC485: {c=0x01;} break; // ą
				case 0xC487: {c=0x03;} break; // ć
				case 0xC499: {c=0x05;} break; // ę
				case 0xC582: {c=0x0C;} break; // ł
				case 0xC584: {c=0x0E;} break; // ń
				case 0xC3B3: {c=0x0F;} break; // ó
				case 0xC59B: {c=0x13;} break; // ś
				case 0xC5BA: {c=0x02;} break; // ź
				case 0xC5BC: {c=0x1A;} break; // ż
				case 0xC384: {c=0x19;} break; // Ä
				case 0xC38B: {c='E';}  break; // Ë
				case 0xC396: {c=0x07;} break; // Ö
				case 0xC39C: {c=0x15;} break; // Ü
				case 0xC3A4: {c=0x14;} break; // ä
				case 0xC3AB: {c='e';}  break; // ë
				case 0xC3B6: {c=0x06;} break; // ö
				case 0xC3BC: {c=0x09;} break; // ü
				case 0xC39F: {c=0x0A;} break; // ß
				case 0xC2A3: {c=0x08;} break; // £
				case 0xC2B1: {c=0x1B;} break; // ±
				default: {c=0x20;} break;
			};	
		} else
		if (c==0xE2)
		{
			unsigned int utf8=c;
			utf8<<=8;
			utf8|=name[j];
			j++;
			utf8<<=8;
			utf8|=name[j];
			j++;
			switch (utf8)
			{
				case 0xE28690: {c=0x1E;} break; // ←
				case 0xE28691: {c=0x1C;} break; // ↑
				case 0xE28692: {c=0x1F;} break; // →				
				case 0xE28693: {c=0x1D;} break; // ↓
				default: {c=0x20;} break;
			};			
		}
		else {c=0x20;};
		txt[i]=ATASCII2Internal(c);
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
		fillATASCII(&data[32*4+16*32+32*pos+6],(U8 *)name,24);
	};
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
	if ((buf[0]==0xFF) && (buf[1]==0xFF))
	{
		unsigned int a,b,start,stop;
		i+=2;
		while (i<ret)
		{
			if ((buf[i]==0xFF) && (buf[i+1]==0xFF))
			{
				for (j=i; j<ret; j++) {buf[j]=buf[j+2];};
				ret-=2;
			};
			a=buf[i]; b=buf[i+1];
			start=((b<<8)|a);
			a=buf[i+2]; b=buf[i+3];
			stop=((b<<8)|a);
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
	if (status)
	{
		unsigned int size=loadFile(path,buf,sizeof(buf)-8192-6);
		size=repairFile(buf,size);
		//saveRAW(buf,size);
		if (size)
		{
			unsigned int over=insertPos(name,data,carsize,pos,buf,size);
			if (over) {printf("Error with \"%s\", need %i bytes.\n",name,over);};
		}
		else {clearPos(data,pos);};
	}
	else {clearPos(data,pos);};
	pos++;
	return pos;
}
/*--------------------------------------------------------------------*/
U8 readLine(FILE *pf,char *name, char *path)
{
	U8 status=0,rb;
	char b[1];
	unsigned int i;
	for (i=0; i<48; i++) {name[i]=0;};
	for (i=0; i<48; i++)
	{
		b[0]=0;
		if (feof(pf)) {i=48;}
		else
		{
			fread(b,sizeof(U8),sizeof(b),pf);
			rb=b[0];
			if (rb==0x0D)
			{
				fread(b,sizeof(U8),sizeof(b),pf);
				i=48;
			} else
			if (rb==0x0A) {i=48;} else
			if (rb=='|') {i=48; status=1;} else{name[i]=rb;};	
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
	char name[48],path[PATHLEN];
	FILE *pf;
	unsigned int i;
	pf=fopen(filemenu,"rb");
	if (pf)
	{
		for (i=0; i<26; i++)
		{
			U8 status=readLine(pf,name,path);
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
	fillData(cardata, CARMAX, 0xEA);
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
