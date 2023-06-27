#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <string.h>
#define FAT_SIZE 0;  // tamaño de la tabla FAT en bytes
#define FAT_OFFSET 0 // desplazamiento de la tabla FAT desde el inicio del archivo

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

unsigned short get_next_cluster(unsigned short current_cluster, unsigned char *fat_table)
{
    unsigned fat_offset = current_cluster + (current_cluster / 2);  // Offset 
    unsigned short next_cluster = 0;                                // Siguiente cluster
    unsigned char fat_entry[2];             

    fat_entry[0] = fat_table[fat_offset];                           // Primer byte
    fat_entry[1] = fat_table[fat_offset + 1];                       // Segundo byte

    if (current_cluster % 2 == 0)
    {
        next_cluster = ((fat_entry[1] & 0x0F) << 8) | fat_entry[0]; // Ensamble si es par
    }
    else
    {
        next_cluster = (fat_entry[1] << 4) | ((fat_entry[0] & 0xF0) >> 4); // Ensamble si es impar
    }

    return next_cluster;
}

void recovery_file(unsigned short firstCluster, unsigned short deletedFileFirstCluster, unsigned short clusterSize,
                   int fileSize, const char *filename, char *extension)
{
    FILE *in = fopen("test.img", "rb");
    char cluster[clusterSize];                                 // Array para almecenar el cluster
    char new_file_name[256];                                   // Variable para el nombre del nuevo archivo
    sprintf(new_file_name, "/mnt/%s.%s", filename, extension); // Guardo el nombre del nuevo archivo
    int bytes_to_write;                                        // Cantidad de bytes que escribo

    unsigned char fat_table[sizeof(Fat12BootSector)]; // Variable para guardar la tabla FAT
    fseek(in, 0, SEEK_SET);
    fread(fat_table, 1, sizeof(Fat12BootSector), in);

    unsigned short current_cluster = deletedFileFirstCluster; // Cluster actual
    unsigned bytes_remaining = fileSize;                      // Bytes restantes a leer
    FILE *new_file = fopen(new_file_name, "wb");              // Creamos nuevo archivo para almacenar los datos recuperados

    /*Recorro si no llego al ultimo cluster y si la cant de bytes restantes es mayor a cero*/
    while (current_cluster < 0xFF8 && bytes_remaining > 0)
    {
        fseek(in, firstCluster + ((current_cluster - 2) * clusterSize), SEEK_SET); // Nos posicionamos en el primer cluster del archivo
        fread(cluster, clusterSize, 1, in);

        if (bytes_remaining > clusterSize) // Si la cantidad restante es mayor al tamaño del cluster la cantidad a escribir es el tamaño del cluster
            bytes_to_write = clusterSize;
        else
            bytes_to_write = bytes_remaining; // Si no la cantidad a escribir son los bytes restantes

        fwrite(cluster, 1, bytes_to_write, new_file); // Escribo el cluster

        // Actualizar el número de bytes restantes y el número del siguiente cluster
        bytes_remaining -= bytes_to_write;
        current_cluster = get_next_cluster(current_cluster, fat_table);
    }

    fclose(in);
    fclose(new_file);
    printf("Archivo recuperado exitosamente.\n");
}

void print_file_info(Fat12Entry *entry, unsigned short firstCluster, unsigned short clusterSize,
                     const char *filename, FILE *in)
{
    // Verificar si la entrada representa un archivo borrado
    if (entry->filename[0] == 0xE5)
    {
        char delete_filename[100];                     // Variable para guardar el archivo a borrar
        strcpy(delete_filename, filename);             // Pasamos el nombre a la variable anterior

        char *name = strtok(delete_filename, ".");     // Dividimos el nombre de la extension
        char *extension = strtok(NULL, ".");

        char *compare_name = strstr(entry->filename, name + 1); // Si el nombre indicado lo contiene el archivo lo devuelve
        char *compare_ext = strstr(entry->extension, extension);// Si la extension indicada la contiene el archivo lo devuelve

        if (compare_name != NULL && compare_ext != NULL) /* Si el nombre y la extension estan 
                                                              contenidas lo recupero */
        {
            printf("Deleted file: [%.8s%.3s]\n", entry->filename, entry->extension);
            recovery_file(firstCluster, entry->cluster_low, clusterSize, entry->size_of_file,
                             name, extension);
            return;
        }
    }
}

int main()
{
    FILE *in = fopen("test.img", "rb");
    int i, position_root_directory, size_cluster;
    PartitionTable pt[4];
    Fat12BootSector bs;
    Fat12Entry entry;
    unsigned short first_cluster;
    const char *filename = "LAPAPA.TXT"; // Nombre del archivo a recuperar

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

    fseek(in, 0, SEEK_SET);
    fread(&bs, sizeof(Fat12BootSector), 1, in);

    printf("En  0x%lX, sector size %d, FAT size %d sectors, %d FATs\n\n",
           ftell(in), bs.sector_size, bs.fat_size_sectors, bs.number_of_fats);

    position_root_directory = (bs.reserved_sectors - 1 + bs.fat_size_sectors * bs.number_of_fats) * bs.sector_size; /* Calculo posicion inicial
                                                                                                                         del root directory */

    fseek(in, position_root_directory, SEEK_CUR); // Ir al inicio del root directory

    printf("Root dir_entries %d \n", bs.root_dir_entries);

    first_cluster = ftell(in) + (bs.root_dir_entries * sizeof(entry)); /* Obtenemos la posición del
                                                             primer byte del primer clúster de datos*/

    size_cluster = bs.sector_size * bs.sector_cluster; // Tamaño en bytes de un clúster.

    for (i = 0; i < bs.root_dir_entries; i++) // Recorro las entradas e imprimo
    {
        fread(&entry, sizeof(entry), 1, in);
        print_file_info(&entry, first_cluster, size_cluster, filename, in);
    }

    fclose(in);
    return 0;
}