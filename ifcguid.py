
b64 = '0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_$'
b16 = '0123456789ABCDEF'

def compress(s):
    s = s.replace('-', '')
    # TODO: check valid ascii
    return ccompress(s)

def ccompress(s):
    assert(len(s)==32)
    s = '0' + s
    oi = 0
    output = [0] * 22
    for i in range(0,33,3):
        n = int(s[i], 16)*(16**2) + int(s[i+1], 16)*16 + int(s[i+2], 16)
        output[oi+1] = b64[n%64]
        output[oi] = b64[n/64]
        oi += 2
    return ''.join(output)

def cdecompress(s):
    assert(len(s)==22)
    oi = 0
    output = [0] * 33
    for i in range(0, 22, 2):
        n = b64.index(s[i])*64 + b64.index(s[i+1])
        t = n>>4
        output[oi+2] = b16[n%16]
        output[oi+1] = b16[t%16]
        output[oi] = b16[t>>4]
        oi += 3
    return ''.join(output[1:])

def decompress(s):
    s = cdecompress(s)
    return s[:8]+"-"+ s[8:12] + "-" + s[12:16] + "-" + s[16:20] + "-" + s[20:]
