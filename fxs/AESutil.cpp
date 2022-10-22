#include<ctime>
#include<iostream>
#include<fstream>
#include<sstream>
#include<vector>
#include<filesystem>
#include<bitset>
#include<ctime>
#include"Haffutil.h"
#include"AESutil.h"
#include"../jgqj/global.h"
using namespace std;
using namespace std::filesystem;


/*
 * AES-128 �����СΪ4
 */
const int Nb = 4;

/*
 * AES-128 ���鳤����4
 */
int Nk = 4;

/*
 * AES-128 ������10
 */
int Nr = 10;

/*
 * S-box �ı�
 */
static uint8_t s_box[256] = {
	// 0     1     2     3     4     5     6     7     8     9     a     b     c     d     e     f
	0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76, // 0
	0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0, // 1
	0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15, // 2
	0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75, // 3
	0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84, // 4
	0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf, // 5
	0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8, // 6
	0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2, // 7
	0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73, // 8
	0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb, // 9
	0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79, // a
	0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08, // b
	0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a, // c
	0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e, // d
	0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf, // e
	0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 };// f

/*
 * ����s_box��
 */
static uint8_t inv_s_box[256] = {
	// 0     1     2     3     4     5     6     7     8     9     a     b     c     d     e     f
	0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb, // 0
	0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb, // 1
	0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e, // 2
	0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25, // 3
	0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92, // 4
	0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84, // 5
	0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06, // 6
	0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b, // 7
	0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73, // 8
	0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e, // 9
	0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b, // a
	0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4, // b
	0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f, // c
	0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef, // d
	0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61, // e
	0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d };// f


/*
 * �ֳ���
 */
uint8_t R[] = { 0x02, 0x00, 0x00, 0x00 };

/*
  * GF��2^8���Ĺ���
  */
uint8_t gmult(uint8_t a, uint8_t b) {

	uint8_t p = 0, i = 0, hbs = 0;

	for (i = 0; i < 8; i++) {
		if (b & 1) {
			p ^= a;
		}

		hbs = a & 0x80;
		a <<= 1;
		if (hbs) a ^= 0x1b; // 0000 0001 0001 1011
		b >>= 1;
	}

	return (uint8_t)p;
}

/*
 * ���ֽڵļӷ�
 */
void coef_add(uint8_t a[], uint8_t b[], uint8_t d[]) {

	d[0] = a[0] ^ b[0];
	d[1] = a[1] ^ b[1];
	d[2] = a[2] ^ b[2];
	d[3] = a[3] ^ b[3];
}

/*
 * ���ֽڵĳ˷�
 */
void coef_mult(uint8_t *a, uint8_t *b, uint8_t *d) {

	d[0] = gmult(a[0], b[0]) ^ gmult(a[3], b[1]) ^ gmult(a[2], b[2]) ^ gmult(a[1], b[3]);
	d[1] = gmult(a[1], b[0]) ^ gmult(a[0], b[1]) ^ gmult(a[3], b[2]) ^ gmult(a[2], b[3]);
	d[2] = gmult(a[2], b[0]) ^ gmult(a[1], b[1]) ^ gmult(a[0], b[2]) ^ gmult(a[3], b[3]);
	d[3] = gmult(a[3], b[0]) ^ gmult(a[2], b[1]) ^ gmult(a[1], b[2]) ^ gmult(a[0], b[3]);
}

uint8_t * Rcon(uint8_t i) {

	if (i == 1) {
		R[0] = 0x01; // x^(1-1) = x^0 = 1
	}
	else if (i > 1) {
		R[0] = 0x02;
		i--;
		while (i - 1 > 0) {
			R[0] = gmult(R[0], 0x02);
			i--;
		}
	}

	return R;
}

/*
 * �㷨��add_round_key����
 */
void add_round_key(uint8_t *state, uint8_t *w, uint8_t r) {

	uint8_t c;

	for (c = 0; c < Nb; c++) {
		state[Nb * 0 + c] = state[Nb * 0 + c] ^ w[4 * Nb*r + 4 * c + 0];
		state[Nb * 1 + c] = state[Nb * 1 + c] ^ w[4 * Nb*r + 4 * c + 1];
		state[Nb * 2 + c] = state[Nb * 2 + c] ^ w[4 * Nb*r + 4 * c + 2];
		state[Nb * 3 + c] = state[Nb * 3 + c] ^ w[4 * Nb*r + 4 * c + 3];
	}
}

/*
 * �㷨��mix_columns����
 */

void mix_columns(uint8_t *state) {

	uint8_t a[] = { 0x02, 0x01, 0x01, 0x03 }; // a(x) = {02} + {01}x + {01}x2 + {03}x3
	uint8_t i, j, col[4], res[4];

	for (j = 0; j < Nb; j++) {
		for (i = 0; i < 4; i++) {
			col[i] = state[Nb*i + j];
		}

		coef_mult(a, col, res);

		for (i = 0; i < 4; i++) {
			state[Nb*i + j] = res[i];
		}
	}
}

/*
 * �㷨����mix_columns����
 */

void inv_mix_columns(uint8_t *state) {

	uint8_t a[] = { 0x0e, 0x09, 0x0d, 0x0b }; // a(x) = {0e} + {09}x + {0d}x2 + {0b}x3
	uint8_t i, j, col[4], res[4];

	for (j = 0; j < Nb; j++) {
		for (i = 0; i < 4; i++) {
			col[i] = state[Nb*i + j];
		}

		coef_mult(a, col, res);

		for (i = 0; i < 4; i++) {
			state[Nb*i + j] = res[i];
		}
	}
}

/*
 * �㷨��shift_rows����
 */
void shift_rows(uint8_t *state) {

	uint8_t i, k, s, tmp;

	for (i = 1; i < 4; i++) {
		s = 0;
		while (s < i) {
			tmp = state[Nb*i + 0];

			for (k = 1; k < Nb; k++) {
				state[Nb*i + k - 1] = state[Nb*i + k];
			}

			state[Nb*i + Nb - 1] = tmp;
			s++;
		}
	}
}

/*
 * �㷨����shift_rows����
 */
void inv_shift_rows(uint8_t *state) {

	uint8_t i, k, s, tmp;

	for (i = 1; i < 4; i++) {
		s = 0;
		while (s < i) {
			tmp = state[Nb*i + Nb - 1];

			for (k = Nb - 1; k > 0; k--) {
				state[Nb*i + k] = state[Nb*i + k - 1];
			}

			state[Nb*i + 0] = tmp;
			s++;
		}
	}
}

/*
 * �㷨��sub_bytes����
 */
void sub_bytes(uint8_t *state) {

	uint8_t i, j;
	uint8_t row, col;

	for (i = 0; i < 4; i++) {
		for (j = 0; j < Nb; j++) {
			row = (state[Nb*i + j] & 0xf0) >> 4;
			col = state[Nb*i + j] & 0x0f;
			state[Nb*i + j] = s_box[16 * row + col];
		}
	}
}

/*
 * �㷨����sub_bytes����
 */
void inv_sub_bytes(uint8_t *state) {

	uint8_t i, j;
	uint8_t row, col;

	for (i = 0; i < 4; i++) {
		for (j = 0; j < Nb; j++) {
			row = (state[Nb*i + j] & 0xf0) >> 4;
			col = state[Nb*i + j] & 0x0f;
			state[Nb*i + j] = inv_s_box[16 * row + col];
		}
	}
}

/*
 * ��Կ��չʱ�õ���S�б任
 */
void sub_word(uint8_t *w) {

	uint8_t i;

	for (i = 0; i < 4; i++) {
		w[i] = s_box[16 * ((w[i] & 0xf0) >> 4) + (w[i] & 0x0f)];
	}
}

/*
 * ����Կ��չʱ����λ�ñ任
 */
void rot_word(uint8_t *w) {

	uint8_t tmp;
	uint8_t i;

	tmp = w[0];

	for (i = 0; i < 3; i++) {
		w[i] = w[i + 1];
	}

	w[3] = tmp;
}

/*
 * ��Կ��չ����
 */
void key_expansion(uint8_t *key, uint8_t *w) {

	uint8_t tmp[4];
	uint8_t i, j;
	uint8_t len = Nb * (Nr + 1);

	for (i = 0; i < Nk; i++) {
		w[4 * i + 0] = key[4 * i + 0];
		w[4 * i + 1] = key[4 * i + 1];
		w[4 * i + 2] = key[4 * i + 2];
		w[4 * i + 3] = key[4 * i + 3];
	}

	for (i = Nk; i < len; i++) {
		tmp[0] = w[4 * (i - 1) + 0];
		tmp[1] = w[4 * (i - 1) + 1];
		tmp[2] = w[4 * (i - 1) + 2];
		tmp[3] = w[4 * (i - 1) + 3];

		if (i%Nk == 0) {

			rot_word(tmp);
			sub_word(tmp);
			coef_add(tmp, Rcon(i / Nk), tmp);

		}
		else if (Nk > 6 && i%Nk == 4) {

			sub_word(tmp);

		}

		w[4 * i + 0] = w[4 * (i - Nk) + 0] ^ tmp[0];
		w[4 * i + 1] = w[4 * (i - Nk) + 1] ^ tmp[1];
		w[4 * i + 2] = w[4 * (i - Nk) + 2] ^ tmp[2];
		w[4 * i + 3] = w[4 * (i - Nk) + 3] ^ tmp[3];
	}
}
/*
 * ���ܳ���
 */
void cipher(uint8_t *in, uint8_t *out, uint8_t *w) {

	uint8_t state[4 * Nb];
	uint8_t r, i, j;

	for (i = 0; i < 4; i++) {
		for (j = 0; j < Nb; j++) {
			state[Nb*i + j] = in[i + 4 * j];
		}
	}

	add_round_key(state, w, 0);

	for (r = 1; r < Nr; r++) {
		sub_bytes(state);
		shift_rows(state);
		mix_columns(state);
		add_round_key(state, w, r);
	}

	sub_bytes(state);
	shift_rows(state);
	add_round_key(state, w, Nr);

	for (i = 0; i < 4; i++) {
		for (j = 0; j < Nb; j++) {
			out[i + 4 * j] = state[Nb*i + j];
		}
	}
}

/*
 * ���ܳ���
 */
void inv_cipher(uint8_t *in, uint8_t *out, uint8_t *w) {

	uint8_t state[4 * Nb];
	uint8_t r, i, j;

	for (i = 0; i < 4; i++) {
		for (j = 0; j < Nb; j++) {
			state[Nb*i + j] = in[i + 4 * j];
		}
	}

	add_round_key(state, w, Nr);

	for (r = Nr - 1; r >= 1; r--) {
		inv_shift_rows(state);
		inv_sub_bytes(state);
		add_round_key(state, w, r);
		inv_mix_columns(state);
	}

	inv_shift_rows(state);
	inv_sub_bytes(state);
	add_round_key(state, w, 0);

	for (i = 0; i < 4; i++) {
		for (j = 0; j < Nb; j++) {
			out[i + 4 * j] = state[Nb*i + j];
		}
	}
}

void write_to_file(uint8_t *ptr, size_t len, string desFilename) {
	ofstream fp;
	fp.open(desFilename, ios::out | ios::binary | ios::app);
	fp.write((char*)ptr, len);
}

void long2charArr(long long num, char *tmp)
{
	long long temp = num;
	for (int i = 0; i < 8; i++) {
		if (-128 < (char)temp && (char)temp <= 127) {
			tmp[i] = (char)temp;
			temp >>= 8;
		}
		else {
			tmp[i] = '\n';
			break;
		}
	}
}

long long char2longArr(char *tmp)
{
	long long temp = 0;
	for (int i = 0; i < 8; i++) {
		if ((char)tmp[i] == '\n')
			break;
		else
		{
			temp += tmp[i] << (i * 8);
		}
	}
	return temp;
}

int getMode() {
	cout << "Encode: input '1'" << endl << "Decode: input '2' :" << endl;
	char mode;
	cin >> mode;
	while (1) {
		if (mode == 49) {
			return 1;
		}
		else if (mode == 50) {
			return 2;
		}
		else {
			cout << "your input is invalid,input it again:" << endl;
			cin >> mode;
		}
	}
}


vector<string> getEncryptionName() {
	vector<string> res;
	//�����ļ�(��)
	cout << "------------------------------------" << endl;
	cout << "please input the filename that you want to encryption:" << endl << "(include the suffix)" << endl;
	cin.sync();
	//������ļ�(��)Ŀ¼
	while (1) {
		string sourceFileName;
		getchar();
		getline(cin, sourceFileName);
		path str(sourceFileName);
		directory_entry entry(str);
		//�ļ���
		if (entry.status().type() == file_type::directory) {
			res.push_back("0");
			res.push_back(sourceFileName);
			break;
		}
		//δ�ҵ��ļ�
		else if (entry.status().type() == file_type::not_found) {
			cout << "not found file or dir. input again:" << endl;
		}
		//�ļ�
		else {
			res.push_back("1");
			res.push_back(sourceFileName);
			break;
		}
	}
	cout << "------------------------------------" << endl;
	//����ļ�(��)
	cout << "please input the output filename that you want:" << endl << "(include the suffix)" << endl;
	string desFileName;
	getline(cin, desFileName);
	res.push_back(desFileName);
	return res;
}

long long encryptionSingleFile(string sourceFileName, string desFileName, int mode, string Key) {
	string key_str = Key;
	uint8_t key[16];
	uint8_t in[16];
	//�����ԿΪ128bit��
	int pos = 0, key_len = key_str.length();
	for (int j = 0; j < 4; j++) {
		for (int k = 0; k < 4; k++) {
			if (pos < key_len)
				key[j * 4 + k] = key_str[pos++];
			else {
				key[j * 4 + k] = 'f';
				if (j == 3 && k == 3)
					key[j * 4 + k] = 16 - key_len;
			}
		}
	}
	ofstream fout;
    ifstream fin=ifstream(sourceFileName,ios::binary);

    if (mode == 1) {
        fout = ofstream(desFileName, ios::binary | ios::app);
	}
	else {
        fout = ofstream(desFileName, ios::binary|ios::trunc);
	}

    long long filesize = file_size(sourceFileName);
    long long size1 = file_size(desFileName);
	int bufferLength = 0;
	unsigned char bufferbit = 0;
	char readChars[512 * 1024];
	char bufferArray[512 * 1024];
	int bufferArrayIndex = 0;
	char buffer;
	//�������Ϊ���ɸ�128bit�����ݿ����
	if (filesize == 0) {
		return 0;
	}
	else {
		//ÿ�ζ�1M���ֽ�,����IO����
		for (int i = 0; i < (filesize / (512 * 1024)); i++) {
			fin.read(readChars, (512 * 1024) * sizeof(char));
			for (int t = 0; t < (512 * 1024) / 16; t++) {
				pos = t * 16;
				for (int j = 0; j < 4; j++) {
					for (int k = 0; k < 4; k++) {
						if (pos < 512 * 1024)
							in[j * 4 + k] = readChars[pos++];
						else {
							in[j * 4 + k] = rand() % 128;
							if (j == 3 && k == 3)
								in[j * 4 + k] = 0;
						}
					}
				}
				uint8_t out[16]; // 128
				uint8_t *w; // ��չ��Կ
				size_t L = sizeof(out);
				w = (uint8_t *)malloc(Nb*(Nr + 1) * 4);
				key_expansion(key, w);
				cipher(in, out, w);
				write_to_file(out, L, desFileName);
                free(w);
			}
		}
		//�����1M������
		fin.read(readChars, (filesize % (512 * 1024)) * sizeof(char));
		int round = (filesize % (512 * 1024)) % 16 ? (filesize % (512 * 1024)) / 16 + 1 : (filesize % (512 * 1024)) / 16;
		for (int t = 0; t < round; t++) {
			pos = t * 16;
			for (int j = 0; j < 4; j++) {
				for (int k = 0; k < 4; k++) {
					if (pos < 512 * 1024)
						in[j * 4 + k] = readChars[pos++];
					else {
						in[j * 4 + k] = rand() % 128;
						if (j == 3 && k == 3)
							in[j * 4 + k] = 0;
					}
				}
			}
			uint8_t out[16]; // 128
			uint8_t *w; // ��չ��Կ
			size_t L = sizeof(out);
			w = (uint8_t *)malloc(Nb*(Nr + 1) * 4);
			key_expansion(key, w);
			cipher(in, out, w);
			write_to_file(out, L, desFileName);
            free(w);
		}

	}
    char fileHead[16];
    //���ļ���Ϣ���ܽ�ȥ�ļ���
    long2charArr(filesize, fileHead);
    for (int t = 0; t < 1; t++) {
        pos = t * 16;
        for (int j = 0; j < 4; j++) {
            for (int k = 0; k < 4; k++) {
                if (pos < 512 * 1024)
                    in[j * 4 + k] = fileHead[pos++];
                else {
                    in[j * 4 + k] = rand() % 128;
                    if (j == 3 && k == 3)
                        in[j * 4 + k] = 0;
                }
            }
        }
        uint8_t out[16]; // 128
        uint8_t *w; // ��չ��Կ
        size_t L = sizeof(out);
        w = (uint8_t *)malloc(Nb*(Nr + 1) * 4);
        key_expansion(key, w);
        cipher(in, out, w);
        write_to_file(out, L, desFileName);
        free(w);
    }
    fout.close();
    fin.close();
	long long size = file_size(desFileName);
	return size - size1;
}

vector<string> getDecryptionName() {
	//�����ļ�
	cout << "------------------------------------" << endl;
	cout << "please input the filename that you want to decryption:" << endl << "(include the suffix)" << endl;
	string sourceFileName;
	cin.sync();
	getline(cin, sourceFileName);
	fstream testFile;
	vector<string> res;
	while (1) {
		testFile.open(sourceFileName);
		if (!testFile) {
			cout << "can not find this file,please input again:" << endl;
			getline(cin, sourceFileName);
		}
		else {
			//�ж����ļ��л����ļ�
			string firstLine;
			getline(testFile, firstLine);
			if (firstLine == "0") {
				//�ļ���
				res.push_back("0");
			}
			else
			{
				if (atoi(firstLine.c_str()) == 0) {
					//�ļ�
					res.push_back("1");
				}
				else {
					//�ļ���
					res.push_back("0");
				}
			}
			testFile.close();
			res.push_back(sourceFileName);
			break;
		}
	}
	cout << "------------------------------------" << endl;
	//����ļ�
	cout << "please input the output file/dir name that you want:" << endl << "(include the suffix)" << endl;
	string desFileName;
	getline(cin, desFileName);
	res.push_back(desFileName);

	return res;
}

void decryptionSingleFile(string sourceFileName, string desFileName, string Key) {
	string key_str = Key;
	uint8_t key[16];
	uint8_t in[16];
	//�����ԿΪ128bit��
	int pos = 0, key_len = key_str.length();
	for (int j = 0; j < 4; j++) {
		for (int k = 0; k < 4; k++) {
			if (pos < key_len)
				key[j * 4 + k] = key_str[pos++];
			else {
				key[j * 4 + k] = 'f';
				if (j == 3 && k == 3)
					key[j * 4 + k] = 16 - key_len;
			}
		}
	}
    ofstream fout(desFileName, ios::binary);
    ifstream fin(sourceFileName, ios::binary);
    long long filesize = file_size(sourceFileName) - 16;
	long long file_len = 0;
	int bufferLength = 0;
	//ÿ��ֻ����һ���ַ�,���ǻ���2^8���ַ�
	unsigned char bufferbit = 0;
	char readChars[512 * 1024];
	char bufferArray[512 * 1024];
	int bufferArrayIndex = 0;
	char buffer;
	if (filesize == 0) {
        return;
	}
	else {
		//ÿ�ζ�1M���ֽ�,����IO����
		for (int i = 0; i < (filesize / (512 * 1024)); i++) {
			fin.read(readChars, (512 * 1024) * sizeof(char));
			for (int t = 0; t < (512 * 1024) / 16; t++) {
				pos = t * 16;
				for (int j = 0; j < 4; j++) {
					for (int k = 0; k < 4; k++) {
						if (pos < 512 * 1024)
							in[j * 4 + k] = readChars[pos++];
						else {
							in[j * 4 + k] = rand() % 128;
							if (j == 3 && k == 3)
								in[j * 4 + k] = 0;
						}
					}
				}
				uint8_t out[16]; // 128
				uint8_t *w; // ��չ��Կ
				size_t L = sizeof(in);
				w = (uint8_t *)malloc(Nb*(Nr + 1) * 4);
				key_expansion(key, w);
				inv_cipher(in, out, w);
				write_to_file(out, L, desFileName);
                free(w);
			}
		}
		//�����1M������
		fin.read(readChars, (filesize % (512 * 1024)) * sizeof(char));
		int round = (filesize % (512 * 1024)) % 16 ? (filesize % (512 * 1024)) / 16 + 1 : (filesize % (512 * 1024)) / 16;
		for (int t = 0; t < round; t++) {
			pos = t * 16;
			for (int j = 0; j < 4; j++) {
				for (int k = 0; k < 4; k++) {
					if (pos < 512 * 1024)
						in[j * 4 + k] = readChars[pos++];
					else {
						in[j * 4 + k] = rand() % 128;
						if (j == 3 && k == 3)
							in[j * 4 + k] = 0;
					}
				}
			}
			uint8_t out[16]; // 128
			uint8_t *w; // ��չ��Կ
			size_t L = sizeof(in);
			w = (uint8_t *)malloc(Nb*(Nr + 1) * 4);
			key_expansion(key, w);
			inv_cipher(in, out, w);
            free(w);
			if (t < round - 1) {
				write_to_file(out, L, desFileName);
			}
            else{
                fin.read(readChars, 16 * (sizeof(char)));
                for (int t = 0; t < 1; t++) {
                    pos = t * 16;
                    for (int j = 0; j < 4; j++) {
                        for (int k = 0; k < 4; k++) {
                            if (pos < 512 * 1024)
                                in[j * 4 + k] = readChars[pos++];
                            else {
                                in[j * 4 + k] = rand() % 128;
                                if (j == 3 && k == 3)
                                    in[j * 4 + k] = 0;
                            }
                        }
                    }
                    uint8_t out[16]; // 128
                    uint8_t *w; // ��չ��Կ
                    size_t L = sizeof(out);
                    w = (uint8_t *)malloc(Nb*(Nr + 1) * 4);
                    key_expansion(key, w);
                    inv_cipher(in, out, w);
                    file_len = char2longArr((char*)out);
                    free(w);
                }
				write_to_file(out, file_len % 16, desFileName);
            }
		}
    }
    fin.close();
	fout.close();
}

void encryptionDir(string path, string desFileName, string encrypKey) {
	ofstream out(desFileName);
	vector<string> fileName;
	vector<string> dirName;
	filesystem::path p(path);
	//�ѵ�ǰ�ļ������ּ�¼��ȥ
	//dirName.push_back(p.filename().string());
	//Ҫ���ܵ��ļ��е�ǰ��ָ����Ŀ¼���ַ���
	string headPath=path;
	/*if (path.find('\\') != string::npos) {
		headPath = path.substr(0, path.find(p.filename().string()));
	}
	else {
		headPath = "";
	}*/
	//��¼���ļ�(��)
	for (auto const& entry : recursive_directory_iterator(path)) {
		if (entry.status().type() == file_type::directory) {
			dirName.push_back(entry.path().string().substr(headPath.length()));
		}
		else {
			fileName.push_back(entry.path().string().substr(headPath.length()));
		}
	}
	//д��Ŀ¼�ļ�
	out << dirName.size() << endl;
	for (int i = 0; i < dirName.size(); i++) {
		out << dirName[i] << endl;
	}
	//д���ļ��ļ�
	out << fileName.size() << endl;
	for (int i = 0; i < fileName.size(); i++) {
		//д�ļ���
		out << fileName[i] << endl;
	}
	out.close();
	// д�����������ļ�
	vector<long long> afterSizes;
	for (int i = 0; i < fileName.size(); i++) {
		int aftersize = encryptionSingleFile(headPath + fileName[i], desFileName, 1, encrypKey);
		afterSizes.push_back(aftersize);
	}
	// д���ܺ�Ĵ�С
	ofstream fout(desFileName, ios::app);
	fout << "\n";
	for (int i = 0; i < afterSizes.size(); i++) {
		fout << " " << afterSizes[i];
	}
    fout.close();
}
void decryptionDir(string sourceFileName, string desFileName, string decrypKey) {
	string key_str = decrypKey;
	//�ȴ����û�ָ�����ļ���
    if (desFileName != "")
        create_directories(desFileName);
	ifstream is(sourceFileName);
	string dirNum, filesNum, str_filesize;
	string path;
	//��ȡ�ļ���Ŀ¼
	getline(is, dirNum);
	int fileNum = atoi(dirNum.c_str());
	for (int i = 0; i < fileNum; i++) {
		getline(is, path);
		if (desFileName != "")
			create_directories(desFileName + "/" + path);
		else {
			create_directories(path);
		}
	}
	//��ȡ�ļ�
	getline(is, filesNum);
	int fileNums = atoi(filesNum.c_str());
	//�ļ�·��
	vector<string> filePaths;
	for (int i = 0; i < fileNums; i++) {
		getline(is, path);
		filePaths.push_back(path);
	}
	int startIndex = is.tellg();
	is.clear();
	is.close();
	vector<long long> aftersize = getAfterSize(sourceFileName, filePaths.size());
	//���ܸ������ļ�
	char buffer[512 * 1024];
	ifstream is2(sourceFileName, ios::binary);
	is2.seekg(startIndex);
	for (int i = 0; i < aftersize.size(); i++) {
		ofstream temp("temp.hfm", ios::binary);
		//��ÿ���ļ��Ķ���������д��temp��
		for (int j = 0; j < (aftersize[i] / (512 * 1024)); j++) {
			is2.read(buffer, 512 * 1024 * sizeof(char));
			temp.write(buffer, 512 * 1024 * sizeof(char));
		}
		is2.read(buffer, (aftersize[i] % (512 * 1024)) * sizeof(char));
		temp.write(buffer, (aftersize[i] % (512 * 1024)) * sizeof(char));
		temp.close();
		//���ܸ������ļ�
		if (desFileName != "")
			decryptionSingleFile("temp.hfm", desFileName + "/" + filePaths[i], key_str);
		else {
			decryptionSingleFile("temp.hfm", filePaths[i], key_str);
		}
		remove("temp.hfm");
	}
}
