
import os
import sys
import argparse
from cryptography.hazmat.primitives import hashes
from cryptography.hazmat.primitives.asymmetric import padding
from cryptography.hazmat.primitives import serialization

def sign_firmware(binary_path, key_path, output_path):
    print(f"Signing {binary_path}...")
    
    # Read binary
    try:
        with open(binary_path, "rb") as f:
            binary_data = f.read()
    except FileNotFoundError:
        print(f"Error: File not found: {binary_path}")
        return

    # Read private key
    try:
        with open(key_path, "rb") as f:
            private_key = serialization.load_pem_private_key(
                f.read(),
                password=None
            )
    except Exception as e:
        print(f"Error loading private key: {e}")
        return

    # Calculate SHA256 hash of the binary
    digest = hashes.Hash(hashes.SHA256())
    digest.update(binary_data)
    hash_to_sign = digest.finalize()

    # Sign the hash
    signature = private_key.sign(
        binary_data, # For RSA PKCS#1 v1.5 with SHA256, standard padding
        padding.PKCS1v15(),
        hashes.SHA256()
    )

    # Append signature to binary
    signed_data = binary_data + signature
    
    with open(output_path, "wb") as f:
        f.write(signed_data)
        
    print(f"Success! Created {output_path}")
    print(f"Original size: {len(binary_data)} bytes")
    print(f"Signed size:   {len(signed_data)} bytes (Signature: {len(signature)} bytes)")

if __name__ == "__main__":
    if len(sys.argv) < 4:
        print("Usage: python sign_firmware.py <input_bin> <private_key_pem> <output_bin>")
        sys.exit(1)
        
    sign_firmware(sys.argv[1], sys.argv[2], sys.argv[3])
