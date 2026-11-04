# Prints the size and FNV-1a 64 hash of CNWSObject::UpdateEffectList (or another symbol) in an nwserver-linux binary:
# the values EffectListCache checks at startup. usage: python fingerprint.py <nwserver-linux> [mangled symbol]
import struct, sys
path = sys.argv[1]
name = sys.argv[2] if len(sys.argv) > 2 else '_ZN10CNWSObject16UpdateEffectListEjj'
b = open(path, 'rb').read()
assert b[:4] == b'\x7fELF' and b[4] == 2
e_phoff, e_shoff = struct.unpack_from('<QQ', b, 0x20)
e_phentsize, e_phnum, e_shentsize, e_shnum, e_shstrndx = struct.unpack_from('<HHHHH', b, 0x36)
e_type = struct.unpack_from('<H', b, 0x10)[0]
secs = [struct.unpack_from('<IIQQQQIIQQ', b, e_shoff + i * e_shentsize) for i in range(e_shnum)]
found = []
for s in secs:
    if s[1] in (2, 11):  # SYMTAB, DYNSYM
        strtab = secs[s[6]]
        for off in range(s[4], s[4] + s[5], 24):
            st_name, st_info, st_other, st_shndx, st_value, st_size = struct.unpack_from('<IBBHQQ', b, off)
            o = strtab[4] + st_name
            n = b[o:b.index(b'\0', o)].decode()
            if n == name:
                found.append((st_value, st_size, s[1]))
phs = [struct.unpack_from('<IIQQQQQQ', b, e_phoff + i * e_phentsize) for i in range(e_phnum)]
for v, size, kind in sorted(set(found)):
    for p in phs:
        if p[0] == 1 and p[3] <= v < p[3] + p[5]:
            fo = v - p[3] + p[2]
            data = b[fo:fo + size]
            h = 0xcbf29ce484222325
            for x in data:
                h = ((h ^ x) * 0x100000001b3) & 0xffffffffffffffff
            print('e_type=%d %s vaddr=%#x size=%d fnv1a=%016x first=%s' % (e_type, 'dynsym' if kind == 11 else 'symtab', v, size, h, data[:16].hex()))
