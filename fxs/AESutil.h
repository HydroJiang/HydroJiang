#ifndef AESUTIL_H
#define AESUTIL_H

#include<ctime>
#include<iostream>
#include<fstream>
#include<sstream>
#include<vector>
#include<filesystem>
#include<bitset>
#include<ctime>

using namespace std;
using namespace std::filesystem;



/*
  * GF��2^8���Ĺ���
  */
extern uint8_t gmult(uint8_t a, uint8_t b);
/*
 * ���ֽڵļӷ�
 */
extern void coef_add(uint8_t a[], uint8_t b[], uint8_t d[]);

/*
 * ���ֽڵĳ˷�
 */
extern void coef_mult(uint8_t *a, uint8_t *b, uint8_t *d);


extern uint8_t * Rcon(uint8_t i);

/*
 * �㷨��add_round_key����
 */
extern void add_round_key(uint8_t *state, uint8_t *w, uint8_t r);

/*
 * �㷨��mix_columns����
 */

extern void mix_columns(uint8_t *state);

/*
 * �㷨����mix_columns����
 */

extern void inv_mix_columns(uint8_t *state);

/*
 * �㷨��shift_rows����
 */
extern void shift_rows(uint8_t *state);

/*
 * �㷨����shift_rows����
 */
extern void inv_shift_rows(uint8_t *state);

/*
 * �㷨��sub_bytes����
 */
extern void sub_bytes(uint8_t *state);

/*
 * �㷨����sub_bytes����
 */
extern void inv_sub_bytes(uint8_t *state);
/*
 * ��Կ��չʱ�õ���S�б任
 */
extern void sub_word(uint8_t *w);
/*
 * ����Կ��չʱ����λ�ñ任
 */
extern void rot_word(uint8_t *w);

/*
 * ��Կ��չ����
 */
extern void key_expansion(uint8_t *key, uint8_t *w);
/*
 * ���ܳ���
 */
extern void cipher(uint8_t *in, uint8_t *out, uint8_t *w);

/*
 * ���ܳ���
 */
extern void inv_cipher(uint8_t *in, uint8_t *out, uint8_t *w);

extern void write_to_file(uint8_t *ptr, size_t len, string desFilename);

extern void long2charArr(long long num, char *tmp);

extern long long char2longArr(char *tmp);

extern int getMode();


extern vector<string> getEncryptionName();

extern long long encryptionSingleFile(string sourceFileName, string desFileName, int mode, string Key);

extern vector<string> getDecryptionName();

extern void decryptionSingleFile(string sourceFileName, string desFileName, string Key);

extern void encryptionDir(string path, string desFileName, string encrypKey);
extern void decryptionDir(string sourceFileName, string desFileName, string decrypKey);

#endif 