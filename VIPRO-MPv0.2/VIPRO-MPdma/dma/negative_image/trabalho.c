#include <stdio.h>
#include <stdlib.h>

volatile int int_cont = 0;
unsigned char *pIRQ;

void __it_handle(){
 	int_cont++;
	printf("IT_HANDLE!!! "); 

}

// Copy the IRQ_Routine to the IRQ address = 0x00300000
void init_IT() { 
   unsigned char *p=0x00300000;
 
   asm(" la  $27, _IRQ_Routine" );
   asm(" sw  $27, pIRQ " );

   memcpy(p, pIRQ, 1024); 

}

int main(){
	unsigned char c0, c1, c2, c3, final, RGB;
	unsigned long int total_char, endian, i;
	unsigned char vetor_carac[16];
	FILE *foto_in = fopen ("/home/alunos/Área de Trabalho/VIPRO-MPv0.2/dma/negative_image/planeta.tif", "rb");
	FILE *foto_out = fopen ("/home/alunos/Área de Trabalho/VIPRO-MPv0.2/dma/negative_image/planeta.tif", "w+");
	if(!foto_in){
		printf ("Erro Open File 1");
	}else{
		if(!foto_out){
			printf ("Erro Open File 2");
		}else{
			fscanf (foto_in, "%c%c%c%c", &c0, &c1, &c2, &c3);
			fprintf (foto_out, "%c%c%c%c", c0, c1, c2, c3);
			endian = (c0 << 8) + c1;
			fscanf (foto_in, "%c%c%c%c", &c0, &c1, &c2, &c3);
			fprintf (foto_out, "%c%c%c%c", c0, c1, c2, c3);
			if (endian = 18761){
				total_char = ((c3 << 24) + (c2 << 16) + (c1 << 8) + c0);
			}else{
				total_char = ((c0 << 24) + (c1 << 16) + (c2 << 8) + c3);
			}
			total_char = total_char - 8;
			
			for (total_char; total_char > 16; total_char = total_char - 16){
				fscanf (foto_in, "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c", &vetor_carac[0], &vetor_carac[1], &vetor_carac[2], &vetor_carac[3], &vetor_carac[4], &vetor_carac[5],
							&vetor_carac[6], &vetor_carac[7], &vetor_carac[8], &vetor_carac[9], &vetor_carac[10], &vetor_carac[11], &vetor_carac[12], &vetor_carac[13],
							&vetor_carac[14], &vetor_carac[15]);
				for( i = 0; i < 16; i ++){
					vetor_carac[i] = 255 - vetor_carac[i];
				}
				fprintf (foto_out, "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c", vetor_carac[0], vetor_carac[1], vetor_carac[2], vetor_carac[3], vetor_carac[4], vetor_carac[5],
							vetor_carac[6], vetor_carac[7], vetor_carac[8], vetor_carac[9], vetor_carac[10], vetor_carac[11], vetor_carac[12], vetor_carac[13],
							vetor_carac[14], vetor_carac[15]);
			}
			for(total_char; total_char >= 1; total_char--){
				fscanf (foto_in, "%c", &RGB);
				RGB = 255 - RGB;
				fprintf (foto_out, "%c", RGB);
			}
			while (fscanf(foto_in,"%c",&final) != EOF){
				putc (final, foto_out);	
			}
		}
	}
	fclose (foto_in);
	fclose (foto_out);
	return 0;
}
