// for samp compatibility: TEA block encryptor

#ifndef __TEA_BLOCK_ENCRYPTOR_H
#define __TEA_BLOCK_ENCRYPTOR_H

class TEABlockEncryptor
{
public:
    TEABlockEncryptor();
    ~TEABlockEncryptor();

    bool IsKeySet(void) const;
    void SetKey(const unsigned char key[16]);
    void UnsetKey(void);
    void Encrypt(unsigned char* input, int inputLength, unsigned char* output, int* outputLength);
    bool Decrypt(unsigned char* input, int inputLength, unsigned char* output, int* outputLength);

protected:
    bool keySet;
    unsigned char key[16];
    unsigned int initSum;
    unsigned int initDelta;

    static unsigned int initObsDelta;

    void EncryptBlock(unsigned int& V0, unsigned int& V1);
    void DecryptBlock(unsigned int& V0, unsigned int& V1);
};

#endif
