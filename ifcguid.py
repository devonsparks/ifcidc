import string

ct = string.digits + string.ascii_uppercase + string.ascii_lowercase + '_$'



def encode(chunk):
    """
    Given a list of 2-length character strings from an 
    uncompressed IFC GUID, return a string representing 
    the list's contents compressed according to the IFC 
    GUID mapping (ct).

    encode() and decode() are inverses, 
    e.g.,  encode([decode(['1s'])]) == '1s'
    """
    r = []
    n = int(''.join(chunk), 16)
    while n > 0:
        r.append(ct[n%64])
        n = n >> 6
    return ''.join(reversed(r))



def decode(chunk):
    """
    Given a list of 2-length character strings from an 
    compressed IFC GUID, return a string representing 
    the list's contents /de/compressed according to the 
    IFC oGUID mapping (ct).
    """
    n = 0
    chunk = ''.join(chunk)
    for c in chunk[:-1]:
        n = (n + ct.index(c))<<6
    n += ct.index(chunk[-1])
    return format(n,'x')



def compress(s):
    """
    Given an uncompress IFC GUID string, 
    returned its compressed equivalent
    as a string.

    compress() and decompress are inverses, 
    i.e., let s = decompress(compress(s))
    """
    s = s.replace('-','')
    bytes = map(''.join, zip(*[iter(s)]*2))
    r = [encode(bytes[0:1]),  \
         encode(bytes[1:4]),  \
         encode(bytes[4:7]),  \
         encode(bytes[7:10]), \
         encode(bytes[10:13]),\
         encode(bytes[13:16])]
    return ''.join(r)



def decompress(s):
    """
    Given a compressed IFC GUID string,
    return its uncompressed equivalent 
    as a string.
    """
    parts = map(''.join, zip(*[iter(s)]*2))
    r = ''.join([decode(parts[0:1]), \
                 decode(parts[1:3]), \
                 decode(parts[3:5]), \
                 decode(parts[5:7]), \
                 decode(parts[7:9]), \
                 decode(parts[9:12])])
    return [:8] + "-" + r[8:12] + "-" + r[12:16] + "-" + r[16:20] + "-" + r[20:]
