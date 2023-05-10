#include <stdio.h>

int main(){
	
	FILE *ptr = NULL;
	long one_megab = 1048575;

	ptr = fopen("1_mb.txt","w");
	fseek(ptr,one_megab,SEEK_SET);
	fputc('\0',ptr);
	fclose(ptr);

	ptr = fopen("10_mb.txt","w");
        fseek(ptr,one_megab*10,SEEK_SET);
        fputc('\0',ptr);
        fclose(ptr);

	ptr = fopen("100_mb.txt","w");
        fseek(ptr,one_megab*100,SEEK_SET);
        fputc('\0',ptr);
        fclose(ptr);
	return 0;
}
