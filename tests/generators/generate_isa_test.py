import struct

e_ident = b'\x7FELF\x01\x02\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00'
elf_header = struct.pack(">16sHHIIIIIHHHHHH", e_ident, 2, 8, 1, 0x00400000, 52, 0, 0, 52, 32, 1, 40, 0, 0)
program_header = struct.pack(">IIIIIIII", 1, 84, 0x00400000, 0x00400000, 60, 60, 5, 0x1000)

instructions = struct.pack(">15I",
    0x3C080040,  # lui $8, 0x0040       -> Set up memory address top half
    0x35080044,  # ori $8, $8, 0x0044   -> Complete address (0x00400044)
    0x2009002A,  # addi $9, $0, 42      -> $9 = 42
    0xAD090000,  # sw $9, 0($8)         -> Store 42 into memory at 0x00400044
    0x8D0A0000,  # lw $10, 0($8)        -> Load 42 from memory into $10
    0x200B000A,  # addi $11, $0, 10     -> $11 = 10
    0x014B6022,  # sub $12, $10, $11    -> $12 = 42 - 10 = 32
    0x014B6824,  # and $13, $10, $11    -> $13 = 42 & 10 = 10
    0x014B7025,  # or $14, $10, $11     -> $14 = 42 | 10 = 42
    0x016A782A,  # slt $15, $11, $10    -> $15 = (10 < 42) ? 1 : 0
    0x0C10000D,  # jal 0x00400034       -> Jump and Link to instruction [13]
    0x2002000A,  # addi $2, $0, 10      -> Syscall 10 (Exit) - We return here!
    0x0000000C,  # syscall              -> Terminate
    
    # sub func
    0x20100063,  # addi $16, $0, 99     -> $16 = 99 (Proof we made it here)
    0x03E00008   # jr $31               -> Jump Register ($ra) to return
)

with open("tests/isa_test.elf", "wb") as f:
    f.write(elf_header + program_header + instructions)
print("Created tests/isa_test.elf")