/*--------------------------------------------------------------------*/
/* menu4car                                                           */
/* GienekP & jhusak                                                   */
/* (c) 2023                                                           */
/*--------------------------------------------------------------------*/
//#define SAVERAW
#if defined(__MINGW32__) || defined(__MINGW64__)
#define __MINGW__
#endif

#include <stdio.h>
#include <ctype.h>
#include <string.h>

#if !defined(__MINGW__)
#include <sysexits.h>
#endif

#include <sys/param.h>
#include <errno.h>

#include "libapultra.h"
#include "ZX0/src/zx0.h"
#define MAX_OFFSET_ZX0    32640
#define MAX_OFFSET_ZX7     2176

#define COMPRESS_APL	1
#define COMPRESS_ZX0	2
/*--------------------------------------------------------------------*/
typedef unsigned char U8;
/*--------------------------------------------------------------------*/
#define RAM_MENU_SPACE	4096	// space for compressed menu entries

#define FLASHMAX (2*512*1024)
#define BANKSIZE (0x2000)
#define PATHLEN (0x400)
#define NAMELEN (0x100)
#define PARAMSLEN (8)
#define DELIM	('|')
#define MAX_PAGES	6
#define MAX_ENTRIES	(MAX_PAGES*26)
#define MAX_ENTRIES_1	(MAX_ENTRIES+1)

#if ! defined(MIN)
#define	MIN(a,b)	((a)>(b)?(b):(a))
#endif

// those types correspond to .asm file ; do not change
// bits 6,7(1-based) determine compression algorithm used
// when TYPE_XEX.
#define	TYPE_XEX	0b00000000
#define TYPE_MASK_XEX	0b10000000
#define TYPE_MASK	0b11100000
#define	TYPE_BOOT	0b10000000
#define	TYPE_ATR	0b10100000
#define TYPE_BAS	0b11000000
#define TYPE_CAR	0b11100000


// this is for another use, for output without car header
#define TYPE_BIN	20
#define TYPE_UNKNOWN	-1

// provided from .asm mads compile
#include "menu4car_interface_ram.h"
#include "menu4car_interface_flash.h"
#include "flashgenerator/flashgenerator.h"
#include "flashgenerator/menu4car_templateflasher.h"
/*--------------------------------------------------------------------*/
#include "ramdata.h"
#include "menu4car.h"
#include "apultra/src/libapultra.h"
/*--------------------------------------------------------------------*/
int do_compress=1;
int be_verbose=0;
int errorcounter=0;
int skipcounter=0;
int do_bin_output=0;
int do_analyze=0;
int default_do_compress=-1;
int cartsizetab[]={32,64,128,256,512,1024};
int xex_compress=0;
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

/*--------------------------------------------------------------------*/
void fillData(U8 *cardata, unsigned int size, U8 byte)
{
	unsigned int i;
	for (i=0; i<size; i++) {cardata[i]=byte;};
}

/*--------------------------------------------------------------------*/
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
#define DAOFFS(i,f) (DATAARRAY_OFFSET+(i)+(f)*MAX_ENTRIES_1)
#define GETADDR(_data,_pos)	(((_data[DAOFFS(_pos,0)]&0x7f)*BANKSIZE)|(((_data[DAOFFS(_pos,2)]<<8)|_data[DAOFFS(_pos,1)])&0x1FFF))
#define GETBANK(_data,_pos)	(_data[DAOFFS(_pos,0)]&0x7f)
#define BANK(_data,_pos)	(_data[DAOFFS(_pos,0)])
#define GETTYPE(_data,_pos)	(_data[DAOFFS(_pos,2)]&TYPE_MASK)
#define IS_LAST(_data,_pos)	(_data[DAOFFS(_pos,0)]&0x80)
#define SETMETADATA(_data,_pos,_flags,_bank,_adr,_tpos) {\
	data[DAOFFS(_pos,0)]=_bank;\
	data[DAOFFS(_pos,1)]=((_adr)&0xFF);\
	data[DAOFFS(_pos,2)]=(((_adr)>>8)&0x1F)|((_flags)&TYPE_MASK);\
	data[DAOFFS(_pos,3)]=_tpos;\
	}
int findLastCarPos(U8 * data){
	int lpos=0;
	int last=-1;
	while (lpos<MAX_ENTRIES)
	{
		if (GETTYPE(data,lpos)==TYPE_CAR) // bank
			last=lpos;
		lpos++;
	}
	return last; // here returns "no room"
}
/*--------------------------------------------------------------------*/
void outTable(U8 * data) {

	int i=0;
	while (i<MAX_ENTRIES) {
		int start=GETADDR(data,i);
		printf("Data: pos: %d, flags: %02x, bank: %02x, addr: %06x, pos: %d\n",i,GETTYPE(data,i),GETBANK(data,i),start,data[DAOFFS(i,3)]);
		if (IS_LAST(data,i)) break; // flags
		i++;
	}
}

/*--------------------------------------------------------------------*/
int getRoomFor8kBCart(U8 * data,int carsize, int start,int ipos, const U8 * cbuf)
{
	// 1. move data one bank up.
	// get start addr
	int stop=start;

	// get last stop addr
	int i=0;
	while (i<MAX_ENTRIES){
		if (IS_LAST(data,i)) {
			stop=GETADDR(data,i);
			if (be_verbose>=2) {
				printf("Found end at pos %d, value: %06x\n",i,stop);
				break;
			}
		}
		i++;
	}

	// now copy data 8kB up
	//if (stop==0) stop=start;
	//printf("Moving 8kB to free place for cart, size: %04x, %06x-%06x to %06x-%06x\n",stop-start,start,stop,start+0x2000,stop+0x2000);
	//outTable(data);
	i=0;
	if (stop>carsize-0x2000) {
		return 1; // no room, return with error
	}
	printf("Cart image stored at: %06x\n",start);

	for (int k=stop-1; k>=start; k--) {
		data[k+0x2000]=data[k];
		data[k]=0;
		//printf("%06x ",k);
	}
	for (i=0; i<0x2000; i++) {data[start+i]=cbuf[i];};

	// also moving entries and store under current
	i=MAX_ENTRIES-1;
	while (i>=ipos) {
		for (int j=0; j<4; j++)
			data[DAOFFS(i+1,j)]=data[DAOFFS(i,j)];
		i--;
	}

	i=0;
	// 2. update pos chain by adding 1 to banks of not carts
	while (i<MAX_ENTRIES_1)
	{
		if (GETTYPE(data,i)!=TYPE_CAR)
			BANK(data,i)++; // inc bank and mark free 8kB

		if (IS_LAST(data,i)) break;
		i++;
	}
	return 0; // ok.
}
/*--------------------------------------------------------------------*/
unsigned int insertPos(const char *name, U8 *data, U8 *ramdata, unsigned int carsize, unsigned int pos,
					const U8 *buf, unsigned int size,int flags, int compmeth)
{
	unsigned int i,ret=0;
	unsigned int start,stop;
	int SC_POS_OFFSET=SCREENDATA_OFFSET+32*pos;

	if (pos==0) 
	{	// init first entry as end marker
		SETMETADATA(data,pos,0,0x81,0x0000,0);
	}
		
	start=GETADDR(data,pos);
	stop=(start+size);
	if (be_verbose>=2)
		printf("Pos: %d, file start: %06x, stop: %06x, packer: %d\n",pos,start,stop,compmeth);

	if (stop<=carsize) {// if fits


		// if is CAR then copy slot to next before setting
		// on the asm side there will be two POSes, one for START
		// and second for STOP
		if (flags==TYPE_CAR) { // move the last to next slot
			// insert car data:
			// find last cart slot and get the bank, or -1 if not found any
			int lcartpos=findLastCarPos(data);
			// lcartpos = -1 for 0, 0 for 1 etc.
			int lcartstart=0; // by default
			int lcartbank=lcartpos>=0?GETBANK(data,lcartpos):0;
			// lcartbank = 0 when no carts, 1 when one cart etc
			//if (lcartpos>=0) 
			lcartstart=GETADDR(data,lcartpos);
			lcartstart+=BANKSIZE;

			//printf("lcartpos: %d lcartbank: %d lcartstart: %06x\n",lcartpos,lcartbank,lcartstart);
			//outTable(data);

			if (0==getRoomFor8kBCart(data,carsize,lcartstart,lcartbank,buf))
			{
				SETMETADATA(data,lcartbank,flags,lcartbank+1,lcartstart,pos);
				//move last entry one pos up
				SETMETADATA(data,pos+1,0,((stop/BANKSIZE)&0x7F)|0x80,stop,0);
			}
			else {
				//printf("Cart image '%s' not added due to insufficient room.\n",name);
				skipcounter++;
				ret=stop-carsize;
				stop=start;
			}
		}
		else if ((flags & TYPE_MASK_XEX)==TYPE_XEX)
		{
			// update "last" flags - stop becomes start
			data[DAOFFS(pos,0)]&=0x7f;
			data[DAOFFS(pos,2)]&=~TYPE_MASK;
			data[DAOFFS(pos,2)]|=flags|((compmeth&3)<<5);
			data[DAOFFS(pos,3)]=pos;

			// append with data
			for (i=0; i<size; i++) {data[start+i]=buf[i];};
			// mark as last in advance.
			SETMETADATA(data,pos+1,0,((stop/BANKSIZE)&0x7F)|0x80,stop,0);
		}

		ramdata[SC_POS_OFFSET+3]='A'+(pos%26)-0x20;
		ramdata[SC_POS_OFFSET+4]='.'-0x20;
		fillATASCII(&ramdata[SC_POS_OFFSET+6],(U8 *)name,24);
	}
	else
	{
		//printf("File '%s' not added due to insufficient room.\n",name);
		skipcounter++;
		ret=stop-carsize;
		stop=start;
	}

	//
	if (!ret) {
		if (be_verbose)
			printf("SUCCESS");
		if (be_verbose>=2)
			printf(", added, length %d.",size);
		if (be_verbose)
			printf("\n");
	}
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
		errorcounter++;
		return -1;
	};
	return size;
}
/*--------------------------------------------------------------------*/
void saveFile(const char * tname, U8 *raw, unsigned int size)
{
	static int licz=0;
	char name[1000];
	FILE *pf;
	if (tname==NULL)
		sprintf(name,"RAW%d.XEX",licz++);
	else
		sprintf(name,"%s",tname);

	pf=fopen(name,"wb");
	fwrite(raw,1,size,pf);
	fclose(pf);
};
/*--------------------------------------------------------------------*/
int repairFile(U8 *buf, int size)
{
	int i=0,j,first=0xFFFF,run=0,init=0,ret=size;

#define BYTES_IN_BUF(i,ret) ((ret)-(i))

	
	if ((ret>2) && GETW(buf,0)==0xFFFF )
	{
		if (be_verbose>=3) fprintf(stderr,"Atari binary file header read: 0xff 0xff\n");
		unsigned int a,b,start,stop;
		i+=2;
		while (BYTES_IN_BUF(i,ret)>=4)
		{
			while (BYTES_IN_BUF(i,ret)>=4) {
				if  (GETW(buf,i)==0xFFFF)
				{
					if (be_verbose>=3) fprintf(stderr,"uneven binary file header read: 0xff 0xf; removing.\n");
					for (j=i; j<ret; j++) {buf[j]=buf[j+2];};
					ret-=2;
				}
				else
					break;
			}

			if (BYTES_IN_BUF(i,ret)>=4) {
				start = GETW(buf,i);
				if (be_verbose>=3) fprintf(stderr,"BLOCK: %04x-",start);
				stop  = GETW(buf,i+2);
				if (be_verbose>=3) fprintf(stderr,"%04x\n",stop);
				i+=4;
			}
			else
				return 0;

			if (start>stop)
				return 0;

			if (BYTES_IN_BUF(i,ret)<1+stop-start) return 0;
			if (first==0xFFFF) {first=start;};
			if ((start<=0x02E0) && (stop>=0x02E1)) { run=1; };
			if ((start<=0x02E2) && (stop>=0x02E3)) {init=1;};
			i+=(1+stop-start);
		};
		if (BYTES_IN_BUF(i,ret)>0) return 0;

		if (!init && !run)
		{
			if (be_verbose>=3) fprintf(stderr,"Added not-existent RUN: %04x\n",first);
			U8 runad[6]={0xE0, 0x02, 0xE1, 0x02, 0xFF, 0xFF};
			runad[4]=(first&0xFF);
			runad[5]=((first>>8)&0xFF);
			for (i=0; i<6; i++) {buf[ret+i]=runad[i];};
			ret+=6;
			return ret;
		};
	}
	else
		return 0;

	return ret;
}
/*--------------------------------------------------------------------*/
unsigned int compressBlockByBlock(int comprmethod, U8 *bufin, unsigned int retsize, U8 * bufout)
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
			if (be_verbose>=3)
				printf("Block: %04x-%04x, len %04x\n",start,stop, stop-start+1);

			int initrun=(start<=0x2e2) && (stop>=0x2e2);
			PUTW(bufout,o,start);

			int tsize=(1+stop-start);
			int csize=0;

			i+=4;
			o+=4;

#define COMPRESSION_THRESHOLD	32
			if (tsize>=COMPRESSION_THRESHOLD) {
				for (j=o; j<MIN(o+tsize,FLASHMAX); j++) {bufout[j]=0;};
				switch(comprmethod) {
					case COMPRESS_APL:
						csize= apultra_compress(&bufin[i],
								&bufout[o],
								tsize,
								FLASHMAX,
								0,
								255,
								0, 
								NULL,
								NULL);
						break;
					case COMPRESS_ZX0:
						{
							int delta;
							int quick_mode=1;

							unsigned char * output_data = compress(
									optimize(&bufin[i], tsize, 0, quick_mode ? MAX_OFFSET_ZX7 : MAX_OFFSET_ZX0),
									&bufin[i], tsize,
									0, 0, 1,
									&csize, &delta
									);
							for (int c=0; c<csize; c++) bufout[o+c]=output_data[c];
							free(output_data);
							//printf("Compress: %d to %d, offset %04x\n",tsize,csize,o);
						}
						break;
				}
			}
			if (tsize>=COMPRESSION_THRESHOLD && csize<tsize && !initrun) {
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
int checkTypeByPath(const char * filename) {
#define TSTEXT(f,a) strcasecmp(&(f)[strlen(f)-4],(a))==0

	if (TSTEXT(filename,".car")) return TYPE_CAR;
	if (TSTEXT(filename,".bin")) return TYPE_BIN;
	if (TSTEXT(filename,".bas")) return TYPE_BAS;
	if (TSTEXT(filename,".xex")) return TYPE_XEX;
	if (TSTEXT(filename,".exe")) return TYPE_XEX;
	if (TSTEXT(filename,".obx")) return TYPE_XEX;
	if (TSTEXT(filename,".com")) return TYPE_XEX;
	if (TSTEXT(filename,".atr")) return TYPE_ATR;
	if (TSTEXT(filename,".bot")) return TYPE_BOOT;
	return TYPE_UNKNOWN;
}


/*--------------------------------------------------------------------*/
void process_input_types(const char * path, int * flags) {
/*
		0->	LOADXEX
		1->	LOADBOOT
		2->	LOADATR
		3->	LOADBASIC
		4->	LOADCAR
*/
	*flags=checkTypeByPath(path);
	if (*flags>=20) *flags=TYPE_UNKNOWN;
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
				case '3':
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

unsigned int addPos(U8 *data, U8 *ramdata, unsigned int carsize, const char *name, const char *path, const char *addparams)
{
	static unsigned int pos=0;

	static U8 bufplain[FLASHMAX];
	static U8 bufcompr[FLASHMAX];
	static U8 bufcompr2[FLASHMAX];

	int advance=0;
	static int osize=0;
	static int ncsize=0;
	int flags=0;

	fillData(bufplain,FLASHMAX,0);
	fillData(bufcompr,FLASHMAX,0);
	fillData(bufcompr2,FLASHMAX,0);

	if (data==NULL) {
		if (be_verbose) {
			printf("SUMMARY:\n");
			printf("Processed %d file entries.\n",pos);
			printf("Spotted %d errors.\n",errorcounter);
			printf("Skipped %d files.\n",skipcounter);
			if (default_do_compress!=0) {
				printf("Cartridge size: %d/0x%06x\n",carsize,carsize);
				printf("Cartridge data section size: %d/%06x\n",carsize-BANKSIZE,carsize-BANKSIZE);
				printf("Overall file size before compressed: %d/0x%06x\n",ncsize,ncsize);
				printf("Summary size taken by binaries: %d/0x%06x\n",osize,osize);
				printf("Compression ratio: %d%%\n",osize*100/ncsize);
			}
			printf("Cartridge fill: %d%%\n",((osize+1)*200)/(2*(carsize-BANKSIZE)));
			printf("Free Bytes left: %d\n",carsize-BANKSIZE-osize);
		}
		return 0;
	}

	process_inline_params(addparams);
	process_input_types(path,&flags);

	if (flags==TYPE_UNKNOWN || flags==TYPE_BIN) return pos;

	// simplest way that compiles everywhere
	FILE * fd=fopen(path,"rb");
	if (!fd) return pos;

	fseek(fd, 0L, SEEK_END);
	int length = ftell(fd);
	fclose(fd);

	unsigned int size=loadFile(path,bufplain,MIN(sizeof(bufplain)-BANKSIZE-6,length));
	if (size<length) return pos;

	if (be_verbose)
		printf("%s length: %d ",path,size);
	int filetype=checkTypeByPath(path);
	if (filetype==TYPE_XEX) {
		if (be_verbose)
			printf("type XEX... ");
		size=repairFile(bufplain,size);
		// compress file, get new size.
		if (size) {
			int comprsize=0;
			int choosen_compress_method=0;
			if (do_compress==-1 || do_compress==1) {
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
				comprsize= apultra_compress(bufplain,
						bufcompr,
						size,
						sizeof(bufcompr),
						0,
						250,//256 - cycle buffer size as well as compression window, one byte less works
						0, 
						NULL,
						NULL);
				choosen_compress_method=1;
			}
			if (do_compress==-1 || do_compress==2) {

				int comprsize2=compressBlockByBlock(COMPRESS_APL,bufplain,size,bufcompr2);

				if (comprsize2<comprsize || do_compress>0)
				{
					int j;
					for (j=0; j<comprsize2; j++) {bufcompr[j]=bufcompr2[j];};
					comprsize=comprsize2;
					choosen_compress_method=2;

				}
			}
			if (do_compress==-1 || do_compress==3) {

				int comprsize2=compressBlockByBlock(COMPRESS_ZX0,bufplain,size,bufcompr2);

				if (comprsize2<comprsize || do_compress>0)
				{
					int j;
					for (j=0; j<comprsize2; j++) {bufcompr[j]=bufcompr2[j];};
					comprsize=comprsize2;
					choosen_compress_method=3;

				}
			}

#ifdef SAVERAW
			//saveFile(bufplain,size);
			if (!do_compress)
				saveFile(name,bufplain,size);
			else
				saveFile(name,bufcompr,comprsize);
#endif
			int over=0;
			int incrsize=0;
			if (do_compress && ((comprsize < size) || do_compress>=1)) // forced 
			{
				//flags|=choosen_compress_method<<5;
				over=insertPos(name,data,ramdata,carsize,pos,bufcompr,comprsize,flags,choosen_compress_method);
				incrsize=comprsize;
			}
			else if ((comprsize >= size)||!do_compress)
			{
				over=insertPos(name,data,ramdata,carsize,pos,bufplain,size,flags,0);
				incrsize=size;
			}

			if (over) {
				if (be_verbose) {
					printf("SKIPPED");
					if (be_verbose>=2)
						printf(", \"%s\", does not fit, need %i bytes.",name,over);
					printf("\n");
				}
			}
			else {
				if (0)
					if (be_verbose) {
						printf("SUCCESS!");
						if (be_verbose>=2) {
							if (incrsize==size)
							{
								printf(", added, length: %d",incrsize);
							}
							else
							{
								printf(", compressed with method %02x, length (compr/uncompr): %d/%d, ratio %d%%",choosen_compress_method,comprsize,size,comprsize*100/size);
							}
						}
						printf("\n");
					}
				osize+=incrsize;
				ncsize+=size;
				advance=1;
			}
		}
		else {
			fprintf(stderr,"Error in xex file '%s'\n",path);
			printf("\n");
			errorcounter++;
		}
	}
	else if (filetype==TYPE_CAR)
	{
		if (be_verbose)
			printf("type CAR... ");
		switch (size){
			case 0x410:
				for (int i=0x10; i<0x410; i++) {bufplain[0x400+i]=bufplain[i];};
				size+=0x400;
				// no break;
			case (0x810):
				for (int i=0x10; i<0x810; i++) {bufplain[0x800+i]=bufplain[i];};
				size+=0x800;
				// no break;
			case (0x1010):
				for (int i=0x10; i<0x1010; i++) {bufplain[0x1000+i]=bufplain[i];};
				size+=0x1000;
				// no break;
			case (0x2010):
				{
					unsigned int over=insertPos(name,data,ramdata,carsize,pos,&bufplain[16],0x2000,flags,0);
					if (over){
						if (be_verbose)
							printf("SKIPPED: \"%s\", does not fit, need %i bytes.\n",name,over);
					} else {
						osize+=size&0xf800;
						ncsize+=size&0xf800;
						advance=1;
					}
				}
				break;
			default:
				printf("ERROR: \"%s\", only <=8k cartridges are handled (size: %04x)\n",name,size);
				errorcounter++;
		}
	}
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
			fread(b,sizeof(U8),sizeof(b),pf); // read one byte
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
int addData(U8 *data, U8 *ramdata, unsigned int carsize, const char *filemenu)
{
	char name[NAMELEN],path[PATHLEN],addparams[PARAMSLEN];
	FILE *pf;
	unsigned int i;
	pf=fopen(filemenu,"rb");
	if (pf)
	{
		i=0;
		int o=0;
		while (i<MAX_ENTRIES && o<200)
		{
			U8 status=readLine(pf,name,path,addparams);
			if (strlen(path)>0 && strlen(name)>0) {
				if (be_verbose>=2)
					printf("Line read num: %d, '%s','%s','%s'\n",i,name,path,addparams);
				if (name[0]=='#')
					continue;
				if (status) 
					i=addPos(data,ramdata,carsize,name,path,addparams);
				//outTable(data);
			}
			else
			if (strlen(path)==0 && strlen(name)==0) {
				break;
			}
			else if (strlen(path)==0 || strlen(name)==0) {
				fprintf(stderr,"BAD Line read num: %d, '%s','%s','%s'\n",i,name,path,addparams);
				errorcounter++;
			}
			if (i>0) o++; // begin counting after some files added
		};
		// output summary info
		addPos(0,0,carsize,0,0,0);
		int j;
		//for (j=0; j<MAX_ENTRIES+1; j++)
		//{
		//	int DA_POS_OFFSET=DATAARRAY_OFFSET+j;
			// update hi byte of every entry
		//	if (data[DA_POS_OFFSET]!=0xFF) {data[DA_POS_OFFSET+3*MAX_ENTRIES_1]|=0xA0;};
		//};
		fclose(pf);
	}
	else
	{
		fprintf(stderr,"Open Error \"%s\".\n",filemenu);
		return 0;
	};

	return i;
};
#define ERROR(str) {fprintf(stderr,str); exit(1);}
/*--------------------------------------------------------------------*/
U8 saveCAR(const char *filename, U8 *data, unsigned int carsize, unsigned int phys_carsize)
{
	U8 header[16]={0x43, 0x41, 0x52, 0x54, 0x00, 0x00, 0x00, 0x2A,
		           0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00};

	if (filename==NULL) {
		fprintf(stderr,"Warning: -o not provided, no output generated.\n");
		return 0;
	}

	int output_type=checkTypeByPath(filename);
	U8 ret=0;
	unsigned int i,j,sum=0;
	FILE *pf;
	if (output_type == TYPE_CAR || output_type==TYPE_BIN ) {
		if (output_type==TYPE_CAR) {
			for (i=0; i<phys_carsize; i++) { sum+=data[i];};
			header[8]=((sum>>24)&0xFF);
			header[9]=((sum>>16)&0xFF);
			header[10]=((sum>>8)&0xFF);
			header[11]=(sum&0xFF);
			if (be_verbose)
				printf("Cartridge Checksum: %02x%02x%02x%02x\n",header[8],header[9],header[10],header[11]);
		}

		pf=fopen(filename,"wb");

		if (pf)
		{
			j=0;
			if (output_type==TYPE_CAR) {
				j=fwrite(header,sizeof(U8),16,pf);
				if (j!=16)
					fprintf(stderr,"Error: Cartridge image '%s' truncated (%d bytes written)\n",filename, j);
			}

			if (j==16 || output_type==TYPE_BIN)
			{
				i=fwrite(data,sizeof(U8),phys_carsize,pf);
				if (i==phys_carsize) {
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

	}
	else if (output_type==TYPE_XEX)
	{
		pf=fopen(filename,"wb");
		//FILE * pft=fopen("testoutput.bin","wb");
		if (pf)
		{
			// change some places in template
			// change chipnum
			menu4car_templateflasher_xex[TWO_CHIPS_SWITCH]=carsize>0x80000;
			// change cartsizes in two places
			char test[10];
			sprintf(test,"%5d",carsize/1024);
			for(int i=0; i<5; i++) {
				menu4car_templateflasher_xex[CARTSIZE1+i]=
					menu4car_templateflasher_xex[CARTSIZE2+i]=
					test[i]+0x80;

			}
			if (carsize/1024<1024) {
				menu4car_templateflasher_xex[CARTSIZE2]='>'+0x80;
				menu4car_templateflasher_xex[CARTSIZE2+1]='='+0x80;
			}

			// write head part with strings converted
			// change strings
			if (END_OF_MAIN_CODE!=fwrite(menu4car_templateflasher_xex,sizeof(U8),END_OF_MAIN_CODE,pf)) ERROR("Error writing file\n");
			
			// write blocks of data - as many as needed.
			for (int bank=0; bank<(phys_carsize/BANKSIZE); bank++) {
				int start=-1;
				int stop=0;
				for (int j=bank*BANKSIZE; j<(bank+1)*BANKSIZE; j++)
				{
					if (start==-1 && data[j]!=0xff) {
						start=j-bank*BANKSIZE;
					}
					if (data[j]!=0xff) stop=j-bank*BANKSIZE;
				}
				if (start==-1) continue; // empty bank
				//printf("Save bank: %06x\n",j);
				// write bank section
				U8 setbank[] = {0x88, 0x00, 0x88, 0x00, 0x00 };
				setbank[4]=bank;
				if (sizeof(setbank)!=fwrite(setbank,sizeof(U8),sizeof(setbank),pf)) ERROR("Error writing file\n");
				// write load address
				int istart=start+0x6000;
				int istop=stop+0x6000;
				U8 banksection[4];
				banksection[0]=istart&0xff;
				banksection[1]=istart>>8;
				banksection[2]=istop&0xff;
				banksection[3]=istop>>8;
				if (sizeof(banksection)!=fwrite(banksection,sizeof(U8),sizeof(banksection),pf)) ERROR("Error writing file\n");
				// write data itself
				if (stop-start+1!=fwrite(&data[bank*BANKSIZE+start],sizeof(U8),stop-start+1,pf)) ERROR("Error writing file\n");
				//if (0x2000!=fwrite(&data[bank*BANKSIZE],sizeof(U8),0x2000,pft)) ERROR("Error writing file\n");
				// write init address
				U8 bankend[] = {0xe2,0x02,0xe3,0x02,MAIN_PROCESS_JMP&0xff,MAIN_PROCESS_JMP>>8};
				if (sizeof(bankend)!=fwrite(bankend,sizeof(U8),sizeof(bankend),pf)) ERROR("Error writing file\n");
			}

			U8 fileend[] = {0xe2,0x02,0xe3,0x02,FINISH_PROCESS_JMP&0xff,FINISH_PROCESS_JMP>>8};
			if (sizeof(fileend)!=fwrite(fileend,sizeof(U8),sizeof(fileend),pf)) ERROR("Error writing file\n");

		fclose(pf);
		//fclose(pft);
		}
	}
	else
	{
		fprintf(stderr,"Error: -o provided filename has unsupported extension (possible are .car, .bin and .xex). No output generated.\n");
		return 0;
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
				cardata[PICTURE_DATA_OFFSET+i]=b;
				if (i>=512) break;
			};
			fclose(pf);
		};
	};
}
/*--------------------------------------------------------------------*/
void addFont(U8 * cardata, const char * fontpath)
{
	loadFile(fontpath,&cardata[FONT_OFFSET],1024);
}
/*--------------------------------------------------------------------*/
void addCTable(U8 * cardata, const char * colortablefile)
{
	loadFile(colortablefile,&cardata[COLORTABLE_OFFSET],16);
}
/*--------------------------------------------------------------------*/
void addPages(U8* data, U8* ramdata)
{
	int i=0;
	while (i<MAX_ENTRIES_1) { if (IS_LAST(data,i)) {--i;break;} i++; }

	int ii=i+1;
	int j=0;
	while (ii>0) {
		int k=MIN(26,ii);
		ii-=k;
		data[FILL_PAGES_OFFSET+j]=k+1;
		j++;
	}

	int pages=i/26;
	data[NUM_PAGES_OFFSET]=pages;

	if (pages>=1)
		for (int page=0; page<=pages; page++)
			for (int tpage=0; tpage<=pages; tpage++)
				ramdata[SCREENDATA_OFFSET+page*26*32+tpage*32]=tpage+17+128*(page==tpage);
}

/*--------------------------------------------------------------------*/
int carbintoflasherxex(const char * carbinfile, const char * carname, int cart_size_physical)
{
	static U8 cardata[FLASHMAX+16];
	int output_type=checkTypeByPath(carname);
	if (output_type!=TYPE_XEX){
		fprintf(stderr,"bad output file type (expected ext: (xex|exe|obx|com)) - no flasher created.\n");
		return 0;
	}
	int input_type=checkTypeByPath(carbinfile);

	if (!(input_type==TYPE_CAR || input_type==TYPE_BIN)) {
		fprintf(stderr,"bad output file type (expected ext: (xex|exe|obx|com)) - no flasher created.\n");
		return 0;
	}

	int size=loadFile(carbinfile,cardata,FLASHMAX+16);

	U8 * cb=cardata;
	if (input_type==TYPE_CAR) {
		cb+=16;
		size-=16;
	}

	// save xex flasher
	saveCAR(carname,cb,size,cart_size_physical);

	return 1;
}
/*--------------------------------------------------------------------*/
int menu4car(const char * filemenu, const char * logo, const char * colortablefile, const char * fontpath, const char * carname, int cart_size, int cart_size_physical, int default_do_compress)
{
	static U8 cardata[FLASHMAX];
	static U8 ramdata[8192];
	int i;
	fillData(cardata, FLASHMAX, 0xFF);
	fillData(ramdata, sizeof(ramdata), 0);
	addMenu(cardata,FLASHMAX,menu4car_bin,menu4car_bin_len,19);
	addCTable(cardata,colortablefile);

	for (i=0; i<ramdata_bin_len; i++) ramdata[i]=ramdata_bin[i];
	addLogo(ramdata,logo,256*16,8);
	addFont(ramdata,fontpath);

	int c=0;
	if ((c=addData(cardata,ramdata,cart_size,filemenu))) {

		addPages(cardata,ramdata);

		// COMPRESS ramdata
		int delta;
		int quick_mode=1;
		int csize;

		unsigned char * output_data = compress(
				optimize(&ramdata[0], ramdata_bin_len, 0, quick_mode ? MAX_OFFSET_ZX7 : MAX_OFFSET_ZX0),
				&ramdata[0], ramdata_bin_len,
				0, 0, 1,
				&csize, &delta
				);

		if (csize>=RAM_MENU_SPACE)
		{
			fprintf(stderr,"Too long entry list - no car file created.\n");
			free(output_data);
			return 0;
		}

		for (int c=0; c<csize; c++) cardata[c]=output_data[c];
		free(output_data);

		int output_type=checkTypeByPath(carname);

		if (output_type==TYPE_UNKNOWN){
			char fname [1024];
			sprintf(fname,"%s.xex",carname);
			saveCAR(fname,cardata,cart_size,cart_size_physical);
			sprintf(fname,"%s.car",carname);
			saveCAR(fname,cardata,cart_size,cart_size_physical);
			sprintf(fname,"%s.bin",carname);
			saveCAR(fname,cardata,cart_size,cart_size_physical);
		}
		else
			saveCAR(carname,cardata,cart_size,cart_size_physical);
	}
	else
	{
		fprintf(stderr,"No files added - no car file created.\n");
		return 0;
	}
	return c;

}
void usage() {
	printf("Menu4CAR - ver: %s\n",__DATE__);
	printf("(c) GienekP\n\n");
	printf("usage:\nmenu4car menu.txt <options>\n");
	printf("\nOptions:\n");
	printf("	-p <path> - picdata path (default Menu4Car, built in), raw 8-bit b&w 512 byte length\n");
	printf("	-t <path> - color table path (default rainbow, built in), 16 byte length of atari colors\n");
	printf("	-o <path> - output car path (filetype: .car, .bin, .exe or .xex); no ext to save all .car, .bin and .xex.\n");
	printf("	-b <path> - input binary car image path (type .car or .bin) to make the cart flasher of\n");
	printf("	-a <path> - input binary car image path (type .car or .bin) to analyse\n");
	printf("	-c <compression> - forced compression method 0/1/2/3/a, (default 'a'uto) like in lines, in lines have priority over this)\n");
	printf("	-f <path> - path to 1024 byte length font file\n");
	printf("	-s <size> - logical cart size: 32/64/128/256/512/1024, default 1024\n");
	printf("	-S <size> - physical cart size: 32/64/128/256/512/1024, default as logical; if set must be after -s\n");
	printf("	-X <path> - offline block compress *.xex file to *.bzx0 for latter use (ignores all other switches)\n");
	printf("	-v - be verbose\n");
	printf("	-? - this help\n\n");
#ifndef __MINGW__
	exit(EX_USAGE);
#else
	exit(64);
#endif
}
/*--------------------------------------------------------------------*/
int main( int argc, char* argv[] )
{	
	if (argc<=1)
	{
		usage();
	};

	static char outfilearr[1024];
	char * logofilepath=NULL;
	char * colortablefile=NULL;
	char * outfile=NULL;
	char * carbinfilename=NULL;
	char * fontpath=NULL;
	int  cart_size=1024*1024;
	int  cart_size_physical=1024*1024;
	char * txtfilename=NULL;
	int i;

	i=1;
	while (i<argc)
	{
		if (argv[i][0]=='-') {
			if  (strlen(argv[i])>=2) {
				int has_val=i<(argc-1);
				//printf("arg: %c\n",argv[i][1]);
				int actualswitch=argv[i][1];
				switch (actualswitch) {
					case 'p':
						if (!has_val) usage();
						logofilepath=argv[++i];
						break;
					case 't':
						if (!has_val) usage();
						colortablefile=argv[++i];
						break;
					case 'o':
						if (!has_val) usage();
						outfile=argv[++i];
						break;
					case 'a':
						do_analyze=1;
					case 'b':
						if (!has_val) usage();
						carbinfilename=argv[++i];
						break;
					case 'c':
						if (!has_val) usage();
						i++;
						if (strlen(argv[i])==1) {
							switch (argv[i][0]) {
								case 'a':
									default_do_compress=-1;
									break;
								case '0':
								case '1':
								case '2':
								case '3':
									default_do_compress=argv[i][0]-'0';
									break;
								default:
									usage();
							}
						}

						break;
					case 'f':
						if (!has_val) usage();
						fontpath=argv[++i];
						break;
					case 'S':
					case 's':
						if (!has_val) usage();
						int s;
						i++;
						for (s=0; s<sizeof(cartsizetab)/sizeof(cartsizetab[0]); s++){
							char test[10];
							sprintf(test,"%d",cartsizetab[s]);

							if (strcmp(test,argv[i])==0) {
								int cs=strtol(argv[i],NULL,10)*1024;

								if (actualswitch=='S')
									cart_size_physical=cs;
								if (actualswitch=='s') {
									cart_size=cs;
									cart_size_physical=cs;
								}
								//printf("Cart size: %d\n",cart_size);
								break;
							}
						}
						if (s==6) usage();
						break;
					case 'X':
						if (!has_val) usage();
						txtfilename=argv[++i];
						xex_compress=1;
						break;
					case 'v': 
						be_verbose=strrchr(argv[i],'v')-argv[i];
						printf("verbose level: %d\n",be_verbose);
						
						break;
					default:
						usage();
						break;
				}
			} else
				usage();

		}
		else {
			if (!xex_compress)
				txtfilename=argv[i];
			else
				usage();
		}
		i++;
	}

	if (txtfilename && (outfile==NULL || xex_compress)) {
		char * c=strrchr(txtfilename,'.');
		if (!c) {
			strncpy(outfilearr,txtfilename,1023-8);
			outfilearr[1023-8]='\0'; // space for extension
		}
		else
		{
			strncpy(outfilearr,txtfilename,c-txtfilename);
			outfilearr[c-txtfilename]='\0';
		}
		outfile=outfilearr;
	}

	if (txtfilename && carbinfilename) {
		fprintf(stderr,"Selected two input files; please select one: menu.txt or -b cartfile.car\n\n");
		usage();
	}
	int res=0;

	if (!xex_compress) {
		if (txtfilename)
			res=menu4car(txtfilename,logofilepath, colortablefile, fontpath, outfile, cart_size, cart_size_physical, default_do_compress);
		else if (carbinfilename) {
			res=carbintoflasherxex(carbinfilename, outfile, cart_size_physical);
		}
		if (errorcounter>0)
			fprintf(stderr,"Warning: %d input file errors encountered.\n",errorcounter);

	} else {
		static U8 input_data[FLASHMAX];
		static U8 output_data[FLASHMAX];
		int delta;
		//strcat(outfilearr,".bzx0");
		strcat(outfilearr,".bapl");
		unsigned int input_size=loadFile(txtfilename,input_data,sizeof(input_data)-BANKSIZE-6);
		if (be_verbose) printf("Loaded %d bytes from file '%s'\n",input_size, txtfilename);
		int output_size=0;
		int quick_mode=0;
		if (input_size>0) {
			//output_size = compressBlockByBlock(COMPRESS_APL,input_data,input_size,output_data);

			input_size=repairFile(input_data,input_size);

			output_size = compressBlockByBlock(COMPRESS_APL,input_data,input_size,output_data);

			//output_data = compress(optimize(input_data, input_size, 0, quick_mode ? MAX_OFFSET_ZX7 : MAX_OFFSET_ZX0), input_data, input_size, 0, 0, 1, &output_size, &delta);

		}
		saveFile((char *)outfilearr,output_data,output_size);
		if (be_verbose) printf("Saved %d bytes to file '%s'\n",output_size,outfilearr);
	}

	return !res;
}
/*--------------------------------------------------------------------*/
