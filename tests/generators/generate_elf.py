import struct

# ELF Header
# \x7FELF, 32-bit (1), Big-Endian (2), Version (1), System V ABI (0), Pad
e_ident = b'\x7FELF\x01\x02\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00'
e_type = 2            # executable
e_machine = 8         # MIPS archi
e_version = 1         # current version
e_entry = 0x00400000  # standard MIPS virtual entry point
e_phoff = 52          # program header immediately follows the 52-byte ELF header
e_shoff = 0           # no section headers
e_flags = 0
e_ehsize = 52         # size of ELF header
e_phentsize = 32      # size of one program header
e_phnum = 1           # num of program headers
e_shentsize = 40      # size of hection header
e_shnum = 0
e_shstrndx = 0

# Pack into a Big-Endian (>) struct
elf_header = struct.pack(">16sHHIIIIIHHHHHH",
    e_ident, e_type, e_machine, e_version, e_entry, e_phoff,
    e_shoff, e_flags, e_ehsize, e_phentsize, e_phnum,
    e_shentsize, e_shnum, e_shstrndx)

# Program header (32 bytes)
p_type = 1            # PT_LOAD (Tells the OS to load this into RAM)
p_offset = 84         # start of instrctn in file (52 + 32 = 84)
p_vaddr = 0x00400000  # virtual addr in RAM
p_paddr = 0x00400000  # physical addr (same as virtual here)
p_filesz = 20         # size in file
p_memsz = 20          # size in RAM
p_flags = 5           # read + exec permissions
p_align = 0x1000

program_header = struct.pack(">IIIIIIII",
    p_type, p_offset, p_vaddr, p_paddr, p_filesz, p_memsz, p_flags, p_align)

instructions = struct.pack(">IIIII", 
    0x20020005, # addi $2, $0, 5
    0x2004000A, # addi $4, $0, 10
    0x00441020, # add  $2, $2, $4
    0x2002000A, # addi $2, $0, 10 (Syscall Exit)
    0x0000000C  # syscall
)

with open("tests/dummy.elf", "wb") as f:
    f.write(elf_header)
    f.write(program_header)
    f.write(instructions)

print("Created tests/dummy.elf successfully!")