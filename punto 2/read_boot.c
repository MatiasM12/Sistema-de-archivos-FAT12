#include <stdio.h>
#include <stdlib.h>

    typedef struct
{
    unsigned char first_byte;
    unsigned char start_chs[3];
    unsigned char partition_type;
    unsigned char end_chs[3];
    char start_sector[4];
    char length_sectors[4];
} __attribute((packed)) PartitionTable;

typedef struct
{
    unsigned char jmp[3];
    char oem[8];
    unsigned short sector_size; // 2 bytes

    unsigned char sectors_per_cluster; // se agregaron estas variables para poder leer y guardar los diferentes parametros del boot sector
    unsigned short reserved_sectors;
    unsigned char number_of_fats;
    unsigned short root_dir_entries;
    unsigned short total_sectors_short;
    unsigned char media_descriptor;
    unsigned short fat_size_sectors;
    unsigned short sectors_per_track;
    unsigned short number_of_heads;
    unsigned long hidden_sectors;
    unsigned long total_sectors_long;
    unsigned char drive_number;
    unsigned char current_head;
    unsigned char boot_signature;
    unsigned int volume_id;
    char volume_label[11];
    char fs_type[8]; 
    char boot_code[448];
    unsigned short boot_sector_signature;
} __attribute((packed)) Fat12BootSector;

int main()
{
    FILE *in = fopen("test.img", "rb");
    int i;
    PartitionTable pt[4];
    Fat12BootSector bs;

    fseek(in, 0x1BE, SEEK_SET);               // Ir al inicio de la tabla de particiones. Completar ...
    fread(pt, sizeof(PartitionTable), 4, in); // leo entradas

    for (i = 0; i < 4; i++)
    {
        printf("Partiion type: %d\n", pt[i].partition_type);
        if (pt[i].partition_type == 1)
        {
            printf("Encontrado FAT12 %d\n", i);
            break;
        }
    }

    if (i == 4)
    {
        printf("No se encontró filesystem FAT12, saliendo ...\n");
        return -1;
    }

    fseek(in, 0x1BE, SEEK_SET);
    fread(&bs, sizeof(Fat12BootSector), 1, in);

    printf("  Jump code: %02X:%02X:%02X\n", bs.jmp[0], bs.jmp[1], bs.jmp[2]);
    printf("  OEM code: [%.8s]\n", bs.oem);
    printf("  sector_size: %d\n", bs.sector_size);
    printf("  sectors_per_cluster: %hhu\n", bs.sectors_per_cluster); // se agregaron para poder imprimir en pantalla cada una de las variabes anteriores
    printf("  reserved_sectors: %hhu\n", bs.reserved_sectors);
    printf("  number_of_fats: %hhu\n", bs.number_of_fats);
    printf("  root_dir_entries: %d\n", bs.root_dir_entries);
    printf("  total_sectors_short: %d\n", bs.total_sectors_short);
    printf("  media_descriptor: %hhu\n", bs.media_descriptor);
    printf("  fat_size_sectors: %d\n", bs.fat_size_sectors);
    printf("  sectors_per_track: %d\n", bs.sectors_per_track);
    printf("  number_of_heads: %d\n", bs.number_of_heads);
    printf("  hidden_sectors: %lu\n", bs.hidden_sectors);
    printf("  total_sectors_long: %ld\n", bs.total_sectors_long);
    printf("  volume_id: 0x%08X\n", bs.volume_id);
    printf("  Volume label: [%.11s]\n", bs.volume_label);
    printf("  Filesystem type: [%.8s]\n", bs.fs_type);
    printf("  Boot sector signature: 0x%04X\n", bs.boot_sector_signature);

    fclose(in);
    return 0;
}