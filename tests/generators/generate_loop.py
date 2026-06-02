import struct

e_ident = b'\x7FELF\x01\x02\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00'
elf_header = struct.pack(">16sHHIIIIIHHHHHH", e_ident, 2, 8, 1, 0x00400000, 52, 0, 0, 52, 32, 1, 40, 0, 0)
program_header = struct.pack(">IIIIIIII", 1, 84, 0x00400000, 0x00400000, 20, 20, 5, 0x1000)

instructions = struct.pack(">IIIII", 
    0x20020005, # addi $2, $0, 5
    0x2042FFFF, # addi $2, $2, -1
    0x1440FFFE, # bne  $2, $0, -2
    0x2002000A, # addi $2, $0, 10 (Syscall Exit)
    0x0000000C  # syscall
)

with open("tests/loop.elf", "wb") as f:
    f.write(elf_header + program_header + instructions)
print("Created tests/loop.elf")