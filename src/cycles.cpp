#include "cycles.h"

#include "cpu.h"

int getCycles(uint8_t opcode, uint8_t cbopcode, bool z, bool c)
{
    auto cycles = 0;
    switch (opcode) {
    case 0x00:
        cycles = 4;
        break;
    case 0x01:
        cycles = 12;
        break;
    case 0x02:
        cycles = 8;
        break;
    case 0x03:
        cycles = 8;
        break;
    case 0x04:
        cycles = 4;
        break;
    case 0x05:
        cycles = 4;
        break;
    case 0x06:
        cycles = 8;
        break;
    case 0x07:
        cycles = 4;
        break;
    case 0x08:
        cycles = 20;
        break;
    case 0x09:
        cycles = 8;
        break;
    case 0x0a:
        cycles = 8;
        break;
    case 0x0b:
        cycles = 8;
        break;
    case 0x0c:
        cycles = 4;
        break;
    case 0x0d:
        cycles = 4;
        break;
    case 0x0e:
        cycles = 8;
        break;
    case 0x0f:
        cycles = 4;
        break;
    case 0x10:
        cycles = 4;
        break;
    case 0x11:
        cycles = 12;
        break;
    case 0x12:
        cycles = 8;
        break;
    case 0x13:
        cycles = 8;
        break;
    case 0x14:
        cycles = 4;
        break;
    case 0x15:
        cycles = 4;
        break;
    case 0x16:
        cycles = 8;
        break;
    case 0x17:
        cycles = 4;
        break;
    case 0x18:
        cycles = 12;
        break;
    case 0x19:
        cycles = 8;
        break;
    case 0x1a:
        cycles = 8;
        break;
    case 0x1b:
        cycles = 8;
        break;
    case 0x1c:
        cycles = 4;
        break;
    case 0x1d:
        cycles = 4;
        break;
    case 0x1e:
        cycles = 8;
        break;
    case 0x1f:
        cycles = 4;
        break;
    case 0x20:
        cycles = z == 0 ? 12 : 8;
        break;
    case 0x21:
        cycles = 12;
        break;
    case 0x22:
        cycles = 8;
        break;
    case 0x23:
        cycles = 8;
        break;
    case 0x24:
        cycles = 4;
        break;
    case 0x25:
        cycles = 4;
        break;
    case 0x26:
        cycles = 8;
        break;
    case 0x27:
        cycles = 4;
        break;
    case 0x28:
        cycles = z == 1 ? 12 : 8;
        break;
    case 0x29:
        cycles = 8;
        break;
    case 0x2a:
        cycles = 8;
        break;
    case 0x2b:
        cycles = 8;
        break;
    case 0x2c:
        cycles = 4;
        break;
    case 0x2d:
        cycles = 4;
        break;
    case 0x2e:
        cycles = 8;
        break;
    case 0x2f:
        cycles = 4;
        break;
    case 0x30:
        cycles = c == 0 ? 12 : 8;
        break;
    case 0x31:
        cycles = 12;
        break;
    case 0x32:
        cycles = 8;
        break;
    case 0x33:
        cycles = 8;
        break;
    case 0x34:
        cycles = 12;
        break;
    case 0x35:
        cycles = 12;
        break;
    case 0x36:
        cycles = 12;
        break;
    case 0x37:
        cycles = 4;
        break;
    case 0x38:
        cycles = c == 1 ? 12 : 8;
        break;
    case 0x39:
        cycles = 8;
        break;
    case 0x3a:
        cycles = 8;
        break;
    case 0x3b:
        cycles = 8;
        break;
    case 0x3c:
        cycles = 4;
        break;
    case 0x3d:
        cycles = 4;
        break;
    case 0x3e:
        cycles = 8;
        break;
    case 0x3f:
        cycles = 4;
        break;
    case 0x40:
        cycles = 4;
        break;
    case 0x41:
        cycles = 4;
        break;
    case 0x42:
        cycles = 4;
        break;
    case 0x43:
        cycles = 4;
        break;
    case 0x44:
        cycles = 4;
        break;
    case 0x45:
        cycles = 4;
        break;
    case 0x46:
        cycles = 8;
        break;
    case 0x47:
        cycles = 4;
        break;
    case 0x48:
        cycles = 4;
        break;
    case 0x49:
        cycles = 4;
        break;
    case 0x4a:
        cycles = 4;
        break;
    case 0x4b:
        cycles = 4;
        break;
    case 0x4c:
        cycles = 4;
        break;
    case 0x4d:
        cycles = 4;
        break;
    case 0x4e:
        cycles = 8;
        break;
    case 0x4f:
        cycles = 4;
        break;
    case 0x50:
        cycles = 4;
        break;
    case 0x51:
        cycles = 4;
        break;
    case 0x52:
        cycles = 4;
        break;
    case 0x53:
        cycles = 4;
        break;
    case 0x54:
        cycles = 4;
        break;
    case 0x55:
        cycles = 4;
        break;
    case 0x56:
        cycles = 8;
        break;
    case 0x57:
        cycles = 4;
        break;
    case 0x58:
        cycles = 4;
        break;
    case 0x59:
        cycles = 4;
        break;
    case 0x5a:
        cycles = 4;
        break;
    case 0x5b:
        cycles = 4;
        break;
    case 0x5c:
        cycles = 4;
        break;
    case 0x5d:
        cycles = 4;
        break;
    case 0x5e:
        cycles = 8;
        break;
    case 0x5f:
        cycles = 4;
        break;
    case 0x60:
        cycles = 4;
        break;
    case 0x61:
        cycles = 4;
        break;
    case 0x62:
        cycles = 4;
        break;
    case 0x63:
        cycles = 4;
        break;
    case 0x64:
        cycles = 4;
        break;
    case 0x65:
        cycles = 4;
        break;
    case 0x66:
        cycles = 8;
        break;
    case 0x67:
        cycles = 4;
        break;
    case 0x68:
        cycles = 4;
        break;
    case 0x69:
        cycles = 4;
        break;
    case 0x6a:
        cycles = 4;
        break;
    case 0x6b:
        cycles = 4;
        break;
    case 0x6c:
        cycles = 4;
        break;
    case 0x6d:
        cycles = 4;
        break;
    case 0x6e:
        cycles = 8;
        break;
    case 0x6f:
        cycles = 4;
        break;
    case 0x70:
        cycles = 8;
        break;
    case 0x71:
        cycles = 8;
        break;
    case 0x72:
        cycles = 8;
        break;
    case 0x73:
        cycles = 8;
        break;
    case 0x74:
        cycles = 8;
        break;
    case 0x75:
        cycles = 8;
        break;
    case 0x76:
        cycles = 4;
        break;
    case 0x77:
        cycles = 8;
        break;
    case 0x78:
        cycles = 4;
        break;
    case 0x79:
        cycles = 4;
        break;
    case 0x7a:
        cycles = 4;
        break;
    case 0x7b:
        cycles = 4;
        break;
    case 0x7c:
        cycles = 4;
        break;
    case 0x7d:
        cycles = 4;
        break;
    case 0x7e:
        cycles = 8;
        break;
    case 0x7f:
        cycles = 4;
        break;
    case 0x80:
        cycles = 4;
        break;
    case 0x81:
        cycles = 4;
        break;
    case 0x82:
        cycles = 4;
        break;
    case 0x83:
        cycles = 4;
        break;
    case 0x84:
        cycles = 4;
        break;
    case 0x85:
        cycles = 4;
        break;
    case 0x86:
        cycles = 8;
        break;
    case 0x87:
        cycles = 4;
        break;
    case 0x88:
        cycles = 4;
        break;
    case 0x89:
        cycles = 4;
        break;
    case 0x8a:
        cycles = 4;
        break;
    case 0x8b:
        cycles = 4;
        break;
    case 0x8c:
        cycles = 4;
        break;
    case 0x8d:
        cycles = 4;
        break;
    case 0x8e:
        cycles = 8;
        break;
    case 0x8f:
        cycles = 4;
        break;
    case 0x90:
        cycles = 4;
        break;
    case 0x91:
        cycles = 4;
        break;
    case 0x92:
        cycles = 4;
        break;
    case 0x93:
        cycles = 4;
        break;
    case 0x94:
        cycles = 4;
        break;
    case 0x95:
        cycles = 4;
        break;
    case 0x96:
        cycles = 8;
        break;
    case 0x97:
        cycles = 4;
        break;
    case 0x98:
        cycles = 4;
        break;
    case 0x99:
        cycles = 4;
        break;
    case 0x9a:
        cycles = 4;
        break;
    case 0x9b:
        cycles = 4;
        break;
    case 0x9c:
        cycles = 4;
        break;
    case 0x9d:
        cycles = 4;
        break;
    case 0x9e:
        cycles = 8;
        break;
    case 0x9f:
        cycles = 4;
        break;
    case 0xa0:
        cycles = 4;
        break;
    case 0xa1:
        cycles = 4;
        break;
    case 0xa2:
        cycles = 4;
        break;
    case 0xa3:
        cycles = 4;
        break;
    case 0xa4:
        cycles = 4;
        break;
    case 0xa5:
        cycles = 4;
        break;
    case 0xa6:
        cycles = 8;
        break;
    case 0xa7:
        cycles = 4;
        break;
    case 0xa8:
        cycles = 4;
        break;
    case 0xa9:
        cycles = 4;
        break;
    case 0xaa:
        cycles = 4;
        break;
    case 0xab:
        cycles = 4;
        break;
    case 0xac:
        cycles = 4;
        break;
    case 0xad:
        cycles = 4;
        break;
    case 0xae:
        cycles = 8;
        break;
    case 0xaf:
        cycles = 4;
        break;
    case 0xb0:
        cycles = 4;
        break;
    case 0xb1:
        cycles = 4;
        break;
    case 0xb2:
        cycles = 4;
        break;
    case 0xb3:
        cycles = 4;
        break;
    case 0xb4:
        cycles = 4;
        break;
    case 0xb5:
        cycles = 4;
        break;
    case 0xb6:
        cycles = 8;
        break;
    case 0xb7:
        cycles = 4;
        break;
    case 0xb8:
        cycles = 4;
        break;
    case 0xb9:
        cycles = 4;
        break;
    case 0xba:
        cycles = 4;
        break;
    case 0xbb:
        cycles = 4;
        break;
    case 0xbc:
        cycles = 4;
        break;
    case 0xbd:
        cycles = 4;
        break;
    case 0xbe:
        cycles = 8;
        break;
    case 0xbf:
        cycles = 4;
        break;
    case 0xc0:
        cycles = z == 0 ? 20 : 8;
        break;
    case 0xc1:
        cycles = 12;
        break;
    case 0xc2:
        cycles = z == 0 ? 16 : 8;
        break;
    case 0xc3:
        cycles = 16;
        break;
    case 0xc4:
        cycles = z == 0 ? 24 : 8;
        break;
    case 0xc5:
        cycles = 16;
        break;
    case 0xc6:
        cycles = 8;
        break;
    case 0xc7:
        cycles = 16;
        break;
    case 0xc8:
        cycles = z == 1 ? 20 : 8;
        break;
    case 0xc9:
        cycles = 16;
        break;
    case 0xca:
        cycles = z == 1 ? 16 : 8;
        break;
    case 0xcb:
        if ((cbopcode & 0xf) == 0x6 || (cbopcode & 0xf) == 0xe) {
            cycles = 16;
        } else {
            cycles = 8;
        }
        break;
    case 0xcc:
        cycles = z == 1 ? 24 : 8;
        break;
    case 0xcd:
        cycles = 24;
        break;
    case 0xce:
        cycles = 8;
        break;
    case 0xcf:
        cycles = 16;
        break;
    case 0xd0:
        cycles = c == 0 ? 20 : 8;
        break;
    case 0xd1:
        cycles = 12;
        break;
    case 0xd2:
        cycles = c == 0 ? 16 : 8;
        break;
    case 0xd4:
        cycles = c == 0 ? 24 : 8;
        break;
    case 0xd5:
        cycles = 16;
        break;
    case 0xd6:
        cycles = 8;
        break;
    case 0xd7:
        cycles = 16;
        break;
    case 0xd8:
        cycles = c == 1 ? 20 : 8;
        break;
    case 0xd9:
        cycles = 16;
        break;
    case 0xda:
        cycles = c == 1 ? 16 : 8;
        break;
    case 0xdc:
        cycles = c == 1 ? 24 : 8;
        break;
    case 0xde:
        cycles = 8;
        break;
    case 0xdf:
        cycles = 16;
        break;
    case 0xe0:
        cycles = 12;
        break;
    case 0xe1:
        cycles = 12;
        break;
    case 0xe2:
        cycles = 8;
        break;
    case 0xe5:
        cycles = 16;
        break;
    case 0xe6:
        cycles = 8;
        break;
    case 0xe7:
        cycles = 16;
        break;
    case 0xe8:
        cycles = 16;
        break;
    case 0xe9:
        cycles = 4;
        break;
    case 0xea:
        cycles = 16;
        break;
    case 0xee:
        cycles = 8;
        break;
    case 0xef:
        cycles = 16;
        break;
    case 0xf0:
        cycles = 12;
        break;
    case 0xf1:
        cycles = 12;
        break;
    case 0xf2:
        cycles = 8;
        break;
    case 0xf3:
        cycles = 4;
        break;
    case 0xf5:
        cycles = 16;
        break;
    case 0xf6:
        cycles = 8;
        break;
    case 0xf7:
        cycles = 16;
        break;
    case 0xf8:
        cycles = 12;
        break;
    case 0xf9:
        cycles = 8;
        break;
    case 0xfa:
        cycles = 16;
        break;
    case 0xfb:
        cycles = 4;
        break;
    case 0xfe:
        cycles = 8;
        break;
    case 0xff:
        cycles = 16;
        break;
    default:
        assert(false);
    }
    return cycles;
}
