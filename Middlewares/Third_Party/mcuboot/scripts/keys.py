import sys
from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives.asymmetric import ec
from cryptography.hazmat.primitives.hashes import SHA256
from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives.asymmetric import ed25519
from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives.asymmetric import rsa
from cryptography.hazmat.primitives.asymmetric.padding import PSS, MGF1
from cryptography.hazmat.primitives.hashes import SHA256
from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives.asymmetric.rsa import RSAPrivateKey, RSAPublicKey
from cryptography.hazmat.primitives.asymmetric.ec import EllipticCurvePrivateKey, EllipticCurvePublicKey
from cryptography.hazmat.primitives.asymmetric.ed25519 import Ed25519PrivateKey, Ed25519PublicKey
AUTOGEN_MESSAGE = "/* Autogenerated by imgtool.py, do not edit. */"

 
class KeyClass(object):
    def _emit(self, header, trailer, encoded_bytes, indent, file=sys.stdout, len_format=None):
        print(AUTOGEN_MESSAGE, file=file)
        print(header, end='', file=file)
        for count, b in enumerate(encoded_bytes):
            if count % 8 == 0:
                print("\n" + indent, end='', file=file)
            else:
                print(" ", end='', file=file)
            print("0x{:02x},".format(b), end='', file=file)
        print("\n" + trailer, file=file)
        if len_format is not None:
            print(len_format.format(len(encoded_bytes)), file=file)

    def emit_c_public(self, file=sys.stdout):
        self._emit(
                header="const unsigned char {}_pub_key[] = {{".format(self.shortname()),
                trailer="};",
                encoded_bytes=self.get_public_bytes(),
                indent="    ",
                len_format="const unsigned int {}_pub_key_len = {{}};".format(self.shortname()),
                file=file)

    def emit_rust_public(self, file=sys.stdout):
        self._emit(
                header="static {}_PUB_KEY: &'static [u8] = &[".format(self.shortname().upper()),
                trailer="];",
                encoded_bytes=self.get_public_bytes(),
                indent="    ",
                file=file)

    def emit_private(self, minimal, file=sys.stdout):
        self._emit(
                header="const unsigned char enc_priv_key[] = {",
                trailer="};",
                encoded_bytes=self.get_private_bytes(minimal),
                indent="    ",
                len_format="const unsigned int enc_priv_key_len = {};",
                file=file)


class ECDSAUsageError(Exception):
    pass

class ECDSA256P1Public(KeyClass):
    def __init__(self, key):
        self.key = key

    def shortname(self):
        return "ecdsa"

    def _unsupported(self, name):
        raise ECDSAUsageError("Operation {} requires private key".format(name))

    def _get_public(self):
        return self.key

    def get_public_bytes(self):
        # The key is embedded into MBUboot in "SubjectPublicKeyInfo" format
        return self._get_public().public_bytes(
                encoding=serialization.Encoding.DER,
                format=serialization.PublicFormat.SubjectPublicKeyInfo)

    def get_private_bytes(self, minimal):
        self._unsupported('get_private_bytes')

    def export_private(self, path, passwd=None):
        self._unsupported('export_private')

    def export_public(self, path):
        """Write the public key to the given file."""
        pem = self._get_public().public_bytes(
                encoding=serialization.Encoding.PEM,
                format=serialization.PublicFormat.SubjectPublicKeyInfo)
        with open(path, 'wb') as f:
            f.write(pem)

    def sig_type(self):
        return "ECDSA256_SHA256"

    def sig_tlv(self):
        return "ECDSA256"

    def sig_len(self):
        # Early versions of MCUboot (< v1.5.0) required ECDSA
        # signatures to be padded to 72 bytes.  Because the DER
        # encoding is done with signed integers, the size of the
        # signature will vary depending on whether the high bit is set
        # in each value.  This padding was done in a
        # not-easily-reversible way (by just adding zeros).
        #
        # The signing code no longer requires this padding, and newer
        # versions of MCUboot don't require it.  But, continue to
        # return the total length so that the padding can be done if
        # requested.
        return 72

    def verify(self, signature, payload):
        # strip possible paddings added during sign
        signature = signature[:signature[1] + 2]
        k = self.key
        if isinstance(self.key, ec.EllipticCurvePrivateKey):
            k = self.key.public_key()
        return k.verify(signature=signature, data=payload,
                        signature_algorithm=ec.ECDSA(SHA256()))


class ECDSA256P1(ECDSA256P1Public):
    """
    Wrapper around an ECDSA private key.
    """

    def __init__(self, key):
        """key should be an instance of EllipticCurvePrivateKey"""
        self.key = key
        self.pad_sig = False

    @staticmethod
    def generate():
        pk = ec.generate_private_key(
                ec.SECP256R1(),
                backend=default_backend())
        return ECDSA256P1(pk)

    def _get_public(self):
        return self.key.public_key()

    def _build_minimal_ecdsa_privkey(self, der):
        '''
        Builds a new DER that only includes the EC private key, removing the
        public key that is added as an "optional" BITSTRING.
        '''
        offset_PUB = 68
        EXCEPTION_TEXT = "Error parsing ecdsa key. Please submit an issue!"
        if der[offset_PUB] != 0xa1:
            raise ECDSAUsageError(EXCEPTION_TEXT)
        len_PUB = der[offset_PUB + 1]
        b = bytearray(der[:-offset_PUB])
        offset_SEQ = 29
        if b[offset_SEQ] != 0x30:
            raise ECDSAUsageError(EXCEPTION_TEXT)
        b[offset_SEQ + 1] -= len_PUB
        offset_OCT_STR = 27
        if b[offset_OCT_STR] != 0x04:
            raise ECDSAUsageError(EXCEPTION_TEXT)
        b[offset_OCT_STR + 1] -= len_PUB
        if b[0] != 0x30 or b[1] != 0x81:
            raise ECDSAUsageError(EXCEPTION_TEXT)
        b[2] -= len_PUB
        return b

    def get_private_bytes(self, minimal):
        priv = self.key.private_bytes(
                encoding=serialization.Encoding.DER,
                format=serialization.PrivateFormat.PKCS8,
                encryption_algorithm=serialization.NoEncryption())
        if minimal:
            priv = self._build_minimal_ecdsa_privkey(priv)
        return priv

    def export_private(self, path, passwd=None):
        """Write the private key to the given file, protecting it with the optional password."""
        if passwd is None:
            enc = serialization.NoEncryption()
        else:
            enc = serialization.BestAvailableEncryption(passwd)
        pem = self.key.private_bytes(
                encoding=serialization.Encoding.PEM,
                format=serialization.PrivateFormat.PKCS8,
                encryption_algorithm=enc)
        with open(path, 'wb') as f:
            f.write(pem)

    def raw_sign(self, payload):
        """Return the actual signature"""
        return self.key.sign(
                data=payload,
                signature_algorithm=ec.ECDSA(SHA256()))

    def sign(self, payload):
        sig = self.raw_sign(payload)
        if self.pad_sig:
            # To make fixed length, pad with one or two zeros.
            sig += b'\000' * (self.sig_len() - len(sig))
            return sig
        else:
            return sig
            
 # Sizes that bootutil will recognize
RSA_KEY_SIZES = [2048, 3072]


class RSAUsageError(Exception):
    pass


class RSAPublic(KeyClass):
    """The public key can only do a few operations"""
    def __init__(self, key):
        self.key = key

    def key_size(self):
        return self.key.key_size

    def shortname(self):
        return "rsa"

    def _unsupported(self, name):
        raise RSAUsageError("Operation {} requires private key".format(name))

    def _get_public(self):
        return self.key

    def get_public_bytes(self):
        # The key embedded into MCUboot is in PKCS1 format.
        return self._get_public().public_bytes(
                encoding=serialization.Encoding.DER,
                format=serialization.PublicFormat.PKCS1)

    def get_private_bytes(self, minimal):
        self._unsupported('get_private_bytes')

    def export_private(self, path, passwd=None):
        self._unsupported('export_private')

    def export_public(self, path):
        """Write the public key to the given file."""
        pem = self._get_public().public_bytes(
                encoding=serialization.Encoding.PEM,
                format=serialization.PublicFormat.SubjectPublicKeyInfo)
        with open(path, 'wb') as f:
            f.write(pem)

    def sig_type(self):
        return "PKCS1_PSS_RSA{}_SHA256".format(self.key_size())

    def sig_tlv(self):
        return"RSA{}".format(self.key_size())

    def sig_len(self):
        return self.key_size() / 8

    def verify(self, signature, payload):
        k = self.key
        if isinstance(self.key, rsa.RSAPrivateKey):
            k = self.key.public_key()
        return k.verify(signature=signature, data=payload,
                        padding=PSS(mgf=MGF1(SHA256()), salt_length=32),
                        algorithm=SHA256())


class RSA(RSAPublic):
    """
    Wrapper around an RSA key, with imgtool support.
    """

    def __init__(self, key):
        """The key should be a private key from cryptography"""
        self.key = key

    @staticmethod
    def generate(key_size=2048):
        if key_size not in RSA_KEY_SIZES:
            raise RSAUsageError("Key size {} is not supported by MCUboot"
                                .format(key_size))
        pk = rsa.generate_private_key(
                public_exponent=65537,
                key_size=key_size,
                backend=default_backend())
        return RSA(pk)

    def _get_public(self):
        return self.key.public_key()

    def _build_minimal_rsa_privkey(self, der):
        '''
        Builds a new DER that only includes N/E/D/P/Q RSA parameters;
        standard DER private bytes provided by OpenSSL also includes
        CRT params (DP/DQ/QP) which can be removed.
        '''
        OFFSET_N = 7  # N is always located at this offset
        b = bytearray(der)
        off = OFFSET_N
        if b[off + 1] != 0x82:
            raise RSAUsageError("Error parsing N while minimizing")
        len_N = (b[off + 2] << 8) + b[off + 3] + 4
        off += len_N
        if b[off + 1] != 0x03:
            raise RSAUsageError("Error parsing E while minimizing")
        len_E = b[off + 2] + 4
        off += len_E
        if b[off + 1] != 0x82:
            raise RSAUsageError("Error parsing D while minimizing")
        len_D = (b[off + 2] << 8) + b[off + 3] + 4
        off += len_D
        if b[off + 1] != 0x81:
            raise RSAUsageError("Error parsing P while minimizing")
        len_P = b[off + 2] + 3
        off += len_P
        if b[off + 1] != 0x81:
            raise RSAUsageError("Error parsing Q while minimizing")
        len_Q = b[off + 2] + 3
        off += len_Q
        # adjust DER size for removed elements
        b[2] = (off - 4) >> 8
        b[3] = (off - 4) & 0xff
        return b[:off]

    def get_private_bytes(self, minimal):
        priv = self.key.private_bytes(
                encoding=serialization.Encoding.DER,
                format=serialization.PrivateFormat.TraditionalOpenSSL,
                encryption_algorithm=serialization.NoEncryption())
        if minimal:
            priv = self._build_minimal_rsa_privkey(priv)
        return priv

    def export_private(self, path, passwd=None):
        """Write the private key to the given file, protecting it with the
        optional password."""
        if passwd is None:
            enc = serialization.NoEncryption()
        else:
            enc = serialization.BestAvailableEncryption(passwd)
        pem = self.key.private_bytes(
                encoding=serialization.Encoding.PEM,
                format=serialization.PrivateFormat.PKCS8,
                encryption_algorithm=enc)
        with open(path, 'wb') as f:
            f.write(pem)

    def sign(self, payload):
        # The verification code only allows the salt length to be the
        # same as the hash length, 32.
        return self.key.sign(
                data=payload,
                padding=PSS(mgf=MGF1(SHA256()), salt_length=32),
                algorithm=SHA256())
                
class PasswordRequired(Exception):
    """Raised to indicate that the key is password protected, but a
    password was not specified."""
    pass

def load(path, passwd=None):
    """Try loading a key from the given path.  Returns None if the password wasn't specified."""
    with open(path, 'rb') as f:
        raw_pem = f.read()
    try:
        pk = serialization.load_pem_private_key(
                raw_pem,
                password=passwd,
                backend=default_backend())
    # Unfortunately, the crypto library raises unhelpful exceptions,
    # so we have to look at the text.
    except TypeError as e:
        msg = str(e)
        if "private key is encrypted" in msg:
            return None
        raise e
    except ValueError:
        # This seems to happen if the key is a public key, let's try
        # loading it as a public key.
        pk = serialization.load_pem_public_key(
                raw_pem,
                backend=default_backend())

    if isinstance(pk, RSAPrivateKey):
        if pk.key_size not in RSA_KEY_SIZES:
            raise Exception("Unsupported RSA key size: " + pk.key_size)
        return RSA(pk)
    elif isinstance(pk, RSAPublicKey):
        if pk.key_size not in RSA_KEY_SIZES:
            raise Exception("Unsupported RSA key size: " + pk.key_size)
        return RSAPublic(pk)
    elif isinstance(pk, EllipticCurvePrivateKey):
        if pk.curve.name != 'secp256r1':
            raise Exception("Unsupported EC curve: " + pk.curve.name)
        if pk.key_size != 256:
            raise Exception("Unsupported EC size: " + pk.key_size)
        return ECDSA256P1(pk)
    elif isinstance(pk, EllipticCurvePublicKey):
        if pk.curve.name != 'secp256r1':
            raise Exception("Unsupported EC curve: " + pk.curve.name)
        if pk.key_size != 256:
            raise Exception("Unsupported EC size: " + pk.key_size)
        return ECDSA256P1Public(pk)
    else:
        raise Exception("Unknown key type: " + str(type(pk)))