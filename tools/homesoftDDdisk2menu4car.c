#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

uint8_t buffer[256*10000];

void saveFile(int base, char * name)
{

}

uint8_t asctoint(uint8_t a)
{
	if (((a&0x7F)>=0x00) && ((a&0x7F)<=0x1F)) return a+0x40;
	if (((a&0x7F)>=0x20) && ((a&0x7F)<=0x5F)) return a-0x20;
	return a;
}
uint8_t inttoasc(uint8_t a) { return asctoint(asctoint(a)); }

int main(int argc, char ** argv)
{
   //int num;
   FILE *filein;

   // use appropriate location if you are using MacOS or Linux
   filein = fopen(argv[1],"r");
   if(filein == NULL)
   {
      printf("\nUsage: %s <HomesoftAtrFilePath>\n\n",argv[0]);   
      exit(1);             
   }
   fread(buffer, 16, 1, filein);
   size_t bytes=fread(buffer, 1,  256*10000, filein);


   fprintf(stderr,"read ATR bytes: %zu\n",bytes);
   fclose(filein);


#define BASETOC 0x16680
#define BASENAME 0x16B80

   for (int i=0; i<24; i++) {

	   int baseentry=BASETOC + (i/8)*256 + (i%8)*16;
	   int basename=BASENAME + 26 + (i)*24;

	   if (buffer[baseentry]==0x42) {
		   int last=22;
		   for(int j=21; j>=0; j--)
		   {
			   char c=buffer[basename + j];
			   if (!c) last=j;
			   if (c) break;
		   }
		   for(int j=0; j<last; j++)
		   {
			   char c=buffer[basename + j];
			   printf("%c",inttoasc(c));
		   }

		   printf("|");

		   char name[20];
		   char * np=name;

		   for(int j=0; j<8; j++)
		   {
			   char c=buffer[baseentry + j + 5];
			   if (c!=' ') *np++=c;
		   }

		   int f=1;
		   for(int j=0; j<3; j++)
		   {
			   char c=buffer[baseentry + j + 13];
			   if (c!=' ') {
				   if (f) *np++='.';
				   f=0;
				   *np++=c;
			   }
		   }
		   *np++=0;
		   printf("%s\n",name);
		   saveFile(baseentry, name);
	   }

   }


}
