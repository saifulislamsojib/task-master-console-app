#define XOR_KEY 0x5A /* simple XOR cipher key for passwords   */

void xor_encrypt(char *text, char *result, int len) {
  for (int i = 0; i < len; i++)
    result[i] = text[i] ^ XOR_KEY;
  result[len] = '\0';
}

void xor_decrypt(char *cipher, char *result, int len) {
  /* XOR is symmetric – same operation decrypts */
  xor_encrypt(cipher, result, len);
}