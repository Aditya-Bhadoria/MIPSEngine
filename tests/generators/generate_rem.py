import struct

e_ident = b'\x7FELF\x01\x02\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00'
elf_header = struct.pack(">16sHHIIIIIHHHHHH", e_ident, 2, 8, 1, 0x00400000, 52, 0, 0, 52, 32, 1, 40, 0, 0)
program_header = struct.pack(">IIIIIIII", 1, 84, 0x00400000, 0x00400000, 52, 52, 5, 0x1000)

instructions = struct.pack(">13I",
    # Test Shifts 
    0x2008000F,  # addi $8, $0, 15    -> $8 = 15 (binary: 1111)
    0x00084880,  # sll $9, $8, 2      -> $9 = 60 (binary: 111100)
    0x00085082,  # srl $10, $8, 1     -> $10 = 7 (binary: 111)

    # Test Sub-Word Memory (Byte Access)
    0x3C0B0050,  # lui $11, 0x0050
    0x200C00AA,  # addi $12, $0, 0xAA -> $12 = 170 (negative when viewed as a signed 8-bit byte)
    0xA16C0000,  # sb $12, 0($11)     -> Store the lowest byte (0xAA) into memory
    0x816D0000,  # lb $13, 0($11)     -> Load byte (sign-extended) -> should be 0xFFFFFFAA
    0x916E0000,  # lbu $14, 0($11)    -> Load byte unsigned -> should be 0x000000AA

    # Test REGIMM Branches
    0x05A00002,  # bltz $13, 2        -> Jumps to 0x2002000A
    
    # If the branch fails, the emulator hits this and halts with a break trace.
    0x0000000D,  # break
    0x0000000D,  # break

    # Clean Exit
    0x2002000A,  # addi $2, $0, 10    -> Syscall 10 (Exit)
    0x0000000C   # syscall
)

with open("tests/rem.elf", "wb") as f:
    f.write(elf_header + program_header + instructions)
print("Created tests/rem.elf")