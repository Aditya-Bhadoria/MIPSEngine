import struct

e_ident = b'\x7FELF\x01\x02\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00'
elf_header = struct.pack(">16sHHIIIIIHHHHHH", e_ident, 2, 8, 1, 0x00400000, 52, 0, 0, 52, 32, 1, 40, 0, 0)
program_header = struct.pack(">IIIIIIII", 1, 84, 0x00400000, 0x00400000, 60, 60, 5, 0x1000)

instructions = struct.pack(">15I",
    0x2008000A, # addi $8, $0, 10
    0x20090003, # addi $9, $0, 3
    
    # Test Multiply
    0x01090018, # mult $8, $9
    0x00002012, # mflo $4            -> Move 30 to $a0 (for printing)
    0x20020001, # addi $2, $0, 1     -> Syscall 1 (Print Integer)
    0x0000000C, # syscall            -> Should print '30'
    
    # Test Divide
    0x0109001A, # div $8, $9
    0x00002012, # mflo $4            -> Move Quotient (3) to $a0
    0x20020001, # addi $2, $0, 1     
    0x0000000C, # syscall            -> Should print '3'
    
    0x00002010, # mfhi $4            -> Move Remainder (1) to $a0
    0x20020001, # addi $2, $0, 1     
    0x0000000C, # syscall            -> Should print '1'
    
    # Exit
    0x2002000A, # addi $2, $0, 10    -> Syscall 10 (exit)
    0x0000000C  # syscall
)

with open("tests/math.elf", "wb") as f:
    f.write(elf_header + program_header + instructions)
print("Created tests/math.elf")