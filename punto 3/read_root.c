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
    unsigned char jmp[3];                 // Salto de código de arranque
    char oem[8];                          // Nombre del fabricante del sistema de archivos
    unsigned short sector_size;           // Tamaño de cada sector en bytes
    unsigned char sector_cluster;         // Número de sectores por clúster
    unsigned short reserved_sectors;      // Número de sectores reservados en el sistema de archivos
    unsigned char number_of_fats;         // Número de tablas FAT en el sistema de archivos
    unsigned short root_dir_entries;      // Número de entradas en el directorio raíz
    unsigned short sector_volumen;        // Número total de sectores en el volumen
    unsigned char descriptor;             // Descriptor del medio de almacenamiento
    unsigned short fat_size_sectors;      // Tamaño de cada tabla FAT en sectores
    unsigned short sector_track;          // Número de sectores por pista
    unsigned short headers;               // Número de cabezas de lectura/escritura
    unsigned int sector_hidden;           // Número de sectores ocultos antes del inicio de la partición
    unsigned int sector_partition;        // Número total de sectores en la partición
    unsigned char physical_device;        // Número de unidad física
    unsigned char current_header;         // Número de cabeza de lectura/escritura actual
    unsigned char firm;                   // Versión del sistema de archivos
    unsigned int volume_id;               // Identificación única del volumen
    char volume_label[11];                // Etiqueta de volumen
    char fs_type[8];                      // Nombre del sistema de archivos (FAT12 en este caso)
    char boot_code[448];                  // Código de arranque
    unsigned short boot_sector_signature; // Firma del sector de arranque
} __attribute((packed)) Fat12BootSector;

typedef struct
{
    unsigned char filename[8];                           // Nombre del archivo (8 caracteres)
    unsigned char extension[3];                          // Extensión del archivo (3 caracteres)
    unsigned char attributes[1];                         // Atributos del archivo
    unsigned char reserved;                              // Campos reservados
    unsigned char created_time_seconds;                  // Segundos en el que se creó el archivo
    unsigned char created_time_hours_minutes_seconds[2]; // Hora en que se creó el archivo (HH:MM:SS)
    unsigned short create_date;                          // Fecha de creación del archivo
    unsigned short access_date;                          // Fecha de acceso al archivo
    unsigned short cluster_high;                         // Número de clúster alto
    unsigned short modify_time;                          // Hora de modificación del archivo
    unsigned short modify_date;                          // Fecha de modificación del archivo
    unsigned short cluster_low;                          // Número de clúster bajo
    unsigned int size_of_file;                           // Tamaño del archivo en bytes
} __attribute((packed)) Fat12Entry;

void print_file_info(Fat12Entry *entry)
{
    // Verificar si la entrada está vacía
    if (entry->filename[0] == 0x00)
    {
        return;
    }

    // Verificar si la entrada representa un archivo borrado
    if (entry->filename[0] == 0xE5)
    {
        printf("Deleted file: [%.7s%.3s]\n", entry->filename, entry->extension);
        return;
    }

    // Verificar el atributo de la entrada
    switch (entry->attributes[0])
    {
        {
        case 0x10:                                          // Entrada tipo directorio
            printf("Directory: [%.7s]\n", entry->filename); // Imprimo directorio
            return;
        case 0x20:                                                           // Entrada tipo archivo
            printf("File: [%.7s%.7s]\n", entry->filename, entry->extension); // Imprimo archivo
            return;
        }
    }
}

int main()
{
    FILE *in = fopen("test.img", "rb");
    int i, position_root_directory;
    PartitionTable pt[4];
    Fat12BootSector bs;
    Fat12Entry entry;

    fseek(in, 0x1BE, SEEK_SET);               // Ir al inicio de la tabla de particiones.
    fread(pt, sizeof(PartitionTable), 4, in); // Leo entradas

    for (i = 0; i < 4; i++)
    { // Buscar particiones booteables
        printf("Partiion type: %d\n", pt[i].partition_type);
        if (pt[i].partition_type == 1)
        {
            printf("Encontrado FAT12 %d\n", i);
            break;
        }
    }

    if (i == 4)
    { // No se encontro nunguna particion booteable
        printf("No se encontró filesystem FAT12, saliendo ...\n");
        return -1;
    }

    fseek(in, 0, SEEK_SET); // Voy al inicio del filesystem
    fread(&bs, sizeof(Fat12BootSector), 1, in);

    printf("En  0x%lX, sector size %d, FAT size %d sectors, %d FATs\n\n",
           ftell(in), bs.sector_size, bs.fat_size_sectors, bs.number_of_fats);

    position_root_directory = (bs.reserved_sectors - 1 + bs.fat_size_sectors * bs.number_of_fats) * bs.sector_size; /* Calculo posicion inicial
                                                                                                                         del root directory */

    fseek(in, position_root_directory, SEEK_CUR); // Ir al inicio del root directory

    printf("Root dir_entries %d \n", bs.root_dir_entries);

    for (i = 0; i < bs.root_dir_entries; i++) // Recorro las entradas e imprimo
    {
        fread(&entry, sizeof(entry), 1, in);
        print_file_info(&entry);
    }

    fclose(in);
    return 0;
}
