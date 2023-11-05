
/*--------------------------------------------------------------------*/
/* menu4car                                                           */
/* by GienekP                                                         */
/* (c) 2023                                                           */
/*--------------------------------------------------------------------*/
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <sysexits.h>
#include <sys/param.h>
#include <errno.h>

#include "libapultra.h"
/*--------------------------------------------------------------------*/
typedef unsigned char U8;
/*--------------------------------------------------------------------*/
#define CARMAX (1024*1024)
#define FLASHMAX (2*512*1024)
#define BANKSIZE (0x2000)
#define PATHLEN (0x400)
#define NAMELEN (0x30)
#define PARAMSLEN (8)
#define DELIM	('|')
/*--------------------------------------------------------------------*/
#include "menu4car.h"
#include "apultra/src/libapultra.h"
/*--------------------------------------------------------------------*/
int do_compress=1;
int be_verbose=0;
int errornumbers=0;
int do_bin_output=0;
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
#define PUTW(b,i,v) {b[(i)]=(v)&0xff;(b[(i)+1])=((v)>>8)&0xff;}
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
					const U8 *buf, unsigned int size,int flags)
{
	unsigned int i,ret=0;
	unsigned int start,stop;
	if (pos==0) 
	{
		start=BANKSIZE; // first adr in first bank
		stop=BANKSIZE+size;
		data[4*pos+0]=flags; // flags
		data[4*pos+1]=1; // bank
		data[4*pos+2]=0; // abs adr in bank hi (A000-based)
		data[4*pos+3]=0; // abs adr in bank lo (A000-based)
	}
	else
	{
		unsigned int bank=data[4*pos+1];
		unsigned int ah=data[4*pos+2];
		unsigned int al=data[4*pos+3];
		start=(((bank)*BANKSIZE)|(((ah<<8)|al)&0x1FFF));
		data[4*pos]=flags&0x7f; // overwrite with current
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
		data[4*pos+4]=0x80; // mark as last in advance.
		data[4*pos+5]=((stop/BANKSIZE)&0x7F);
		data[4*pos+6]=((stop>>8)&0x1F);
		data[4*pos+7]=(stop&0xFF);

		data[32*4+16*32+32*pos+3]='A'+pos-0x20;
		data[32*4+16*32+32*pos+4]='.'-0x20;
		fillATASCII(&data[32*4+16*32+32*pos+6],(U8 *)name,24);
	};

	if (be_verbose)
		printf("Adding at: $%06x: file \"%s\", length %d bytes... ",start,name,size);
	return ret;
}
/*--------------------------------------------------------------------*/
unsigned int loadFile(const char *path, U8 *buf, unsigned int sizebuf)
{
	if (path==NULL) return 0;
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
		fprintf(stderr,"Load Error \"%s\".\n",path);
		errornumbers++;
	};
	return size;
}
/*--------------------------------------------------------------------*/
/*
void saveRAW(U8 *raw, unsigned int size)
{
	static int licz=0;
	char * name[1000];
	FILE *pf;
	sprintf(name,"RAW%d.XEX",licz++);
	pf=fopen(name,"wb");
	fwrite(raw,1,size,pf);
	fclose(pf);
};
*/
/*--------------------------------------------------------------------*/
unsigned int repairFile(U8 *buf, unsigned int size)
{
	unsigned int i=0,j,first=0xFFFF,run=0,init=0,ret=size;
	if (GETW(buf,0)==0xFFFF )
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
	if (buf[0]==0xFF)
	{
		ret=0;
	};
	return ret;
}
/*--------------------------------------------------------------------*/
unsigned int compressAPLBlockByBlock(U8 *bufin, unsigned int retsize, U8 * bufout)
{
	unsigned int i=2, o=2, j;
	if (GETW(bufin,0)==0xFFFF)
	{
		PUTW(bufout,0,0xFFFF);
		unsigned int a,b,start,stop;

		while (i<retsize) // all blocks are good and repaired before
		{
			start = GETW(bufin,i);
			stop  = GETW(bufin,i+2);

			int initrun=(start<=0x2e2) && (stop>=0x2e2);
			PUTW(bufout,o,start);

			int tsize=(1+stop-start);
			int csize=0;

			i+=4;
			o+=4;

			if (tsize>=32) {
				for (j=o; j<MIN(o+tsize,FLASHMAX); j++) {bufout[j]=0;};

				csize= apultra_compress(&bufin[i],
						&bufout[o],
						tsize,
						FLASHMAX,
						0,
						0,
						0, 
						NULL,
						NULL);
				//saveRAW(&bufout[o],csize);
			}
			if (tsize>=32 && csize<tsize && !initrun) {
				PUTW(bufout,o-2,0);
				o+=csize;
				// ok, compressed in place
			}
			else { // copy tsize bytes; also sometimes overwrite with original if compressed is longer
				PUTW(bufout,o-2,stop);
				for (j=i; j<i+tsize; j++) {bufout[o++]=bufin[j];};
			}
			i+=tsize;
		};
		retsize=o;
	}
	else
		retsize=0;
	return retsize;
}
/*--------------------------------------------------------------------*/
void process_types(const char * path, int * flags) {
	// FFFF xex
	// nagl ATR || ext atr - ATR
	// nagl CART type 8kb || ext bin || ext car- CART
	// nagl BASIC? - ext BAS
}
/*--------------------------------------------------------------------*/
void process_inline_params(const char * addparams) {
	int i=0;
	// default values
	do_compress=default_do_compress;

	while (i<strlen(addparams))
	{
		if (addparams[i]=='c')  {
			i++;
			switch (addparams[i]) {
				case 'a': // auto
					do_compress=-1; 
					break;
				case '0':
				case '1':
				case '2':
					do_compress=addparams[i]-'0';
					break;
				case 0:
					fprintf(stderr,"Error: truncated option c.\n");
					break;
				default:
					fprintf(stderr,"Error: option c%c.\n",addparams[i]);
					break;
			}
		}
		i++;
	} 
}
unsigned int addPos(U8 *data, unsigned int carsize, const char *name, const char *path, const char *addparams,  U8 status)
{
static unsigned int pos=0;
	U8 buf[FLASHMAX];
	U8 bufcompr[FLASHMAX];
	U8 bufcompr2[FLASHMAX];
	int advance=0;
	static int osize=0;
	static int ncsize=0;
	int flags=0;

	if (data==NULL) {
		if (be_verbose) {
			printf("SUMMARY:\n");
			printf("Processed %d file entries.\n",pos);
			printf("Spotted %d errors.\n",errornumbers);
			//if (do_compress) {
			printf("Cartridge size: %d/%06x\n",carsize,carsize);
			printf("Cartridge data section size: %d/%06x\n",carsize-BANKSIZE,carsize-BANKSIZE);
			printf("Overall file size before compressed: %d/0x%06x\n",ncsize,ncsize);
			printf("Summary size taken by binaries: %d/0x%06x\n",osize,osize);
			printf("Compression ratio: %d%%\n",osize*100/ncsize);
			//}
			printf("Cartridge fill: %d%%\n",((osize+1)*200)/(2*(carsize-BANKSIZE)));
		}
		return 0;
	}

	process_inline_params(addparams);
	process_types(path,&flags);

	if (status)
	{
		unsigned int size=loadFile(path,buf,sizeof(buf)-BANKSIZE-6);
		size=repairFile(buf,size);
		// compress file, get new size.
		/**
 * Compress memory
 *
 * @param pInputData pointer to input(source) data to compress
 * @param pOutBuffer buffer for compressed data
 * @param nInputSize input(source) size in bytes
 * @param nMaxOutBufferSize maximum capacity of compression buffer
 * @param nFlags compression flags (set to 0)
 * @param nMaxWindowSize maximum window size to use (0 for default)
 * @param nDictionarySize size of dictionary in front of input data (0 for none)
 * @param progress progress function, called after compressing each block, or NULL for none
 * @param pStats pointer to compression stats that are filled if this function is successful, or NULL
 *
 * @return actual compressed size, or -1 for error
 */
		if (size) {
			int comprsize=0;
			int choosen_compress_method=0;
			if (do_compress==-1 || do_compress==1) {
				comprsize= apultra_compress(buf,
						bufcompr,
						size,
						sizeof(bufcompr),
						0,
						255,//256 - cycle buffer size as well as compression window, one byte less works
						0, 
						NULL,
						NULL);
				choosen_compress_method=1;
			}
			if (do_compress==-1 || do_compress==2) {
				// block compression, to do.
				int comprsize2=compressAPLBlockByBlock(buf,size,bufcompr2);

				if (comprsize2<comprsize || do_compress>0)
				{
					int j;
					for (j=0; j<comprsize2; j++) {bufcompr[j]=bufcompr2[j];};
					comprsize=comprsize2;
					choosen_compress_method=2;

				}
			}

			//saveRAW(buf,size);
			if (do_compress && ((comprsize < size) || do_compress>=1)) // forced 
			{
				
				flags|=((choosen_compress_method)<<4);
				unsigned int over=insertPos(name,data,carsize,pos,bufcompr,comprsize,flags);
				advance=1;
				if (over) {
					if (be_verbose)
						printf("skipped: \"%s\", does not fit, need %i bytes.\n",name,over); advance=0;
				}
				else {
					if (be_verbose)
						printf("compressed: \"%s\", method %d, length (compr/uncompr): %d/%d, ratio %d%%\n",name,choosen_compress_method,comprsize,size,comprsize*100/size);
					osize+=comprsize;
					ncsize+=size;
				}


			}
			else if ((comprsize >= size)||!do_compress)
			{
				unsigned int over=insertPos(name,data,carsize,pos,buf,size,flags);
				advance=1;
				if (over) {
					if (be_verbose)
						printf("skipped: \"%s\", does not fit, need %i bytes.\n",name,over); advance=0;
				}
				else	{
					if (be_verbose)
						printf("added without compression.\n");
					osize+=size;
					ncsize+=size;
				}
			}
			else
			{clearPos(data,pos);};
		}
	}
	else {clearPos(data,pos);};
	pos+=advance;
	return pos;
}
/*--------------------------------------------------------------------*/
U8 readLine(FILE *pf,char *name, char *path, char *add)
{
	U8 status=0,rb;
	char b[1];
	unsigned int i;
	for (i=0; i<NAMELEN; i++) {name[i]=0;};
	for (i=0; i<PATHLEN; i++) {path[i]=0;};
	for (i=0; i<PARAMSLEN; i++) {add[i]=0;};

	for (i=0; i<NAMELEN-1; i++)
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
				i=NAMELEN; // exit from loop
			} else
			if (rb==0x0A) {i=NAMELEN;} else
			if (rb==DELIM) {i=NAMELEN; status=1;}
			else{name[i]=rb;};
		};
	};
	if (status==1)
	{
		for (i=0; i<PATHLEN-1; i++)
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
					i=PATHLEN; // exit from loop
				} else
				if (rb==0x0A) {i=PATHLEN;} else
				if (rb==DELIM) {i=PATHLEN; status=2;} else
				{path[i]=rb;};
			};
		};
	};
	if (status==2)
	{
		for (i=0; i<PARAMSLEN-1; i++)
		{
			b[0]=0;
			if (feof(pf)) {i=PARAMSLEN;}
			else
			{
				fread(b,sizeof(U8),sizeof(b),pf);
				rb=b[0];
				if (rb==0x0D)
				{
					fread(b,sizeof(U8),sizeof(b),pf);
					i=PARAMSLEN;
				} else
				if (rb==0x0A) {i=PARAMSLEN;}
				else
				{add[i]=rb;};	
			};
		};
	};
	return status;
}
/*--------------------------------------------------------------------*/
void addData(U8 *data, unsigned int carsize, const char *filemenu)
{
	char name[NAMELEN],path[PATHLEN],addparams[PARAMSLEN];
	FILE *pf;
	unsigned int i;
	pf=fopen(filemenu,"rb");
	if (pf)
	{
		i=0;
		while (i<26)
		{
			U8 status=readLine(pf,name,path,addparams);
			if (strlen(path)>0 && strlen(name)>0) {
				if (be_verbose)
					printf("Line read:'%s','%s','%s'\n",name,path,addparams);
				if (name[0]=='#')
					continue;
				addPos(data,carsize,name,path,addparams,status);
				i++;
			}
			else
				break;
		};
		addPos(0,carsize,0,0,0,0);
		for (i=0; i<27; i++)
		{
			if (data[4*i]!=0xFF) {data[4*i+2]+=0xA0;};
		};
		fclose(pf);
	}
	else
	{
		fprintf(stderr,"Open Error \"%s\".\n",filemenu);
	};
};
/*--------------------------------------------------------------------*/
U8 saveCAR(const char *filename, U8 *data, unsigned int carsize)
{
	U8 header[16]={0x43, 0x41, 0x52, 0x54, 0x00, 0x00, 0x00, 0x2A,
		           0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00};

	if (filename==NULL)
		fprintf(stderr,"Warning: -o not provided, no output generated.\n");

	U8 ret=0;
	unsigned int i,j,sum=0;
	FILE *pf;
	if (!do_bin_output) {
		for (i=0; i<carsize; i++) { sum+=data[i];};
		header[8]=((sum>>24)&0xFF);
		header[9]=((sum>>16)&0xFF);
		header[10]=((sum>>8)&0xFF);
		header[11]=(sum&0xFF);
		if (be_verbose)
			printf("Cartridge CRC Checksum: %02x%02x%02x%02x\n",header[8],header[9],header[10],header[11]);
	}
	pf=fopen(filename,"wb");
	if (pf)
	{
		j=0;
		if (!do_bin_output) {
			j=fwrite(header,sizeof(U8),16,pf);
			if (j!=16)
				fprintf(stderr,"Error: Cartridge image '%s' truncated (%d bytes written)\n",filename, j);
		}

		if (j==16 || do_bin_output )
		{
			i=fwrite(data,sizeof(U8),carsize,pf);
			if (i==carsize) {
				if (be_verbose)
					printf("Cartridge image '%s' saved (%d bytes)\n",filename, i+j);
				ret=1;
				}
			else
			{
				fprintf(stderr,"Error: Cartridge image '%s' truncated (%d bytes written)\n",filename, i+j);

			}

		};
		fclose(pf);
	}
	else {
		fprintf(stderr,"Error opening file '%s': %s \n",filename, strerror(errno));
		fprintf(stderr,"Cartridge image file write failed.\n");
	}

	return ret;
}
/*--------------------------------------------------------------------*/
void addMenu(U8 *cardata, unsigned int size, 
             U8 *loader, unsigned int loadersize, unsigned int offset)
{
	unsigned int i,j;
	// fill bootstrap on every empty block
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
void addFont(U8 * cardata, const char * fontpath)
{
	loadFile(fontpath,&cardata[0x800],1024);
}
/*--------------------------------------------------------------------*/
void addCTable(U8 * cardata, const char * colortablefile)
{
	loadFile(colortablefile,&cardata[0x7F0],16);
}
/*--------------------------------------------------------------------*/
void menu4car(const char * filemenu, const char * logo, const char * colortablefile, const char * fontpath, const char * carname, int cart_size, int default_do_compress)
{
	U8 cardata[CARMAX];
	fillData(cardata, CARMAX, 0xFF);
	addMenu(cardata,CARMAX,menu4car_bin,menu4car_bin_len,19);
	addLogo(cardata,logo,256*16,8);
	addCTable(cardata,colortablefile);
	addFont(cardata,fontpath);
	addData(cardata,cart_size,filemenu);
	saveCAR(carname,cardata,cart_size);
}
void usage() {
		printf("Menu4CAR - ver: %s\n",__DATE__);
		printf("(c) GienekP\n\n");
		printf("usage:\nmenu4car menu.txt <options>\n");
		printf("\nOptions:\n");
		printf("	-p <path> - picdata path (default Menu4Car, built in)\n");
		printf("	-t <path> - color table path (default rainbow, built in)\n");
		printf("	-o <path> - outputcar path\n");
		printf("	-b <path> - output binary image path\n");
		printf("	-c <compression> - forced compression method 0/1/2/a, (default 'a'uto) like in lines, in lines have priority over this)\n");
		printf("	-f <path> - font path\n");
		printf("	-s <size> - cart size: 32/64/128/256/512/1024, default 1024\n");
		printf("	-v - be verbose\n");
		printf("	-? - this help\n\n");
		exit(EX_USAGE);
}
/*--------------------------------------------------------------------*/
int main( int argc, char* argv[] )
{	
	if (argc<=1)
	{
		usage();
	};

	char * logofilepath=NULL;
	char * colortablefile=NULL;
	char * outfile=NULL;
	char * fontpath=NULL;
	int  cart_size=1024*1024;
	int default_do_compress=-1;
	char * txtfilename=NULL;
	int i;

	i=1;
	while (i<argc)
	{
		if (argv[i][0]=='-') {
			if  (strlen(argv[i])==2) {
				int has_val=i<(argc-1);
				//printf("arg: %c\n",argv[i][1]);
				switch (argv[i][1]) {
					case 'p':
						if (has_val)
							logofilepath=argv[++i];
						else
							usage();
						break;
					case 't':
						if (has_val)
							colortablefile=argv[++i];
						else
							usage();
						break;
					case 'b':
						if (has_val)
							do_bin_output=1;
						// skipped break
					case 'o':
						if (has_val)
							outfile=argv[++i];
						else
							usage();
						break;
					case 'c':
						if (has_val) {
							i++;
							if (strlen(argv[i])==1) {
								switch (argv[i][0]) {
									case 'a':
										default_do_compress=-1;
										break;
									case '0':
									case '1':
									case '2':
										default_do_compress=argv[i][0]-'0';
										break;
									default:
										usage();
								}
							}
						} else usage();

						break;
					case 'f':
						if (has_val)
							fontpath=argv[++i];
						else
							usage();

						//printf("Font path: %s\n",fontpath);

						break;
					case 's':
						if (has_val) {
							int s;
							i++;
							int tab[]={32,64,128,256,512,1024};
							for (s=0; s<6; s++){
								char test[10];
								sprintf(test,"%d",tab[s]);

								if (strcmp(test,argv[i])==0) {
									cart_size=strtol(argv[i],NULL,10)*1024;
									//printf("Cart size: %d\n",cart_size);
									break;
								}
							}
							if (s==6) usage();
						}
						else
							usage();
						break;

					case 'v': 
						be_verbose=1;
						break;
					default:
						usage();
						break;
				}
			} else
				usage();

		}
		else
			txtfilename=argv[i];
		i++;
	}

	if (txtfilename)
		menu4car(txtfilename,logofilepath, colortablefile, fontpath, outfile, cart_size, default_do_compress);
	if (errornumbers>0)
		fprintf(stderr,"Warning: %d input file errors encountered.\n",errornumbers);

	return 0;
}
/*--------------------------------------------------------------------*/
