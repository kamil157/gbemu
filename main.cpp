#include <fstream>
#include <iostream>
#include <vector>

// Disassemble 8080 opcodes into assembly language
ulong disassemble(const std::vector<uint8_t>& code, ulong pc)
{
    auto opbytes = 1u;
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

std::vector<uint8_t> readFile(const std::string& path)
{
    std::ifstream input(path, std::ios::binary);
    if (input.fail()) {
        throw std::runtime_error("Couldn't open file: " + path);
    }

    // copies all data into buffer
    std::vector<uint8_t> buffer(std::istreambuf_iterator<char>(input), {});
    return buffer;
}

int main(int argc, char** argv)
{
    try {
        if (argc < 2) {
            throw std::runtime_error("Please provide rom name.");
        }
        auto rom = readFile(argv[1]);
        auto pc = 0u;
        while (pc < rom.size()) {
            pc += disassemble(rom, pc);
        }
        return 0;
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
