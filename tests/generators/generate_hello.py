import struct

e_ident = b'\x7FELF\x01\x02\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00'
elf_header = struct.pack(">16sHHIIIIIHHHHHH", e_ident, 2, 8, 1, 0x00400000, 52, 0, 0, 52, 32, 1, 40, 0, 0)
program_header = struct.pack(">IIIIIIII", 1, 84, 0x00400000, 0x00400000, 48, 48, 5, 0x1000)

instructions = struct.pack(">IIIIII", 
    0x3C040040, # lui $4, 0x0040
    0x34840018, # ori $4, $4, 0x0018
    0x20020004, # addi $2, $0, 4  -> Syscall 4 (print string)
    0x0000000C, # syscall
    0x2002000A, # addi $2, $0, 10
    0x0000000C  # syscall
)

string_data = b"Hello from MIPSEngine!\n\0\0" 

with open("tests/hello.elf", "wb") as f:
    f.write(elf_header + program_header + instructions + string_data)
print("Created tests/hello.elf")