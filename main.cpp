#include <fstream>
#include <iostream>
#include <vector>

// Disassemble 8080 opcodes into assembly language
ulong disassemble(std::vector<uint8_t> code, ulong pc)
{
    ulong opbytes = 1;
    printf("%04x ", static_cast<uint>(pc));
    switch (code[pc]) {
    case 0x00:
        printf("NOP");
        break;
    case 0x01:
        printf("LXI    B,#$%02x%02x", code[pc + 2], code[pc + 1]);
        opbytes = 3;
        break;
    case 0x02:
        printf("STAX   B");
        break;
    case 0x03:
        printf("INX    B");
        break;
    case 0x04:
        printf("INR    B");
        break;
    case 0x05:
        printf("DCR    B");
        break;
    case 0x06:
        printf("MVI    B,#$%02x", code[pc + 1]);
        opbytes = 2;
        break;
    case 0x07:
        printf("RLC");
        break;
    case 0x08:
        printf("NOP");
        break;
    /* ........ */
    case 0x3e:
        printf("MVI    A,#0x%02x", code[pc + 1]);
        opbytes = 2;
        break;
    /* ........ */
    case 0xc3:
        printf("JMP    $%02x%02x", code[pc + 2], code[pc + 1]);
        opbytes = 3;
        break;
        /* ........ */
    }

    printf("\n");

    return opbytes;
}

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cerr << "Please provide rom name." << std::endl;
        return 1;
    }
    std::ifstream file(argv[1], std::ios::binary | std::ios::ate);
    if (!file) {
        std::cerr << "Couldn't open file: " << argv[1] << std::endl;
        return 1;
    }

    // Get the file size and read it into a memory buffer
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> buffer(static_cast<ulong>(size));
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
        std::cerr << "Couldn't read file: " << argv[1] << std::endl;
        return 1;
    }

    ulong pc = 0;
    while (static_cast<long>(pc) < size) {
        pc += disassemble(buffer, pc);
    }
    return 0;
}
