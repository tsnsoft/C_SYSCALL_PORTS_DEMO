#include <stdio.h> 
#include <unistd.h> 
#include <sys/io.h> 
#include <stdlib.h> 
#include <fcntl.h> 
#define PARPORT_BASE 0x378 // Начальный адрес нужных нам портов
#define NUM_PORTS 2 // Количество нужных нам портов

// Задаем адрес и количество портов
unsigned long addr = PARPORT_BASE, nports = NUM_PORTS;

// Пример работы с портами через ioperm-метод
int do_ioperm() {
    unsigned char zero = 0, readout = 0;
    // Пытаемся получить доступ (1) к нужным портам
    if (ioperm(addr, nports, 1)) return EXIT_FAILURE;
    printf("Запись: %hhu  в %lx\n", zero, addr);
    outb(zero, addr); // Посылаем в порт по адресу addr значение zero
    usleep(1000); // Задержка 1 секунда
    readout = inb(addr + 1); // Считываем байт из порта по адресу addr + 1
    printf("Чтение: %hhu из %lx\n", readout, addr + 1);
    // Пытаемся отключиться от доступа (0) к нужным портам
    if (ioperm(addr, nports, 0)) return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

// Пример работы с портами через /dev/port-метод
int do_devport() {
    unsigned char zero = 0, readout = 0; int fd, ss;
    // Пытаемся получить доступ к портам
    if ((fd = open("/dev/port", O_RDWR)) < 0) return EXIT_FAILURE;
    // Пытаемся открыть нужный порт (через смещение от начала)
    if (addr != lseek(fd, addr, SEEK_SET)) return EXIT_FAILURE;
    printf("Запись: %hhu  в  %lx\n", zero, addr);
    ss = write(fd, &zero, 1); // Записываем в порт число 0, один байт
    usleep(1000); // Задержка 1 секунда
    ss = read(fd, &readout, 1); // Читаем из порта число, один байт
    printf("Чтение: %hhu из %lx\n", readout, addr + 1);
    close(fd); // Закрываем доступ к портам
    return EXIT_SUCCESS;
}

// Точка входа в программу
int main(int argc, char *argv[]) {
    // Запускаем пример работы с портами через /dev/port-метод
    if (do_devport()) fprintf(stderr, "reading /dev/port method failed\n");
    // Запускаем пример работы с портами через ioperm-метод
    if (do_ioperm()) fprintf(stderr, "ioperm method failed\n");

    char msg[] = "Пока всем!\n";
    int sys_write = 1, handle_stdout = 1, number_bytes = sizeof (msg);

    // Пример системного вызова syscall
    syscall(sys_write, handle_stdout, msg, number_bytes);

    // Пример системного вызова syscall на встроенном шаблонном ассемблере с AT&T-синтаксисом (не Intel!)
    asm volatile ("syscall"::"a" (sys_write), "D" (handle_stdout), "S" (msg), "d" (number_bytes));
    //                a - load into the eax register
    //                D - load into the edi register
    //                S - load into the esi register
    //                d - load into the edx register

    //                g - let the compiler decide which register to use for the variable
    //                r - load into any avaliable register
    //                b - load into the ebx register
    //                c - load into the ecx register
    //                f - load into the floating point register

    // Аналог Intel-синтаксиса
    //        mov     eax, 1                  ; system call 1 is write
    //        mov     edi, 1                  ; file handle 1 is stdout
    //        mov     esi, message            ; address of string to output
    //        mov     edx, 19                 ; number of bytes
    //        syscall                         ; invoke operating system to do the write

    return EXIT_SUCCESS;
}
