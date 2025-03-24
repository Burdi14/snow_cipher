#include <iostream>
#include <cstdint>
#include <vector>
#include <random>

#define ROTL32(x, shift) ((x << shift) | (x >> (32 - shift)))
#define ROTL8(x, shift) ((x << shift) | (x >> (8 - shift)))

class SNOW {
private:
    uint32_t LFSR[16];
    uint32_t FSM_R1, FSM_R2;
    std::vector<uint32_t> keystream;
    uint8_t SBOX[256];

    void static initialize_sbox(uint8_t sbox[256]) {
        uint8_t p = 1, q = 1;
        do {
            p = p ^ (p << 1) ^ (p & 0x80 ? 0x1B : 0);
            q ^= q << 1;
            q ^= q << 2;
            q ^= q << 4;
            q ^= q & 0x80 ? 0x09 : 0;
            uint8_t xformed = q ^ ROTL8(q, 1) ^ ROTL8(q, 2) ^ ROTL8(q, 3) ^ ROTL8(q, 4);
            sbox[p] = xformed ^ 0x63;
        } while (p != 1);
        sbox[0] = 0x63;
    }

    uint32_t mul_alpha(uint32_t x) {
        return (((x << 9) ^ (x >> 23)) & 0xFFFFFFFF);
    }

    uint32_t S(uint32_t R) {
        return ((SBOX[(R >> 24) & 0xFF] << 24) |
                (SBOX[(R >> 16) & 0xFF] << 16) |
                (SBOX[(R >> 8) & 0xFF] << 8) |
                (SBOX[R & 0xFF]));
    }

    void LFSR_Update() {
        uint32_t new_value = mul_alpha((LFSR[0] ^ LFSR[2] ^ LFSR[11] ^ LFSR[15]));
        for (int i = 0; i < 15; i++) {
            LFSR[i] = LFSR[i + 1];
        }
        LFSR[15] = new_value;
    }

    void FSM_Update() {
        uint32_t F = ((LFSR[15] + FSM_R1) & 0xFFFFFFFF) ^ FSM_R2;
        uint32_t FSM_R1_prev = FSM_R1;
        FSM_R1 = FSM_R1_prev ^ ROTL32((F + FSM_R2), 7);
        FSM_R2 = S(FSM_R1_prev);
    }

public:
    void Encrypt(std::string &text) {
        for (size_t i = 0; i < text.size(); i++) {
            if (i % 4 == 0) {
                keystream.push_back(GenerateKeystreamWord());
            }
            text[i] ^= (keystream.back() >> (8 * (i % 4))) & 0xFF;
        }
    }

    explicit SNOW(std::vector<uint32_t> key) {
        initialize_sbox(SBOX);
        for (int i = 0; i < 16; i++) {
            LFSR[i] = key[0] ^ (i * 0x01010101);
        }
        FSM_R1 = key[1];
        FSM_R2 = key[2];
    }

    uint32_t GenerateKeystreamWord() {
        uint32_t keystream_word = ((LFSR[15] + FSM_R1) & 0xFFFFFFFF) ^ FSM_R2 ^ LFSR[0];
        FSM_Update();
        LFSR_Update();
        return keystream_word;
    }
};

void PrintAsHex(const std::string &data) {
    for (unsigned char c : data) {
        if (c < 16){
            std::cout << "0";
        }
        std::cout << std::hex << (int)c;
    }
    std::cout << std::endl;
}

uint32_t random_uint32() {
    static std::random_device rd;
    static std::mt19937 mt(rd());
    std::uniform_int_distribution<uint32_t> dist(0, 0xFFFFFFFF);
    return dist(mt);
}

int main() {
    std::string data = "abcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcddd";
    std::vector<uint32_t> key = {random_uint32(), random_uint32(), random_uint32()};
    SNOW cipher(key);

    cipher.Encrypt(data);
    PrintAsHex(data);
    return 0;
}
