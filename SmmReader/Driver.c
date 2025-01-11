#include <ntddk.h>

#define SMRAM_ADDRESS 0x30000 // SMRAMBASE - CS base.address = 0x30000 quando está no modo SMM
#define LOADER_ADDRESS 0x7C00 // Endereço físico onde o bootloader é carregado

#define ES_SELECTOR_OFFSET 0xFE00 // Seletor de segmento do registrador ES
#define CS_SELECTOR_OFFSET 0x10 // Seletor de segmento do registrador CS
#define SS_SELECTOR_OFFSET 0x20 // Seletor de segmento do registrador SS
#define DS_SELECTOR_OFFSET 0x30 // Seletor de segmento do registrador DS
#define FS_SELECTOR_OFFSET 0x40 // Seletor de segmento do registrador FS
#define GS_SELECTOR_OFFSET 0X50 // Seletor de segmento do registrador GS

void kernelUnload(_In_ PDRIVER_OBJECT DriverObject)
{
    UNREFERENCED_PARAMETER(DriverObject);
    KdPrint(("Descarregando Driver\n"));
}

NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath)
{
    UNREFERENCED_PARAMETER(RegistryPath);

    // Declarações e inicializações
    PHYSICAL_ADDRESS PhysicalAddressSearch = { 0 };
    PUCHAR ReturnedVirtualAddress = NULL;
    USHORT CsSelector = 0;
    USHORT GsSelector = 0;
    USHORT SsSelector = 0;
    USHORT DsSelector = 0;
    USHORT FsSelector = 0;
    USHORT EsSelector = 0;

    // Código do Driver
    PhysicalAddressSearch.QuadPart = SMRAM_ADDRESS + ES_SELECTOR_OFFSET;
    ReturnedVirtualAddress = MmMapIoSpace(PhysicalAddressSearch, 50, MmNonCached);
    if (ReturnedVirtualAddress == NULL)
    {
        KdPrint(("Endereço Virtual não pode ser mapeado\n"));
        return STATUS_UNSUCCESSFUL;
    }
    KdPrint(("SMRAM Address: 0x%x\n", SMRAM_ADDRESS));
    KdPrint(("SMRAM Status-Save Area: 0x%llx  - Mapeado para %p\n", PhysicalAddressSearch.QuadPart, ReturnedVirtualAddress));
    if (MmIsAddressValid(ReturnedVirtualAddress))
    {
        KdPrint(("Endereço %p é VÁLIDO\n", ReturnedVirtualAddress));
        // Mapeando os seletores
        KdPrint(("Mapeando Seletores\n"));
        EsSelector = *((USHORT*)(ReturnedVirtualAddress));
        CsSelector = *((USHORT*)(ReturnedVirtualAddress + CS_SELECTOR_OFFSET));
        SsSelector = *((USHORT*)(ReturnedVirtualAddress + SS_SELECTOR_OFFSET));
        DsSelector = *((USHORT*)(ReturnedVirtualAddress + DS_SELECTOR_OFFSET));
        FsSelector = *((USHORT*)(ReturnedVirtualAddress + FS_SELECTOR_OFFSET));
        GsSelector = *((USHORT*)(ReturnedVirtualAddress + GS_SELECTOR_OFFSET));

        // Exibir SELECTORS
        KdPrint(("\t\\___Value in CS(0x3FE%X) - Selector(0x%02X)\n", CS_SELECTOR_OFFSET, CsSelector));
        KdPrint(("\t\\___Value in DS(0x3FE%X) - Selector(0x%02X)\n", DS_SELECTOR_OFFSET, DsSelector));
        KdPrint(("\t\\___Value in SS(0x3FE%X) - Selector(0x%02X)\n", SS_SELECTOR_OFFSET, SsSelector));
        KdPrint(("\t\\___Value in ES(0x3%X) - Selector(0x%02X)\n", ES_SELECTOR_OFFSET, EsSelector));
        KdPrint(("\t\\___Value in FS(0x3FE%X) - Selector(0x%02X)\n", FS_SELECTOR_OFFSET, FsSelector));
        KdPrint(("\t\\___Value in GS(0x3FE%X) - Selector(0x%02X)\n", GS_SELECTOR_OFFSET, GsSelector));
    }
    else {
        KdPrint(("O endereço mapeado não é válido\n"));
    }

    MmUnmapIoSpace(ReturnedVirtualAddress, 50); // Realizar o desmapeamento do endereço virtual

    KdPrint(("----------------------------------\n"));
    KdPrint(("Iniciando mapeamento do Bootloader\n"));

    // Iniciando mapeamento do bootloader
    BOOLEAN StateLoaderValid = 0;
    PhysicalAddressSearch.QuadPart = LOADER_ADDRESS;
    ReturnedVirtualAddress = MmMapIoSpace(PhysicalAddressSearch, 512, MmNonCached);
    if (ReturnedVirtualAddress == NULL)
    {
        KdPrint(("O endereço não pode ser mapeado\n"));
        return STATUS_UNSUCCESSFUL;
    }
    KdPrint(("Endereço Mapeado: %p\n", ReturnedVirtualAddress));
    if (MmIsAddressValid(ReturnedVirtualAddress))
    {
        if (ReturnedVirtualAddress[510] == 0x55 && ReturnedVirtualAddress[511] == 0xAA)
        {
            KdPrint(("Bootloader STATE: VALID - SIGNATURE: 0x55AA\n"));
            StateLoaderValid = TRUE;
        }
        if (StateLoaderValid == 0)
        {
            KdPrint(("Returned - BOOTLOADER INVALID\n"));
        }
    }
    else
    {
        KdPrint(("Endereço de memória retornado não é válido\n"));
    }

    MmUnmapIoSpace(ReturnedVirtualAddress, 512);
    WRITE_PORT_UCHAR((PUCHAR)0xB2, 0xFE);
    DriverObject->DriverUnload = kernelUnload;
    return STATUS_SUCCESS;
}