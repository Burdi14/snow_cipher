## This is the task about 1.0 version of SNOW encrypting algorithm.

### Solution
The key mistake in the algorithm is that the LFSR and FSM were not initialized separately. As a result, by the second cycle of the algorithm, we can recover both the LFSR and FSM by brute-forcing only key[0]. This means that initializing FSM_R2 in this implementation of the algorithm was actually unnecessary.

  ```cpp
  LFSR[0] = key_0_init;
  for (int i = 0; i < 15; ++i){
      LFSR[i + 1] = (LFSR[i] >> (1)) + ((((LFSR[i] >> 15)) ^ (LFSR[i] & 0x1)) << 16);
  }
  uint16_t FSM_R1_0 = 0xFFFF - LFSR[15];
  uint16_t FSM_R1_1 = LFSR[15];
  uint16_t FSM_R2_1 = (SBOX[(0xFF00 & FSM_R1_0) >> 8] << 8) + SBOX[0x00FF & FSM_R1_0];
  ```
        
Once the states of the LFSR and FSM in the second cycle are determined, we can recover the entire keystream and decrypt the flag, following the usual decryption process.
