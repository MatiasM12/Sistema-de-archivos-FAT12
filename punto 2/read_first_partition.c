#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE * in = fopen("test.img", "rb");
    unsigned int start_sector, length_sectors;
    
    fseek(in, 0x1BE , SEEK_SET); //Voy a bucar la tabla de particiones
    
    //Imprimimos los datos correspondientes
    printf("Partition entry %d: First byte %02X\n", 0, fgetc(in));
    printf("Comienzo de partición en CHS: %02X:%02X:%02X\n", fgetc(in), fgetc(in), fgetc(in));
    printf("Partition type 0x%02X\n", fgetc(in));
    printf("Fin de partición en CHS: %02X:%02X:%02X\n", fgetc(in), fgetc(in), fgetc(in));
        
    fread(&start_sector, 4, 1, in);     //leo elemento y lo guardo en start_sector
    fread(&length_sectors, 4, 1, in);   //leo elemento y lo guardo en length_sectors

    printf("Dirección LBA relativa 0x%08X, de tamaño en sectores %d\n",start_sector,length_sectors);
    printf("---------------------------------------------------------\n");
    
    fclose(in);
    return 0;
}