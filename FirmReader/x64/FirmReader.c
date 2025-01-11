#include <stdio.h>
#include <windows.h>

#define BUFFERSIZE 1024

int main(int argc, char *argv[])
{
    // Variáveis
    HANDLE hFile;
    char readBuffer[BUFFERSIZE];
    DWORD dwBytesRead = 0;
    char searchOpcode[2] = {0xE6, 0xB2}; // Opcode para "out 0xB2"
    DWORD fileOffset = 0; // Para rastrear o offset no arquivo

    //PROGRAMA
    if(argv[1] == NULL)
    {
        printf("Caminho do arquvio nao informado\n");
        printf("EXAMPLE: FirmReader.exe [caminho]\n");
        return -1;
    }
    // Abre o arquivo para leitura
    hFile = CreateFile(argv[1], GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        printf("Erro ao abrir o arquivo: %ld\n", GetLastError());
        return 1;
    }
    system("cls");
    printf("\nProcurando pelo opcode %02X %02X no arquivo: %s\n", (unsigned char)searchOpcode[0], (unsigned char)searchOpcode[1], argv[1]);

    // Lê o arquivo em blocos
    while (ReadFile(hFile, readBuffer, BUFFERSIZE, &dwBytesRead, NULL) && dwBytesRead > 0)
    {
        for (DWORD i = 0; i < dwBytesRead - 1; i++) // -1 para evitar leitura além do buffer
        {
            if (memcmp(&readBuffer[i], searchOpcode, 2) == 0) // Compara o que tem no buffer com a assinatura do OPCODE para validar se o que tem no endereço é o
            {
                printf("Opcode encontrado no offset: 0x%lX - Content: [%02X %02X]\n", fileOffset + i, (unsigned char)readBuffer[i], (unsigned char)readBuffer[i + 1]);
                printf("\t\\___ Mapped OPCODE: out 0xB2\n\n");
            }
        }
        fileOffset += dwBytesRead; //
    }

    // Fecha o identificador para o arquivo
    CloseHandle(hFile);

    printf("Finalizando a busca\n");
    return 0;
}
