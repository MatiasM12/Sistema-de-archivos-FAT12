# Sistema de Archivos FAT12 - Exploración en Profundidad

Este repositorio contiene el código y resultados de la exploración en profundidad del sistema de archivos FAT12 utilizando el archivo de imagen `test.img`.

## Puntos Destacados

### 1. Montaje del Archivo de Imagen
Para montar el archivo de imagen `test.img` en el directorio `/mnt` con permisos completos, ejecutamos el siguiente comando:
sudo mount test.img /mnt -o loop,umask=000
```
sudo mount test.img /mnt -o loop,umask=000
```
Esto permite trabajar con el disco montado como un block device.

### 2. MBR y Hex Editor
a) Mostramos el MBR con un Hex Editor, destacando los primeros bytes y la tabla de particiones. Se proporciona el código en C para leer y mostrar esta información.

b) Leemos los datos del MBR usando código C y los mostramos por pantalla.

c) Verificamos si la primera partición es booteable utilizando un Hex Editor y proporcionamos código C para mostrar sus detalles.

d) Mostramos detalles de la primera partición, como el flag de booteable, dirección CHS, tipo de partición y tamaño en sectores.

### 3. Exploración del Directorio Raíz
a) Identificamos y mostramos los archivos en el sistema de archivos FAT12 usando Ghex y proporcionamos código C para mostrarlos.

b) Demostramos la creación y eliminación de un archivo en la carpeta root y lo mostramos utilizando el código C previamente generado.

c) Mostramos en Ghex el archivo que ha sido borrado y proporcionamos código C para mostrar la información.

d) Discutimos la posibilidad de recuperar archivos y cómo se realiza.

### 4. Manipulación del FileSystem
a) Creamos un nuevo archivo llamado `lapapa.txt` en la carpeta root y mostramos su presencia en el sistema de archivos.

b) Mostramos el contenido del archivo `lapapa.txt` usando un Hex Editor y proporcionamos código C para mostrar el contenido no borrado.

c) Proporcionamos código C para buscar y recuperar un archivo, explicando el proceso detalladamente.

## Contacto

Para cualquier pregunta o comentario, no dudes en contactarnos:
- [LinkedIn ](https://www.linkedin.com/in/matias-morales-44409a215/)
- [Correo Electrónico](mailto:matias.agustin.morales@gmail.com)

Para mas informacion revisar el informe adjunto al repositorio.
¡Gracias por explorar nuestro Sistema de Archivos FAT12 en detalle! 🕵️‍♂️📂
