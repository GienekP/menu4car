
/*--------------------------------------------------------------------*/
/* menu4car                                                           */
/* by GienekP                                                         */
/* (c) 2023                                                           */
/*--------------------------------------------------------------------*/
#include <stdio.h>
/*--------------------------------------------------------------------*/
typedef unsigned char U8;
/*--------------------------------------------------------------------*/
#define STARTINBANK (0x0000)
#define CARMAX (1024*1024)
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
		if ((c==0xC3) || (c==0xC4) || (c==0xC5)) // UTF-8
		{
			unsigned int utf8=c;
			utf8<<=8;
			utf8|=name[j];
			j++;
			switch (utf8)
			{
				case 0xC484: {c='A';} break; // Ą
				case 0xC486: {c='C';} break; // Ć
				case 0xC498: {c='E';} break; // Ę
				case 0xC581: {c='L';} break; // Ł
				case 0xC583: {c='N';} break; // Ń
				case 0xC393: {c='O';} break; // Ó
				case 0xC59A: {c='S';} break; // Ś
				case 0xC5B9: {c='Z';} break; // Ź
				case 0xC5BB: {c='Z';} break; // Ż
				case 0xC485: {c='a';} break; // ą
				case 0xC487: {c='c';} break; // ć
				case 0xC499: {c='e';} break; // ę
				case 0xC582: {c='l';} break; // ł
				case 0xC584: {c='n';} break; // ń
				case 0xC3B3: {c='o';} break; // ó
				case 0xC59B: {c='s';} break; // ś
				case 0xC5BA: {c='z';} break; // ź
				case 0xC5BC: {c='z';} break; // ż
				default: {c=0x20;} break;
			};	
		};
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
		start=((((bh<<7)+bl)*BANKSIZE)|(((ah<<8)|al)&0x1FFF)+0);
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
	U8 buf[512*1024];
	if (status)
	{
		unsigned int size=loadFile(path,buf,sizeof(buf)-8192-6);
		size=repairFile(buf,size);
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
	unsigned int i;
	for (i=0; i<loadersize; i++) {cardata[i]=loader[i];};
	for (i=0; i<offset; i++) {cardata[size-offset+i]=loader[loadersize-offset+i];};
}
/*--------------------------------------------------------------------*/
void fillData(U8 *cardata, unsigned int size, U8 byte)
{
	unsigned int i;
	for (i=0; i<size; i++) {cardata[i]=byte;};
}
/*--------------------------------------------------------------------*/
void menu4car(const char *filemenu, const char *carname)
{
	U8 cardata[CARMAX];
	fillData(cardata, CARMAX, 0xFF);
	addMenu(cardata,CARMAX,menu4car_bin,menu4car_bin_len,12);
	addData(cardata,512*1024,filemenu);
	saveCAR(carname,cardata,CARMAX);
}
/*--------------------------------------------------------------------*/
int main( int argc, char* argv[] )
{	
	printf("Menu4CAR - ver: %s\n",__DATE__);
	if (argc==3)
	{
		menu4car(argv[1],argv[2]);
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
