
//compile with shake :  gcc -Wall -Wextra -O3 -fomit-frame-pointer -march=native -o sample sampler-96.c

#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include"randombytes.c"
#include"fips202.c"
#include "cpucycles.c"

unsigned long long int hist[84];

//-------shake------
void initRandom(uint8_t *x, uint32_t len){

	randombytes(x,len);
}
//-------shake ends-

int main(){

	long int i,j,k;
	unsigned long long int bit[97];// to hold the bits
        unsigned long long int sample[64];
	volatile unsigned long long int out[7];
	//int segment[10];//to keep track if the code enters into some segments

	
	volatile unsigned long long int clock1, clock2,clock3;
	clock1=0;clock2=0;clock3=0;
	
/*
	//-----sha--------------
	time_t t;
	srand((unsigned) time(&t));

	long long int data_length=16;
	unsigned char current_seed1[data_length];
	unsigned char* hash = (unsigned char*) ((void*) bit);
	fp = fopen("/dev/urandom", "r");
	size_t r = fread(current_seed1,sizeof(unsigned char),data_length,fp);
	SHA512_CTX sha512;
	SHA512_Init(&sha512);
	//-----sha ends--------------
*/

	//-------------------shake--------------------

	uint8_t seed[32];
	uint32_t SEEDBYTES=32;
	uint64_t s[25];
	uint8_t temp_byte[SHAKE128_RATE];
	uint32_t nrounds=4;
 	unsigned char* byte_bank = (unsigned char*) ((void*) bit);

	for (i = 0; i < 25; ++i)
	    s[i] = 0;
	initRandom(seed,SEEDBYTES);
	keccak_absorb(s, SHAKE128_RATE, seed, SEEDBYTES, 0x1F);
	

	//-------------------shake ends---------------

	unsigned long long int disable_update,control;
	
	for(i=0;i<84;i++)
		hist[i]=0;

	/*for(i=0;i<10;i++)
		segment[i]=0;*/

	clock3=0;
	int repeat=1000000;
	for(i=0;i<repeat;i++){	

		for(k=0;k<7;k++)
		out[k]=out[k]^out[k];

		clock1=0;
		clock2=0;
//------------------sha-------------------------
		/*for (m=0; m<data_length; m++)
		{
		    	if (current_seed1[m]!=255)
		    	{
		    		for (l=0; l<m; l++) current_seed1[l] = 0;
		    		current_seed1[m]++;
		    		break;
		    	}
	    	}*/
//------------------sha ends------------------------

		//clock1 = cpucycles();

//-------------------sha--------------------------

//-------------------shake----------------------
	keccak_squeezeblocks(byte_bank, nrounds, s, SHAKE128_RATE);
		
	keccak_squeezeblocks(temp_byte, 1, s, SHAKE128_RATE);

	for(j=0;j<(97*8-SHAKE128_RATE*nrounds);j++){
		byte_bank[nrounds*SHAKE128_RATE+j]=temp_byte[j];			
	}

//-------------------shake ends-----------------



/*
//--------------------0----------------------
		SHA512_Update(&sha512, current_seed1, 1); //generate the random numbers
		SHA512_Final(hash, &sha512);
//--------------------1----------------------
		SHA512_Update(&sha512, current_seed1+1, 1);
		SHA512_Final(hash+64*1, &sha512);
//--------------------2----------------------
		SHA512_Update(&sha512, current_seed1+2, 1);
		SHA512_Final(hash+64*2, &sha512);
//--------------------3----------------------
		SHA512_Update(&sha512, current_seed1+3, 1);
		SHA512_Final(hash+64*3, &sha512);
//--------------------4----------------------
		SHA512_Update(&sha512, current_seed1+4, 1);
		SHA512_Final(hash+64*4, &sha512);
//--------------------5----------------------
		SHA512_Update(&sha512, current_seed1+5, 1);
		SHA512_Final(hash+64*5, &sha512);
//--------------------6----------------------
		SHA512_Update(&sha512, current_seed1+6, 1);
		SHA512_Final(hash+64*6, &sha512);
//--------------------7----------------------
		SHA512_Update(&sha512, current_seed1+7, 1);
		SHA512_Final(hash+64*7, &sha512);
//--------------------8----------------------
		SHA512_Update(&sha512, current_seed1+8, 1);
		SHA512_Final(hash+64*8, &sha512);
//--------------------9----------------------
		SHA512_Update(&sha512, current_seed1+9, 1);
		SHA512_Final(hash+64*9, &sha512);
//--------------------10----------------------
		SHA512_Update(&sha512, current_seed1+10, 1); 
		SHA512_Final(hash+64*10, &sha512);
//--------------------11----------------------
		SHA512_Update(&sha512, current_seed1+11, 1);
		SHA512_Final(hash+64*11, &sha512);
*/
//-------------------sha ends--------------------------
		clock1 = cpucycles();

//-----------------------------------------------------------------------------------------------------------

		disable_update=0; //0-> to update every time. 1-> don't update anymore. Once switched to 1 stays at 1
		control=0xffffffffffffffff; //used to control the disable_update properly

		out[2] = (~disable_update & ((~bit[2]&bit[1]) | (bit[2]&~bit[1]))) | (disable_update & out[2]);

		out[1] = (~disable_update & (bit[2])) | (disable_update & out[1]);

		out[0] = (~disable_update & ((~bit[2]&~bit[1]) | (~bit[3]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[0]);
		control=control & bit[0];
		//-------------1--------------

		out[3] = (~disable_update & ((~bit[4]&~bit[3]&bit[2]) | (bit[4]&bit[3]&~bit[2]) | (~bit[4]&bit[3]&~bit[2]))) | (disable_update & out[3]);

		out[2] = (~disable_update & (bit[4]&~bit[3]&bit[2])) | (disable_update & out[2]);

		out[1] = (~disable_update & ((~bit[4]&bit[3]&~bit[2]) | (bit[3]&bit[2]))) | (disable_update & out[1]);

		out[0] = (~disable_update & ((~bit[4]&bit[3]&bit[2]) | (bit[4]&bit[3]&~bit[2]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[1]);
		control=control & bit[1];

		//-------------2--------------

		out[3] = (~disable_update & ((~bit[4]&~bit[3]) | (bit[6]&bit[5]&bit[4]&bit[3]) | (~bit[6]&bit[5]&bit[4]&bit[3]) | (~bit[5]&~bit[3]))) | (disable_update & out[3]);

		out[2] = (~disable_update & ((bit[6]&bit[5]&bit[4]&bit[3]) | (~bit[6]&bit[5]&bit[4]&bit[3]) | (bit[5]&bit[4]&~bit[3]) | (~bit[5]&~bit[4]))) | (disable_update & out[2]);

		out[1] = (~disable_update & ((~bit[5]&bit[4]&bit[3]) | (~bit[6]&bit[5]&bit[4]&bit[3]) | (bit[5]&bit[4]&~bit[3]) | (bit[5]&~bit[4]))) | (disable_update & out[1]);

		out[0] = (~disable_update & ((~bit[4]&bit[3]) | (bit[6]&bit[5]&bit[4]&bit[3]) | (bit[5]&~bit[4]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[2]);
		control=control & bit[2];

		//-------------3--------------

		out[4] = (~disable_update & ((~bit[7]&~bit[6]&bit[5]&bit[4]))) | (disable_update & out[4]);

		out[3] = (~disable_update & ((bit[7]&~bit[6]&bit[5]&bit[4]) | (~bit[6]&~bit[4]) | (bit[6]&bit[5]&bit[4]) | (~bit[5]&~bit[4]))) | (disable_update & out[3]);

		out[2] = (~disable_update & ((~bit[7]&bit[6]&bit[5]) | (~bit[6]&~bit[5]&bit[4]) | (bit[6]&bit[5]&~bit[4]) | (bit[7]&~bit[6]&bit[5]&bit[4]))) | (disable_update & out[2]);

		out[1] = (~disable_update & ((~bit[6]&~bit[5]&~bit[4]) | (bit[6]&~bit[5]&bit[4]) | (bit[7]&~bit[6]&bit[5]&bit[4]))) | (disable_update & out[1]);

		out[0] = (~disable_update & ((bit[6]&bit[5]&~bit[4]) | (bit[7]&~bit[6]&bit[5]&bit[4]) | (bit[6]&bit[5]&bit[4]) | (~bit[5]&~bit[4]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[3]);
		control=control & bit[3];

		//-------------4--------------

		out[4] = (~disable_update & ((~bit[8]&~bit[7]&~bit[6]&bit[5]))) | (disable_update & out[4]);

		out[3] = (~disable_update & ((bit[7]&~bit[6]&bit[5]) | (~bit[7]&bit[6]&bit[5]) | (bit[8]&~bit[7]&~bit[6]&bit[5]))) | (disable_update & out[3]);

		out[2] = (~disable_update & ((bit[8]&bit[7]&bit[6]&bit[5]) | (~bit[8]&bit[7]&bit[6]&bit[5]) | (bit[7]&~bit[6]&bit[5]) | (bit[8]&~bit[7]&~bit[6]&bit[5]) | (~bit[6]&~bit[5]))) | (disable_update & out[2]);

		out[1] = (~disable_update & ((~bit[8]&bit[7]&bit[6]&bit[5]) | (bit[6]&~bit[5]) | (~bit[7]&bit[6]&bit[5]) | (bit[8]&~bit[7]&~bit[6]&bit[5]) | (~bit[7]&~bit[5]))) | (disable_update & out[1]);

		out[0] = (~disable_update & ((bit[8]&bit[7]&bit[6]&bit[5]) | (~bit[8]&~bit[6]) | (~bit[8]&~bit[7]) | (bit[8]&~bit[7]&~bit[6]&bit[5]) | (~bit[6]&~bit[5]) | (~bit[7]&~bit[5]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[4]);
		control=control & bit[4];

		//-------------5--------------

		out[4] = (~disable_update & ((bit[8]&bit[7]&~bit[6]))) | (disable_update & out[4]);

		out[3] = (~disable_update & ((~bit[9]&~bit[8]&bit[6]) | (~bit[7]&bit[6]))) | (disable_update & out[3]);

		out[2] = (~disable_update & ((bit[9]&~bit[8]&bit[7]&bit[6]) | (bit[8]&bit[7]&bit[6]) | (~bit[8]&~bit[7]))) | (disable_update & out[2]);

		out[1] = (~disable_update & ((bit[8]&bit[7]&~bit[6]) | (bit[9]&~bit[8]&bit[7]&bit[6]) | (~bit[9]&~bit[7]&bit[6]))) | (disable_update & out[1]);

		out[0] = (~disable_update & ((bit[9]&bit[8]&~bit[7]) | (bit[9]&~bit[8]&bit[7]&bit[6]) | (bit[8]&~bit[7]&~bit[6]) | (~bit[9]&bit[8]&bit[7]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[5]);
		control=control & bit[5];

		//-------------6--------------

		out[4] = (~disable_update & ((bit[8]&~bit[7]))) | (disable_update & out[4]);

		out[3] = (~disable_update & ((~bit[10]&~bit[9]&bit[7]) | (~bit[8]&bit[7]))) | (disable_update & out[3]);

		out[2] = (~disable_update & ((~bit[10]&~bit[9]&bit[8]&~bit[7]) | (bit[10]&~bit[9]&bit[8]&bit[7]) | (~bit[10]&bit[9]&bit[7]) | (~bit[9]&~bit[8]&bit[7]))) | (disable_update & out[2]);

		out[1] = (~disable_update & ((bit[9]&bit[8]&bit[7]) | (bit[10]&bit[9]&bit[7]) | (~bit[9]&~bit[8]&bit[7]) | (~bit[8]&~bit[7]) | (bit[10]&~bit[9]))) | (disable_update & out[1]);

		out[0] = (~disable_update & ((~bit[10]&bit[9]&bit[8]&~bit[7]) | (bit[10]&~bit[9]&bit[8]&bit[7]) | (~bit[9]&~bit[8]&~bit[7]) | (~bit[10]&~bit[9]&bit[7]) | (bit[10]&bit[9]&bit[7]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[6]);
		control=control & bit[6];

		//-------------7--------------

		out[4] = (~disable_update & ((bit[11]&~bit[10]&bit[9]&~bit[8]) | (~bit[11]&bit[9]&~bit[8]))) | (disable_update & out[4]);

		out[3] = (~disable_update & ((bit[11]&bit[10]&bit[9]&~bit[8]) | (~bit[9]&bit[8]) | (~bit[11]&~bit[10]&bit[8]))) | (disable_update & out[3]);

		out[2] = (~disable_update & ((~bit[11]&~bit[10]&bit[9]&~bit[8]) | (~bit[11]&~bit[9]&bit[8]) | (bit[11]&bit[10]&bit[9]&~bit[8]) | (~bit[11]&bit[10]&bit[8]) | (bit[11]&~bit[10]&bit[8]))) | (disable_update & out[2]);

		out[1] = (~disable_update & ((bit[11]&~bit[10]&bit[9]&~bit[8]) | (~bit[10]&~bit[9]&~bit[8]) | (bit[11]&bit[10]&bit[9]&~bit[8]) | (bit[11]&bit[10]&bit[8]) | (~bit[11]&~bit[10]&bit[8]))) | (disable_update & out[1]);

		out[0] = (~disable_update & ((~bit[11]&~bit[10]&bit[9]&~bit[8]) | (bit[11]&bit[10]&bit[8]) | (bit[11]&~bit[10]&bit[8]) | (bit[10]&~bit[8]))) | (disable_update & out[0]);


		disable_update= disable_update | (control & ~bit[7]);
		control=control & bit[7];


		//-------------8--------------

		out[4] = (~disable_update & ((~bit[12]&~bit[11]&~bit[10]&bit[9]) | (bit[12]&bit[11]&bit[10]&~bit[9]) | (~bit[12]&bit[11]&bit[10]&~bit[9]) | (~bit[12]&bit[11]&~bit[10]&bit[9]) | (bit[12]&~bit[11]&~bit[10]&bit[9]))) | (disable_update & out[4]);

		out[3] = (~disable_update & ((bit[12]&bit[11]&~bit[10]&bit[9]) | (~bit[11]&bit[10]&bit[9])))  | (disable_update & out[3]);

		out[2] = (~disable_update & ((~bit[12]&~bit[11]&~bit[10]&bit[9]) | (bit[12]&bit[11]&~bit[10]&bit[9]) | (bit[11]&bit[10])))  | (disable_update & out[2]);

		out[1] = (~disable_update & ((~bit[12]&bit[11]&bit[10]&~bit[9]) | (~bit[11]&~bit[10]&~bit[9]) | (bit[12]&bit[11]&~bit[10]&bit[9]) | (~bit[11]&bit[10]&bit[9]) | (bit[12]&~bit[11]&~bit[10]&bit[9])))  | (disable_update & out[1]);

		out[0] = (~disable_update & ((bit[12]&bit[11]&bit[10]&~bit[9]) | (~bit[12]&bit[10]&bit[9]) | (bit[11]&~bit[10]&~bit[9]) | (~bit[12]&bit[11]&~bit[10]&bit[9]) | (bit[12]&~bit[11]&~bit[10]&bit[9])))  | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[8]);
		control=control & bit[8];

		//-------------9--------------

		out[4] = (~disable_update & ((~bit[13]&~bit[12]&bit[11]&~bit[10]) | (bit[13]&~bit[12]&bit[11]&~bit[10]) | (bit[12]&bit[11]&~bit[10]))) | (disable_update & out[4]);

		out[3] = (~disable_update & ((~bit[13]&~bit[12]&bit[11]&~bit[10]) | (bit[13]&~bit[11]&bit[10]) | (~bit[13]&~bit[11]&bit[10]) | (~bit[13]&~bit[12]&bit[10]))) | (disable_update & out[3]);

		out[2] = (~disable_update & ((~bit[13]&bit[12]&bit[11]&~bit[10]) | (bit[13]&~bit[12]&bit[10]) | (~bit[13]&~bit[11]&bit[10]) | (bit[13]&~bit[12]&bit[11]&~bit[10]))) | (disable_update & out[2]);

		out[1] = (~disable_update & ((~bit[13]&bit[12]&bit[11]) | (bit[13]&~bit[11]&bit[10]) | (bit[13]&~bit[12]&bit[11]&~bit[10]) | (bit[12]&bit[11]&~bit[10]) | (~bit[12]&~bit[11]))) | (disable_update & out[1]);

		out[0] = (~disable_update & ((bit[12]&~bit[11]&~bit[10]) | (bit[12]&bit[11]&bit[10]) | (~bit[13]&~bit[12]&bit[10]) | (bit[13]&~bit[12]&bit[11]&~bit[10]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[9]);
		control=control & bit[9];

		//-------------10--------------

		out[4] = (~disable_update & ((~bit[14]&bit[13]&~bit[12]&~bit[11]) | (bit[14]&bit[13]&~bit[11]) | (~bit[13]&bit[12]&~bit[11]) | (~bit[14]&bit[12]&~bit[11]))) | (disable_update & out[4]);

		out[3] = (~disable_update & ((~bit[14]&bit[13]&~bit[12]&~bit[11]) | (~bit[12]&bit[11]))) | (disable_update & out[3]);

		out[2] = (~disable_update & ((bit[14]&bit[13]&~bit[12]&~bit[11]) | (~bit[14]&bit[12]&bit[11]) | (~bit[13]&bit[12]&~bit[11]) | (~bit[13]&bit[11]))) | (disable_update & out[2]);

		out[1] = (~disable_update & ((bit[14]&bit[13]&bit[12]&bit[11]) | (bit[14]&bit[13]&~bit[12]&~bit[11]) | (~bit[14]&~bit[13]&bit[11]) | (~bit[14]&~bit[12]&bit[11]) | (~bit[14]&bit[12]&~bit[11]))) | (disable_update & out[1]);

		out[0] = (~disable_update & ((~bit[14]&bit[13]&~bit[12]&~bit[11]) | (bit[14]&bit[12]&bit[11]) | (bit[14]&bit[13]&~bit[11]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[10]);
		control=control & bit[10];

		//-------------11--------------

		out[4] = (~disable_update & ((bit[14]&bit[13]&~bit[12]) | (~bit[13]&bit[12]))) | (disable_update & out[4]);

		out[3] = (~disable_update & ((~bit[15]&bit[14]&bit[13]&~bit[12]) | (bit[15]&~bit[14]&bit[13]&bit[12]) | (~bit[15]&~bit[14]&bit[13]&bit[12]) | (~bit[15]&bit[14]&bit[13]&bit[12]))) | (disable_update & out[3]);

		out[2] = (~disable_update & ((~bit[14]&bit[12]) | (bit[15]&bit[14]&bit[13]))) | (disable_update & out[2]);

		out[1] = (~disable_update & ((~bit[14]&~bit[13]&~bit[12]) | (~bit[15]&bit[14]&bit[13]&~bit[12]) | (~bit[15]&~bit[14]&bit[13]&bit[12]) | (bit[15]&bit[14]&bit[13]))) | (disable_update & out[1]);

		out[0] = (~disable_update & ((bit[15]&~bit[14]&bit[13]&bit[12]) | (bit[14]&~bit[13]&~bit[12]) | (~bit[15]&~bit[13]&bit[12]) | (~bit[15]&bit[14]&bit[13]&bit[12]) | (bit[15]&bit[14]&bit[13]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[11]);
		control=control & bit[11];

		//-------------12--------------

		out[4] = (~disable_update & ((~bit[16]&~bit[14]&bit[13]) | (bit[16]&~bit[15]&~bit[14]&bit[13]))) | (disable_update & out[4]);

		out[3] = (~disable_update & ((~bit[15]&bit[14]&bit[13]) | (bit[16]&bit[15]&~bit[14]&bit[13]) | (~bit[16]&~bit[15]&bit[13]))) | (disable_update & out[3]);

		out[2] = (~disable_update & ((~bit[15]&~bit[14]&~bit[13]) | (bit[16]&bit[15]&~bit[14]&bit[13]) | (bit[15]&bit[14]&bit[13]) | (bit[16]&~bit[15]&~bit[14]&bit[13]))) | (disable_update & out[2]);

		out[1] = (~disable_update & ((bit[16]&bit[15]&~bit[14]&bit[13]) | (bit[15]&~bit[14]&~bit[13]) | (bit[15]&bit[14]&bit[13]) | (bit[16]&~bit[15]&~bit[14]&bit[13]) | (~bit[16]&~bit[15]&bit[13]))) | (disable_update & out[1]);

		out[0] = (~disable_update & ((~bit[16]&bit[14]&bit[13]) | (bit[15]&~bit[14]&~bit[13]) | (~bit[15]&~bit[13]) | (bit[16]&~bit[15]&~bit[14]&bit[13]) | (~bit[16]&~bit[15]&bit[13]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[12]);
		control=control & bit[12];

		//-------------13--------------

		out[4] = (~disable_update & ((~bit[17]&bit[16]&bit[15]&~bit[14]) | (bit[17]&bit[16]&bit[15]&~bit[14]) | (~bit[15]&bit[14]))) | (disable_update & out[4]);

		out[3] = (~disable_update & ((~bit[17]&~bit[16]&bit[15]&bit[14]) | (~bit[17]&bit[16]&bit[15]&~bit[14]))) | (disable_update & out[3]);

		out[2] = (~disable_update & ((~bit[17]&bit[16]&bit[15]&~bit[14]) | (bit[17]&~bit[16]&bit[15]&bit[14]) | (bit[17]&bit[16]&bit[15]&~bit[14]) | (~bit[17]&bit[15]&bit[14]) | (~bit[16]&~bit[15]))) | (disable_update & out[2]);

		out[1] = (~disable_update & ((bit[16]&~bit[15]&~bit[14]) | (~bit[17]&~bit[15]&bit[14]) | (bit[17]&~bit[16]&bit[15]&bit[14]) | (bit[17]&bit[16]&bit[15]&~bit[14]))) | (disable_update & out[1]);

		out[0] = (~disable_update & ((bit[17]&~bit[16]&bit[15]&bit[14]) | (bit[17]&bit[16]&bit[15]&~bit[14]) | (~bit[16]&~bit[14]) | (~bit[17]&bit[15]&bit[14]) | (bit[16]&~bit[15]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[13]);
		control=control & bit[13];

		//-------------14--------------

		out[4] = (~disable_update & ((~bit[18]&~bit[17]&~bit[16]) | (bit[16]&~bit[15]) | (~bit[16]&~bit[15]))) | (disable_update & out[4]);

		out[3] = (~disable_update & ((~bit[18]&bit[17]&~bit[16]&bit[15]) | (~bit[17]&bit[16]&bit[15]) | (bit[18]&~bit[16]&bit[15]) | (~bit[16]&~bit[15]))) | (disable_update & out[3]);

		out[2] = (~disable_update & ((bit[18]&~bit[17]&~bit[16]&bit[15]) | (~bit[18]&bit[17]&~bit[16]&bit[15]) | (~bit[17]&bit[16]&~bit[15]) | (~bit[18]&~bit[17]&~bit[15]))) | (disable_update & out[2]);

		out[1] = (~disable_update & ((bit[17]&bit[16]&bit[15]) | (bit[18]&~bit[17]&~bit[16]) | (bit[18]&~bit[16]&bit[15]) | (~bit[18]&bit[16]))) | (disable_update & out[1]);

		out[0] = (~disable_update & ((~bit[18]&bit[17]&bit[16]) | (~bit[18]&~bit[15]) | (bit[18]&~bit[16]&bit[15]) | (bit[16]&~bit[15]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[14]);
		control=control & bit[14];

		//-------------15--------------

		out[4] = (~disable_update & ((bit[20]&bit[19]&bit[18]&bit[17]&bit[16]) | (bit[19]&~bit[18]&~bit[17]&bit[16]) | (~bit[19]&~bit[18]&~bit[17]&bit[16]) | (~bit[19]&~bit[18]&bit[17]&~bit[16]) | (~bit[20]&bit[18]&bit[17]&bit[16]) | (bit[19]&~bit[18]&bit[17]&~bit[16]) | (bit[19]&bit[18]&bit[17]&~bit[16]) | (bit[18]&~bit[17]&~bit[16]) | (~bit[19]&bit[18]))) | (disable_update & out[4]);

		out[3] = (~disable_update & ((bit[20]&~bit[19]&bit[18]&bit[17]&bit[16]) | (bit[19]&bit[18]&~bit[17]&bit[16]) | (~bit[19]&~bit[18]&bit[17]&~bit[16]) | (~bit[20]&bit[18]&bit[17]&bit[16]) | (bit[18]&~bit[17]&~bit[16]))) | (disable_update & out[3]);

		out[2] = (~disable_update & ((~bit[20]&~bit[19]&bit[18]&bit[17]&bit[16]) | (~bit[19]&bit[18]&bit[17]&~bit[16]) | (bit[19]&bit[18]&~bit[17]&bit[16]) | (bit[20]&bit[19]&bit[18]&bit[17]&bit[16]) | (bit[19]&~bit[18]&bit[17]&~bit[16]))) | (disable_update & out[2]);

		out[1] = (~disable_update & ((~bit[20]&~bit[19]&bit[18]&bit[17]&bit[16]) | (~bit[19]&bit[18]&~bit[17]&~bit[16]) | (bit[20]&bit[19]&bit[18]&bit[17]&bit[16]) | (~bit[19]&~bit[18]&~bit[17]&bit[16]) | (bit[19]&bit[18]&bit[17]&~bit[16]))) | (disable_update & out[1]);

		out[0] = (~disable_update & ((bit[20]&~bit[19]&bit[18]&bit[17]&bit[16]) | (~bit[19]&~bit[18]&bit[17]&bit[16]) | (bit[19]&~bit[18]&~bit[17]&bit[16]) | (bit[19]&~bit[18]&bit[17]&~bit[16]) | (bit[19]&bit[18]&bit[17]&~bit[16]) | (bit[18]&~bit[17]&~bit[16]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[15]);
		control=control & bit[15];

		//-------------16--------------

		out[4] = (~disable_update & ((~bit[18]&~bit[17]))) | (disable_update & out[4]);

		out[3] = (~disable_update & ((~bit[20]&~bit[18]&bit[17]) | (~bit[19]&~bit[18]&bit[17]) | (bit[18]&~bit[17]))) | (disable_update & out[3]);

		out[2] = (~disable_update & ((bit[20]&bit[19]&~bit[18]&bit[17]) | (~bit[20]&~bit[19]&bit[18]) | (~bit[20]&~bit[19]&~bit[17]) | (bit[18]&~bit[17]))) | (disable_update & out[2]);

		out[1] = (~disable_update & ((bit[20]&bit[19]&~bit[18]&bit[17]) | (~bit[20]&bit[19]&~bit[18]&~bit[17]) | (~bit[20]&~bit[19]&~bit[18]&bit[17]) | (bit[20]&~bit[19]&~bit[18]&~bit[17]) | (bit[20]&~bit[19]&bit[18]) | (~bit[19]&bit[18]&~bit[17]))) | (disable_update & out[1]);

		out[0] = (~disable_update & ((bit[20]&~bit[19]&~bit[18]&~bit[17]) | (~bit[19]&~bit[18]&bit[17]) | (~bit[20]&~bit[19]&~bit[17]) | (~bit[20]&bit[18]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[16]);
		control=control & bit[16];

		//-------------17--------------

		out[5] = (~disable_update & ((~bit[22]&bit[21]&bit[20]&bit[19]&bit[18]))) | (disable_update & out[5]);

		out[4] = (~disable_update & ((bit[22]&bit[21]&bit[20]&bit[19]&bit[18]) | (~bit[18]))) | (disable_update & out[4]);

		out[3] = (~disable_update & ((bit[22]&bit[21]&bit[20]&bit[19]&bit[18]) | (~bit[19]&~bit[18]) | (~bit[21]&~bit[19]&bit[18]) | (~bit[20]&~bit[19]))) | (disable_update & out[3]);

		out[2] = (~disable_update & ((bit[22]&bit[21]&bit[20]&bit[19]&bit[18]) | (~bit[21]&~bit[19]&~bit[18]) | (bit[21]&bit[20]&~bit[19]&bit[18]) | (~bit[21]&~bit[20]) | (~bit[20]&~bit[19]) | (~bit[20]&~bit[18]))) | (disable_update & out[2]);

		out[1] = (~disable_update & ((bit[22]&bit[21]&bit[20]&bit[19]&bit[18]) | (bit[21]&~bit[20]&bit[19]&bit[18]) | (~bit[21]&bit[20]&bit[19]) | (bit[21]&bit[20]&~bit[19]&bit[18]) | (~bit[21]&~bit[19]&bit[18]) | (~bit[20]&~bit[18]))) | (disable_update & out[1]);

		out[0] = (~disable_update & ((bit[22]&bit[21]&bit[20]&bit[19]&bit[18]) | (bit[21]&~bit[20]&bit[19]&bit[18]) | (~bit[21]&~bit[20]&~bit[18]) | (bit[21]&bit[20]&~bit[19]&bit[18]) | (~bit[21]&~bit[19]&bit[18]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[17]);
		control=control & bit[17];

		//-------------18--------------

		out[4] = (~disable_update & ((bit[22]&bit[21]&~bit[20]&~bit[19]) | (~bit[21]&bit[20]&~bit[19]) | (~bit[22]&~bit[20]&~bit[19]) | (~bit[21]&~bit[20]&~bit[19]))) | (disable_update & out[4]);

		out[3] = (~disable_update & ((bit[22]&~bit[21]&~bit[20]&bit[19]) | (~bit[22]&~bit[20]&~bit[19]) | (~bit[22]&~bit[21]&~bit[20]) | (bit[21]&bit[20]&~bit[19]) | (~bit[21]&~bit[20]&~bit[19]))) | (disable_update & out[3]);

		out[2] = (~disable_update & ((~bit[22]&~bit[21]&bit[20]&bit[19]) | (bit[22]&bit[21]&~bit[20]&~bit[19]) | (~bit[22]&~bit[21]&~bit[20]&~bit[19]) | (bit[21]&~bit[20]&bit[19]) | (bit[21]&bit[20]&~bit[19]))) | (disable_update & out[2]);

		out[1] = (~disable_update & ((bit[22]&~bit[21]&bit[20]&bit[19]) | (~bit[22]&bit[21]&bit[19]) | (~bit[22]&bit[20]&~bit[19]) | (~bit[22]&~bit[21]&~bit[20]) | (bit[21]&bit[20]&~bit[19]) | (~bit[21]&~bit[20]&~bit[19]))) | (disable_update & out[1]);

		out[0] = (~disable_update & ((bit[22]&~bit[21]&bit[20]&bit[19]) | (bit[22]&~bit[21]&~bit[20]&bit[19]) | (bit[21]&~bit[20]&bit[19]) | (~bit[21]&bit[20]&~bit[19]) | (~bit[22]&bit[20]&~bit[19]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[18]);
		control=control & bit[18];

		//-------------19--------------

		out[5] = (~disable_update & ((~bit[23]&bit[22]&bit[21]&bit[20]) | (~bit[23]&~bit[22]&~bit[21]&~bit[20]))) | (disable_update & out[5]);

		out[4] = (~disable_update & ((bit[23]&bit[22]&bit[21]&bit[20]) | (bit[23]&bit[22]&~bit[21]&~bit[20]) | (~bit[23]&~bit[22]&bit[21]&~bit[20]) | (~bit[23]&bit[22]&~bit[21]&~bit[20]) | (~bit[23]&bit[22]&bit[21]&~bit[20]) | (bit[23]&~bit[22]&~bit[20]))) | (disable_update & out[4]);

		out[3] = (~disable_update & ((~bit[23]&~bit[22]&bit[21]&~bit[20]) | (~bit[23]&bit[22]&~bit[21]&~bit[20]) | (~bit[23]&~bit[21]&bit[20]) | (bit[23]&~bit[21]) | (bit[23]&bit[22]&bit[21]))) | (disable_update & out[3]);

		out[2] = (~disable_update & ((~bit[23]&~bit[22]&bit[21]&bit[20]) | (~bit[23]&bit[22]&~bit[21]&~bit[20]) | (bit[23]&~bit[22]&~bit[20]) | (bit[23]&bit[22]&bit[21]))) | (disable_update & out[2]);

		out[1] = (~disable_update & ((~bit[24]&bit[22]&bit[21]&bit[20]) | (~bit[22]&~bit[21]&bit[20]) | (~bit[23]&bit[22]&bit[21]&~bit[20]) | (bit[23]&~bit[22]&~bit[20]))) | (disable_update & out[1]);

		out[0] = (~disable_update & ((bit[23]&bit[21]&~bit[20]) | (~bit[23]&~bit[22]&~bit[21]&~bit[20]) | (bit[23]&bit[22]&~bit[21]&~bit[20]) | (~bit[23]&~bit[21]&bit[20]) | (~bit[23]&bit[22]&bit[21]&~bit[20]) | (bit[23]&bit[22]&bit[21]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[19]);
		control=control & bit[19];

		//-------------20--------------

		out[5] = (~disable_update & ((~bit[25]&bit[24]&~bit[23]&bit[22]&bit[21]))) | (disable_update & out[5]);

		out[4] = (~disable_update & ((~bit[24]&~bit[23]&bit[22]&~bit[21]) | (bit[25]&bit[24]&~bit[23]&bit[22]&bit[21]) | (~bit[24]&bit[23]&bit[22]&bit[21]) | (bit[24]&bit[23]&bit[22]&bit[21]) | (~bit[22]&~bit[21]))) | (disable_update & out[4]);

		out[3] = (~disable_update & ((bit[25]&bit[24]&~bit[23]&bit[22]&bit[21]) | (~bit[24]&~bit[23]&~bit[22]) | (bit[24]&~bit[23]&~bit[21]) | (bit[23]&bit[22]))) | (disable_update & out[3]);

		out[2] = (~disable_update & ((bit[25]&bit[24]&~bit[23]&bit[22]&bit[21]) | (bit[24]&~bit[23]&~bit[22]&bit[21]) | (~bit[24]&bit[23]&bit[22]&bit[21]) | (~bit[24]&~bit[22]&~bit[21]) | (bit[23]&~bit[22]&~bit[21]))) | (disable_update & out[2]);

		out[1] = (~disable_update & ((~bit[25]&bit[24]&bit[23]&bit[21]) | (~bit[24]&~bit[23]&bit[22]&~bit[21]) | (bit[25]&bit[24]&~bit[23]&bit[22]&bit[21]) | (~bit[24]&bit[23]&~bit[21]) | (bit[23]&~bit[22]&bit[21]) | (bit[24]&~bit[23]&~bit[21]))) | (disable_update & out[1]);

		out[0] = (~disable_update & ((~bit[25]&bit[23]&bit[22]&bit[21]) | (~bit[24]&bit[23]&~bit[22]&bit[21]) | (bit[25]&bit[24]&~bit[23]&bit[22]&bit[21]) | (bit[24]&~bit[23]&~bit[22]&bit[21]) | (bit[24]&bit[23]&bit[22]&bit[21]) | (bit[24]&bit[22]&~bit[21]) | (~bit[24]&~bit[23]&bit[22]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[20]);
		control=control & bit[20];

		//-------------21--------------

		out[5] = (~disable_update & ((~bit[26]&~bit[24]&bit[23]&bit[22]) | (~bit[25]&~bit[24]&bit[23]&bit[22]))) | (disable_update & out[5]);

		out[4] = (~disable_update & ((bit[26]&bit[25]&~bit[24]&bit[23]&bit[22]) | (~bit[25]&bit[24]&bit[23]&bit[22]) | (~bit[25]&~bit[24]&~bit[22]) | (bit[25]&bit[24]&bit[23]&bit[22]) | (~bit[23]&~bit[22]))) | (disable_update & out[4]);

		out[3] = (~disable_update & ((bit[26]&bit[25]&~bit[24]&bit[23]&bit[22]) | (~bit[25]&bit[24]&bit[23]&bit[22]) | (bit[25]&~bit[24]&bit[23]&~bit[22]) | (bit[24]&bit[23]&~bit[22]))) | (disable_update & out[3]);

		out[2] = (~disable_update & ((bit[26]&bit[25]&~bit[24]&bit[23]&bit[22]) | (bit[25]&bit[24]&bit[23]&bit[22]) | (bit[25]&~bit[24]&bit[23]&~bit[22]) | (~bit[24]&~bit[23]&~bit[22]) | (bit[24]&bit[23]&~bit[22]) | (~bit[24]&~bit[23]&bit[22]) | (~bit[25]&~bit[23]))) | (disable_update & out[2]);

		out[1] = (~disable_update & ((~bit[26]&bit[25]&bit[24]&bit[22]) | (~bit[26]&~bit[25]&~bit[24]&bit[22]) | (bit[25]&bit[24]&~bit[23]) | (bit[25]&~bit[24]&bit[23]&~bit[22]) | (~bit[25]&~bit[24]&~bit[23]) | (~bit[24]&~bit[23]&bit[22]))) | (disable_update & out[1]);

		out[0] = (~disable_update & ((~bit[26]&bit[24]&bit[23]&bit[22]) | (~bit[25]&bit[24]&bit[23]&~bit[22]) | (bit[26]&bit[25]&~bit[24]&bit[23]&bit[22]) | (~bit[25]&~bit[24]&bit[23]&bit[22]) | (bit[25]&bit[24]&bit[22]) | (~bit[24]&~bit[23]&~bit[22]) | (~bit[25]&~bit[24]&~bit[23]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[21]);
		control=control & bit[21];

		//-------------22--------------

		out[5] = (~disable_update & ((~bit[25]&bit[24]&bit[23]))) | (disable_update & out[5]);

		out[4] = (~disable_update & ((~bit[26]&~bit[24]&~bit[23]) | (bit[26]&bit[25]&bit[24]&bit[23]) | (~bit[26]&bit[25]&bit[24]&bit[23]) | (~bit[25]&~bit[24]&~bit[23]))) | (disable_update & out[4]);

		out[3] = (~disable_update & ((bit[26]&bit[25]&bit[24]&bit[23]) | (bit[26]&bit[25]&~bit[24]&~bit[23]) | (~bit[26]&bit[25]&bit[24]&bit[23]) | (~bit[25]&bit[24]&~bit[23]))) | (disable_update & out[3]);

		out[2] = (~disable_update & ((~bit[27]&~bit[26]&bit[24]&bit[23]) | (bit[26]&bit[25]&~bit[24]&~bit[23]) | (~bit[26]&bit[25]&bit[24]&bit[23]) | (bit[24]&~bit[23]))) | (disable_update & out[2]);

		out[1] = (~disable_update & ((bit[27]&~bit[26]&~bit[25]&bit[23]) | (~bit[27]&bit[25]&bit[24]&bit[23]) | (~bit[26]&~bit[25]&~bit[24]) | (~bit[26]&bit[25]&bit[24]&~bit[23]) | (bit[26]&bit[25]&~bit[24]&~bit[23]) | (~bit[25]&~bit[24]&bit[23]))) | (disable_update & out[1]);

		out[0] = (~disable_update & ((bit[27]&~bit[26]&~bit[25]&bit[23]) | (~bit[27]&bit[26]&bit[24]&bit[23]) | (~bit[26]&~bit[25]&~bit[23]) | (bit[26]&bit[25]&bit[24]&bit[23]) | (bit[26]&bit[25]&~bit[24]&~bit[23]) | (~bit[26]&~bit[24]&bit[23]) | (~bit[25]&~bit[24]&~bit[23]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[22]);
		control=control & bit[22];

		//-------------23--------------

		out[5] = (~disable_update & ((~bit[28]&bit[27]&~bit[26]&bit[25]&bit[24]))) | (disable_update & out[5]);

		out[4] = (~disable_update & ((bit[28]&bit[27]&~bit[26]&bit[25]&bit[24]) | (~bit[26]&~bit[25]&~bit[24]) | (bit[26]&~bit[25]&~bit[24]) | (bit[26]&bit[25]&bit[24]))) | (disable_update & out[4]);

		out[3] = (~disable_update & ((bit[28]&bit[27]&~bit[26]&bit[25]&bit[24]) | (~bit[26]&~bit[25]&bit[24]) | (bit[26]&bit[25]&bit[24]) | (bit[25]&~bit[24]))) | (disable_update & out[3]);

		out[2] = (~disable_update & ((bit[28]&bit[27]&~bit[26]&bit[25]&bit[24]) | (bit[26]&~bit[25]&bit[24]) | (~bit[26]&~bit[25]&~bit[24]) | (~bit[26]&bit[25]&~bit[24]) | (~bit[27]&bit[25]))) | (disable_update & out[2]);

		out[1] = (~disable_update & ((~bit[28]&bit[27]&bit[26]&bit[25]) | (bit[28]&bit[27]&~bit[26]&bit[25]&bit[24]) | (bit[27]&bit[26]&bit[25]&~bit[24]) | (~bit[27]&~bit[26]&~bit[24]) | (~bit[27]&bit[26]&~bit[25]))) | (disable_update & out[1]);

		out[0] = (~disable_update & ((~bit[28]&bit[27]&~bit[26]&bit[25]&bit[24]) | (~bit[27]&~bit[25]&bit[24]) | (~bit[28]&bit[26]&bit[24]) | (bit[27]&bit[26]&bit[24]) | (bit[26]&~bit[25]&~bit[24]) | (~bit[26]&bit[25]&~bit[24]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[23]);
		control=control & bit[23];

		//-------------24--------------

		out[5] = (~disable_update & ((bit[28]&~bit[27]&bit[26]&bit[25]) | (~bit[28]&~bit[27]&bit[26]&bit[25]))) | (disable_update & out[5]);

		out[4] = (~disable_update & ((bit[29]&bit[28]&bit[27]&bit[26]&bit[25]) | (bit[29]&~bit[28]&bit[27]&bit[26]&bit[25]) | (~bit[29]&bit[27]&bit[26]&bit[25]) | (bit[27]&~bit[26]&~bit[25]) | (~bit[27]&~bit[25]))) | (disable_update & out[4]);

		out[3] = (~disable_update & ((~bit[28]&~bit[27]&~bit[26]&~bit[25]) | (bit[27]&bit[26]))) | (disable_update & out[3]);

		out[2] = (~disable_update & ((~bit[29]&bit[27]&bit[26]&bit[25]) | (~bit[28]&~bit[27]&bit[26]&bit[25]) | (bit[28]&~bit[27]&~bit[26]&~bit[25]) | (bit[27]&~bit[26]&~bit[25]) | (~bit[28]&bit[27]) | (~bit[27]&~bit[26]&bit[25]))) | (disable_update & out[2]);

		out[1] = (~disable_update & ((~bit[29]&~bit[28]&bit[27]&bit[26]&bit[25]) | (bit[29]&bit[28]&bit[27]&bit[26]&bit[25]) | (~bit[28]&bit[27]&~bit[26]&~bit[25]) | (~bit[29]&bit[28]&~bit[27]&bit[25]) | (~bit[28]&~bit[27]&bit[26]&~bit[25]) | (bit[28]&~bit[27]&~bit[26]&~bit[25]) | (bit[28]&~bit[26]&bit[25]) | (~bit[27]&~bit[26]&bit[25]))) | (disable_update & out[1]);

		out[0] = (~disable_update & ((bit[29]&~bit[28]&bit[27]&bit[26]&bit[25]) | (~bit[29]&~bit[27]&bit[26]&bit[25]) | (bit[28]&~bit[27]&bit[26]&bit[25]) | (~bit[28]&~bit[27]&bit[26]&~bit[25]) | (bit[28]&~bit[27]&~bit[26]&~bit[25]) | (~bit[28]&~bit[26]&bit[25]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[24]);
		control=control & bit[24];

		//-------------25--------------

		out[5] = (~disable_update & ((~bit[29]&bit[28]&bit[27]&bit[26]) | (bit[29]&~bit[28]&bit[27]&bit[26]))) | (disable_update & out[5]);

		out[4] = (~disable_update & ((bit[29]&bit[28]&bit[27]&bit[26]) | (bit[29]&bit[28]&~bit[27]&~bit[26]) | (~bit[29]&~bit[28]&bit[27]&~bit[26]) | (bit[29]&~bit[28]&~bit[27]&~bit[26]) | (~bit[29]&~bit[27]&~bit[26]))) | (disable_update & out[4]);

		out[3] = (~disable_update & ((~bit[28]&~bit[27]&bit[26]) | (bit[29]&~bit[28]&bit[27]&~bit[26]) | (~bit[29]&bit[28]&bit[27]&~bit[26]) | (bit[29]&bit[28]&bit[27]))) | (disable_update & out[3]);

		out[2] = (~disable_update & ((bit[29]&~bit[28]&bit[27]&bit[26]) | (~bit[30]&bit[29]&bit[27]&bit[26]) | (~bit[29]&bit[28]&~bit[27]&bit[26]) | (bit[29]&~bit[28]&~bit[27]&~bit[26]) | (~bit[29]&~bit[27]&~bit[26]) | (bit[29]&~bit[28]&bit[27]&~bit[26]) | (~bit[29]&bit[28]&bit[27]&~bit[26]))) | (disable_update & out[2]);

		out[1] = (~disable_update & ((~bit[30]&bit[28]&bit[27]&bit[26]) | (bit[29]&bit[28]&~bit[27]&~bit[26]) | (~bit[30]&bit[29]&bit[27]&bit[26]) | (~bit[29]&~bit[28]&~bit[27]) | (~bit[29]&bit[28]&~bit[27]&bit[26]) | (bit[29]&~bit[28]&bit[27]&~bit[26]) | (bit[29]&bit[28]&bit[27]))) | (disable_update & out[1]);

		out[0] = (~disable_update & ((bit[30]&bit[29]&bit[27]&bit[26]) | (~bit[29]&~bit[28]&bit[27]&~bit[26]) | (bit[30]&bit[28]&bit[27]) | (bit[29]&~bit[28]&~bit[27]&~bit[26]) | (bit[28]&~bit[27]&bit[26]) | (~bit[29]&bit[28]&bit[27]&~bit[26]) | (bit[29]&bit[28]&bit[27]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[25]);
		control=control & bit[25];

		//-------------26--------------

		out[5] = (~disable_update & ((bit[30]&bit[29]&~bit[28]&bit[27]) | (~bit[29]&bit[28]&bit[27]))) | (disable_update & out[5]);

		out[4] = (~disable_update & ((~bit[30]&~bit[29]&~bit[28]&~bit[27]) | (bit[30]&~bit[29]&~bit[28]&~bit[27]) | (bit[29]&bit[28]&bit[27]))) | (disable_update & out[4]);

		out[3] = (~disable_update & ((~bit[30]&~bit[29]&~bit[28]&~bit[27]) | (~bit[30]&~bit[29]&bit[28]&~bit[27]) | (bit[29]&~bit[28]&~bit[27]) | (bit[29]&bit[28]&bit[27]))) | (disable_update & out[3]);

		out[2] = (~disable_update & ((bit[31]&~bit[30]&bit[29]&bit[28]) | (~bit[31]&~bit[30]&bit[29]&bit[28]&bit[27]) | (bit[30]&bit[29]&~bit[28]&bit[27]) | (bit[30]&~bit[29]&bit[28]&~bit[27]) | (bit[30]&~bit[29]&~bit[28]&~bit[27]) | (bit[29]&~bit[27]))) | (disable_update & out[2]);

		out[1] = (~disable_update & ((~bit[31]&~bit[30]&bit[29]&bit[28]&bit[27]) | (~bit[30]&~bit[29]&~bit[28]&~bit[27]) | (bit[30]&~bit[29]&bit[28]&~bit[27]) | (~bit[31]&bit[30]&bit[29]&bit[27]) | (~bit[30]&~bit[29]&bit[27]) | (bit[29]&~bit[28]&~bit[27]))) | (disable_update & out[1]);

		out[0] = (~disable_update & ((bit[31]&~bit[30]&bit[29]&bit[28]) | (~bit[31]&~bit[29]&bit[28]&bit[27]) | (~bit[30]&~bit[29]&bit[28]&~bit[27]) | (~bit[31]&bit[30]&bit[29]&bit[27]) | (bit[30]&~bit[29]&~bit[28]&~bit[27]) | (bit[30]&~bit[28]&bit[27]) | (~bit[30]&bit[29]&~bit[27]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[26]);
		control=control & bit[26];

		//-------------27--------------
/*
		out[4] = (~disable_update & ((~bit[29]&~bit[28]) | (~bit[30]&~bit[28]))) | (disable_update & out[4]);

		out[3] = (~disable_update & ((~bit[31]&~bit[30]&~bit[29]&bit[28]) | (bit[31]&bit[30]&bit[29]&~bit[28]) | (~bit[31]&bit[30]&bit[29]&~bit[28]))) | (disable_update & out[3]);

		out[2] = (~disable_update & ((~bit[31]&bit[30]&bit[29]&~bit[28]) | (~bit[31]&bit[30]&~bit[29]) | (bit[31]&~bit[30]&~bit[29]) | (~bit[29]&~bit[28]))) | (disable_update & out[2]);

		out[1] = (~disable_update & ((~bit[31]&~bit[30]&~bit[29]&bit[28]) | (bit[31]&bit[30]&bit[29]&~bit[28]) | (~bit[31]&bit[30]&bit[29]&~bit[28]) | (bit[31]&~bit[30]&~bit[29]) | (~bit[30]&~bit[28]))) | (disable_update & out[1]);

		out[0] = (~disable_update & ((bit[31]&~bit[29]&bit[28]) | (bit[31]&bit[30]&bit[29]&~bit[28]) | (~bit[31]&~bit[30]&~bit[28]) | (~bit[31]&bit[30]&~bit[29]))) | (disable_update & out[0]);

*/
		//-------------28--------------

		



		//out[5] = (~disable_update & (---put espresso output---)) | (disable_update & out[5]);

		out[5] = (~disable_update & ((~bit[32]&bit[31]&bit[30]&bit[29]&bit[28]) | (~bit[32]&~bit[31] &bit[30]&bit[29]&bit[28]) | (bit[32]&~bit[31]&bit[30]&bit[29]&bit[28]) | (bit[32]&bit[31]&~bit[30]&bit[29]&bit[28]) | (~bit[32]&bit[31]&~bit[30]&bit[29]&bit[28]))) | (disable_update & out[5]);

		out[4] = (~disable_update & ((bit[32]&bit[31]&bit[30]&bit[29]&bit[28]) | (~bit[29]&~bit[28]) | (~bit[30]&~bit[28]))) | (disable_update & out[4]);

		out[3] = (~disable_update & ((~bit[31]&~bit[30]&~bit[29]&bit[28]) | (bit[32]&bit[31]&bit[30]&bit[29]&bit[28]) | (bit[31]&bit[30]&bit[29]&~bit[28]) | (~bit[31]&bit[30]&bit[29]&~bit[28]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((~bit[32]&~bit[31]&bit[30]&bit[29]&bit[28]) | (bit[32]&bit[31]&~bit[30]&bit[29]&bit[28]) | (~bit[32]&bit[31]&~bit[30]&bit[29]&bit[28]) | (bit[32]&bit[31]&bit[30]&bit[29]&bit[28]) | (~bit[31]&bit[30]&bit[29]&~bit[28]) | (~bit[31]&bit[30]&~bit[29]) | (bit[31]&~bit[30]&~bit[29]&bit[28]) | (~bit[29]&~bit[28]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((bit[32]&~bit[31]&bit[30]&bit[29]&bit[28]) | (~bit[31]&~bit[30]&~bit[29]&bit[28]) | (~bit[32]&bit[31]&~bit[30]&bit[29]&bit[28]) | (bit[32]&bit[31]&bit[30]&bit[29]&bit[28]) | (bit[31]&bit[30]&bit[29]&~bit[28]) | (~bit[31]&bit[30]&bit[29]&~bit[28]) | (bit[31]&~bit[30]&~bit[29]&bit[28]) | (~bit[30]&~bit[28]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((~bit[32]&bit[31]&bit[30]&bit[29]&bit[28]) | (bit[32]&bit[31]&~bit[30]&bit[29]&bit[28]) | (~bit[31]&~bit[30]&~bit[28]) | (bit[30]&~bit[29]&bit[28]) | (bit[31]&bit[30]&bit[29]&~bit[28]) | (~bit[31]&bit[30]&~bit[29]) | (bit[31]&~bit[30]&~bit[29]&bit[28]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[27]);
		control=control & bit[27];

		//-------------28-1--------------

		out[5] = (~disable_update & ((~bit[33]&~bit[32]&bit[31]&bit[30]&bit[29]) | (bit[33]&~bit[32]&bit[31]&bit[30]&bit[29]) | (bit[32]&bit[31]&bit[30]&bit[29]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((~bit[31]&~bit[29]) | (bit[31]&~bit[30]&~bit[29]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((~bit[33]&~bit[32]&bit[31]&bit[30]&bit[29]) | (bit[32]&~bit[31]&~bit[30]&bit[29]) | (~bit[32]&bit[31]&bit[30]&~bit[29]) | (~bit[32]&~bit[31]&~bit[30]) | (bit[32]&bit[31]&bit[30]&~bit[29]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((bit[33]&~bit[32]&bit[31]&bit[30]&bit[29]) | (bit[32]&~bit[31]&~bit[30]&~bit[29]) | (bit[32]&bit[31]&bit[30]&bit[29]) | (~bit[32]&bit[31]&bit[30]&~bit[29]) | (bit[31]&~bit[30]&bit[29]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((bit[33]&~bit[32]&bit[31]&bit[30]&bit[29]) | (~bit[31]&bit[30]&bit[29]) | (bit[31]&~bit[30]&~bit[29]) | (~bit[32]&~bit[31]&~bit[30])| (bit[32]&bit[31]&bit[30]&~bit[29]) | (bit[31]&~bit[30]&bit[29]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((~bit[33]&bit[32]&bit[31]&bit[30]) | (bit[32]&~bit[31]&~bit[30]&~bit[29]) | (bit[32]&~bit[31]&~bit[30]&bit[29]) | (~bit[32]&~bit[31]&bit[30]) | (~bit[32]&bit[31]&~bit[30]) | (bit[32]&bit[31]&bit[30]&~bit[29]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[28]);
		control=control & bit[28];

		//-------------29--------------

		out[5] = (~disable_update & ((~bit[34]&~bit[33]&bit[32]&bit[31]&bit[30]) | (bit[34]&bit[33]&~bit[32]&bit[31]&bit[30]) | (~bit[34]&bit[33]&~bit[32]&bit[31]&bit[30]) | (~bit[32]&~bit[31]&~bit[30]) | (~bit[33]&~bit[32]&bit[31]&bit[30]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((~bit[34]&bit[33]&bit[32]&bit[31]&bit[30]) | (~bit[33]&bit[32]&~bit[31]&~bit[30]) | (bit[34]&bit[32]&bit[31]&bit[30]) | (bit[33]&bit[32]&~bit[31]&~bit[30]) | (~bit[33]&bit[31]&~bit[30]) | (~bit[32]&bit[31]&~bit[30]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((~bit[33]&~bit[32]&~bit[31]&bit[30]) | (~bit[33]&~bit[32]&bit[31]&bit[30]) | (~bit[33]&bit[32]&~bit[31]&~bit[30]) | (bit[33]&bit[32]&~bit[31]&~bit[30]) | (bit[33]&bit[32]&bit[31]&~bit[30]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((bit[34]&~bit[33]&bit[32]&bit[31]&bit[30]) | (~bit[34]&bit[33]&bit[32]&bit[31]&bit[30]) | (bit[34]&bit[33]&~bit[32]&bit[31]&bit[30]) | (~bit[34]&bit[33]&~bit[32]&bit[31]&bit[30]) | (~bit[33]&~bit[32]&~bit[31]&bit[30]) | (~bit[33]&bit[32]&~bit[31]&~bit[30]) | (bit[33]&~bit[32]&~bit[31]&bit[30]) | (~bit[32]&bit[31]&~bit[30]) | (bit[33]&bit[32]&bit[31]&~bit[30]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((~bit[34]&~bit[33]&bit[32]&bit[31]&bit[30]) | (~bit[33]&~bit[32]&~bit[30]) | (~bit[34]&bit[33]&~bit[32]&bit[31]&bit[30]) | (bit[34]&bit[32]&bit[31]&bit[30]) | (bit[33]&bit[32]&~bit[31]&~bit[30]) | (bit[33]&~bit[32]&~bit[31]&bit[30]) | (~bit[33]&bit[31]&~bit[30]) | (bit[33]&bit[32]&bit[31]&~bit[30]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((~bit[34]&~bit[33]&~bit[32]&bit[31]) | (bit[34]&bit[33]&~bit[32]&bit[31]&bit[30]) | (~bit[33]&~bit[32]&bit[31]&~bit[30]) | (~bit[33]&bit[32]&~bit[31]) | (bit[34]&bit[32]&bit[31]&bit[30]) | (bit[33]&bit[32]&~bit[31]&~bit[30]) | (bit[33]&~bit[32]&~bit[31]&bit[30]) | (bit[33]&bit[32]&bit[31]&~bit[30]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[29]);
		control=control & bit[29];

		//-------------30--------------

		out[5] = (~disable_update & ((~bit[33]&bit[32]&bit[31]) | (bit[34]&bit[33]&~bit[32]&bit[31]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((bit[35]&bit[34]&bit[33]&bit[32]&bit[31]) | (bit[34]&~bit[33]&~bit[32]&~bit[31]) | (~bit[34]&~bit[33]&~bit[32]&~bit[31]) | (~bit[35]&bit[33]&bit[32]&bit[31]) | (~bit[34]&bit[33]&bit[32]&bit[31]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((bit[35]&bit[34]&bit[33]&bit[32]&bit[31]) | (~bit[34]&~bit[33]&~bit[32]&bit[31]) | (~bit[35]&bit[33]&bit[32]&bit[31]) | (~bit[34]&~bit[33]&bit[32]&~bit[31]) | (~bit[34]&bit[33]&~bit[31]) | (bit[34]&bit[32]&~bit[31]) | (bit[33]&~bit[32]&~bit[31]) | (~bit[34]&bit[33]&bit[32]&bit[31]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((~bit[35]&~bit[34]&bit[32]&bit[31]) | (bit[34]&bit[33]&~bit[32]&bit[31]) | (~bit[35]&bit[33]&bit[32]&bit[31]) | (~bit[34]&~bit[33]&bit[32]&~bit[31]) | (bit[33]&~bit[32]&~bit[31]) | (~bit[34]&bit[33]&bit[32]&bit[31]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((~bit[35]&~bit[34]&bit[33]&bit[32]) | (bit[35]&~bit[34]&~bit[33]&bit[32]&bit[31]) | (~bit[35]&bit[34]&~bit[33]&bit[32]) | (bit[35]&bit[34]&bit[33]&bit[32]&bit[31]) | (bit[34]&~bit[33]&bit[32]&~bit[31]) | (~bit[34]&~bit[33]&~bit[32]&~bit[31]) | (bit[34]&bit[33]&~bit[32]&bit[31]) | (bit[34]&~bit[33]&~bit[32]&bit[31]) | (~bit[34]&bit[33]&~bit[31]) | (bit[33]&~bit[32]&~bit[31]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((~bit[35]&bit[33]&~bit[32]&bit[31]) | (bit[35]&~bit[34]&~bit[33]&bit[32]&bit[31]) | (bit[34]&~bit[33]&~bit[32]&~bit[31]) | (~bit[35]&~bit[34]&bit[32]&bit[31]) | (~bit[34]&bit[33]&~bit[32]) | (bit[34]&~bit[33]&~bit[32]&bit[31]) | (~bit[34]&~bit[33]&bit[32]&~bit[31]) | (bit[34]&bit[32]&~bit[31]) | (~bit[34]&bit[33]&bit[32]&bit[31]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[30]);
		control=control & bit[30];

		//-------------31--------------

		out[5] = (~disable_update & ((bit[36]&bit[35]&~bit[34]&bit[33]&bit[32]) | (~bit[36]&bit[34]&bit[33]&bit[32]) | (~bit[36]&bit[35]&~bit[34]&bit[33]&bit[32]) | (~bit[35]&bit[34]&bit[33]&bit[32]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((bit[36]&bit[35]&bit[34]&bit[33]&bit[32]) | (~bit[35]&~bit[34]&bit[33]&~bit[32]) | (~bit[33]&~bit[32]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((~bit[36]&bit[35]&~bit[34]&bit[33]&bit[32]) | (bit[36]&bit[35]&bit[34]&bit[33]&bit[32]) | (~bit[35]&~bit[34]&~bit[33]&bit[32]) | (~bit[35]&~bit[34]&~bit[33]&~bit[32]) | (bit[35]&~bit[34]&~bit[33]&bit[32]) | (~bit[35]&bit[34]&bit[33]&~bit[32]) | (bit[35]&bit[33]&~bit[32]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((bit[36]&bit[35]&~bit[34]&bit[33]&bit[32]) | (bit[36]&bit[35]&bit[34]&bit[33]&bit[32]) | (bit[35]&~bit[34]&~bit[32]) | (~bit[35]&bit[34]&bit[33]&~bit[32]) | (bit[34]&~bit[33]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((~bit[36]&bit[35]&~bit[34]&bit[33]&bit[32]) | (bit[36]&bit[35]&bit[34]&bit[33]&bit[32]) | (~bit[35]&bit[34]&bit[33]&bit[32]) | (~bit[35]&~bit[34]&bit[33]&~bit[32]) | (~bit[35]&~bit[34]&~bit[33]&bit[32]) | (bit[35]&~bit[34]&~bit[32]) | (bit[35]&bit[33]&~bit[32]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((~bit[36]&~bit[35]&bit[34]) | (~bit[35]&~bit[34]&~bit[33]&~bit[32]) | (bit[35]&~bit[34]&~bit[33]&bit[32]) | (bit[35]&~bit[34]&~bit[32]) | (~bit[35]&bit[34]&~bit[33]) | (~bit[35]&bit[34]&bit[33]&~bit[32]) | (bit[35]&bit[33]&~bit[32]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[31]);
		control=control & bit[31];

		//-------------32--------------

		out[5] = (~disable_update & ((bit[36]&bit[35]&bit[34]&bit[33]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((~bit[36]&bit[34]&~bit[33]) | (~bit[35]&bit[34]&~bit[33]) | (~bit[34]&~bit[33]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((bit[36]&bit[35]&bit[34]&~bit[33]) | (~bit[35]&~bit[34]) | (~bit[34]&~bit[33]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((~bit[37]&bit[36]&bit[35]&bit[33]) | (~bit[35]&bit[34]&~bit[33]) | (bit[36]&bit[35]&bit[34]&~bit[33]) | (bit[35]&~bit[34]&bit[33]) | (~bit[36]&~bit[35]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((bit[36]&~bit[35]&bit[34]&bit[33]) | (~bit[36]&bit[35]&~bit[34]&bit[33]) | (bit[36]&~bit[35]&~bit[34]&~bit[33]) | (~bit[36]&bit[34]&~bit[33]) | (bit[36]&bit[35]&bit[34]&~bit[33]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((~bit[36]&~bit[34]&~bit[33]) | (bit[36]&~bit[35]&~bit[34]) | (bit[36]&bit[35]&bit[34]&~bit[33]) | (bit[35]&~bit[34]&bit[33]) | (~bit[36]&bit[35]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[32]);
		control=control & bit[32];

		//-------------33--------------

		out[5] = (~disable_update & ((~bit[38]&~bit[37]&bit[35]&bit[34]) | (~bit[36]&bit[35]&bit[34]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((bit[37]&bit[36]&bit[35]&bit[34]) | (~bit[37]&bit[36]&bit[35]&~bit[34]) | (bit[38]&bit[36]&bit[35]&bit[34]) | (~bit[36]&bit[35]&~bit[34]) | (~bit[35]&~bit[34]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((bit[37]&bit[36]&bit[35]&bit[34]) | (bit[37]&bit[36]&bit[35]&~bit[34]) | (bit[38]&bit[36]&bit[35]&bit[34]) | (~bit[37]&~bit[36]) | (~bit[35]&~bit[34]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((~bit[38]&bit[37]&bit[35]&bit[34]) | (bit[38]&~bit[37]&bit[36]&bit[34]) | (bit[37]&~bit[36]&~bit[35]&bit[34]) | (~bit[36]&~bit[35]&~bit[34]) | (~bit[37]&bit[36]&~bit[35]) | (bit[37]&bit[36]&bit[35] &~bit[34]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((bit[37]&~bit[36]&~bit[35]&bit[34]) | (~bit[37]&~bit[36]&~bit[34]) | (~bit[36]&bit[35]&bit[34]) | (bit[38]&bit[36]&bit[35]&bit[34]) | (~bit[36]&bit[35]&~bit[34]) | (bit[37]&bit[36]&~bit[35]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((~bit[38]&~bit[37]&bit[35]&bit[34]) | (~bit[37]&bit[36]&bit[35]&~bit[34]) | (bit[37]&~bit[35]&~bit[34]) | (bit[37]&bit[36]&bit[35]&~bit[34]) | (~bit[37]&~bit[35]&bit[34]) | (bit[38]&bit[36]&bit[35]&bit[34]) | (bit[37]&bit[36]&~bit[35]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[33]);
		control=control & bit[33];

		//-------------34--------------

		out[5] = (~disable_update & ((~bit[39]&bit[38]&bit[37]&~bit[36]&bit[35]) | (bit[39]&bit[38]&bit[37]&~bit[36]&bit[35]) | (bit[38]&~bit[37]&bit[36]&bit[35]) | (~bit[38]&bit[36]&bit[35]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((~bit[38]&~bit[36]&~bit[35]) | (bit[38]&bit[37]&bit[36]&bit[35]) | (bit[38]&~bit[37]&~bit[36]&~bit[35]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((bit[39]&~bit[38]&~bit[37]&bit[36]&bit[35]) | (~bit[39]&bit[38]&bit[37]&bit[35]) | (bit[39]&bit[38]&bit[37]&~bit[36]&bit[35]) | (~bit[39]&~bit[38]&~bit[37]&bit[36]) | (~bit[38]&~bit[37]&~bit[35]) | (bit[38]&bit[37]&~bit[36]&~bit[35]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((~bit[39]&bit[38]&bit[37]&~bit[36]&bit[35]) | (~bit[39]&~bit[38]&bit[37]&bit[36]) | (~bit[38]&~bit[37]&~bit[36]&bit[35]) | (~bit[38]&bit[37]&bit[36]&~bit[35]) | (bit[38]&~bit[37]&bit[36]&bit[35]) | (bit[38]&~bit[37]&~bit[36]&~bit[35]) | (bit[38]&bit[36]&~bit[35]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((bit[39]&bit[38]&bit[37]&~bit[36]&bit[35]) | (~bit[39]&~bit[38]&~bit[37]&bit[36]) | (bit[38]&bit[37]&bit[36]&bit[35]) | (~bit[38]&bit[37]&bit[36]&~bit[35]) | (bit[38]&bit[37]&~bit[36]&~bit[35]) | (~bit[37]&bit[36]&~bit[35]) | (bit[38]&~bit[37]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((bit[39]&~bit[38]&~bit[37]&bit[36]&bit[35]) | (~bit[38]&bit[37]&~bit[36]&bit[35]) | (bit[39]&bit[38]&bit[37]&~bit[36]&bit[35]) | (~bit[39]&bit[38]&~bit[37]&bit[36]) | (bit[38]&~bit[37]&~bit[36]&~bit[35]) | (bit[38]&bit[37]&~bit[36]&~bit[35]) | (bit[38]&bit[36]&~bit[35]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[34]);
		control=control & bit[34];

		//-------------35--------------
/*
		//out[5] = (~disable_update & ((~bit[39]&~bit[38]&~bit[37]&~bit[36]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((~bit[39]&~bit[38]&~bit[37]&~bit[36]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((~bit[39]&bit[38]&~bit[37]&~bit[36]) | (bit[39]&bit[38]&~bit[37]&~bit[36]) | (bit[39]&~bit[38]&~bit[37]&~bit[36]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((~bit[39]&~bit[38]&bit[37]&~bit[36]) | (bit[39]&~bit[38]&~bit[37]&~bit[36]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((bit[39]&~bit[38]&bit[37]&~bit[36]) | (~bit[39]&bit[38]&~bit[37]&~bit[36]) | (bit[39]&~bit[38]&~bit[37]&~bit[36]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((~bit[39]&bit[38]&bit[37]&~bit[36]) | (bit[39]&~bit[38]&bit[37]&~bit[36]) | (~bit[39]&~bit[38]&~bit[37]&~bit[36]) | (bit[39]&bit[38]&~bit[37]&~bit[36]))) | (disable_update & out[0]);
*/
		//-------------36--------------

		out[5] = (~disable_update & ((bit[40]&~bit[39]&~bit[38]&~bit[37]&bit[36]) | (~bit[40]&~bit[38]&~bit[37]&bit[36]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((~bit[40]&bit[39]&~bit[38]&bit[37]&bit[36]) | (~bit[40]&~bit[39]&~bit[38]&bit[37]&bit[36]) | (~bit[39]&~bit[38]&~bit[37]&~bit[36]) | (bit[40]&~bit[38]&bit[37]&bit[36]) | (bit[40]&bit[39]&~bit[38]&~bit[37]&bit[36]) | (bit[38]&~bit[37]&bit[36]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((~bit[40]&bit[39]&bit[38]&~bit[37]&bit[36]) | (bit[39]&bit[38]&~bit[37]&~bit[36]) | (~bit[40]&~bit[39]&~bit[38]&~bit[37]&bit[36]) | (bit[40]&bit[39]&bit[38]&bit[37]&bit[36]) | (~bit[39]&bit[38]&~bit[37]&~bit[36]) | (~bit[40]&bit[38]&bit[37]&bit[36]) | (bit[40]&bit[39]&~bit[38]&~bit[37]&bit[36]) | (bit[39]&~bit[38]&~bit[37]&~bit[36]) | (~bit[39]&bit[38]&bit[36]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((bit[40]&~bit[39]&~bit[38]&~bit[37]&bit[36]) | (bit[40]&bit[39]&bit[38]&~bit[37]&bit[36]) | (~bit[39]&~bit[38]&bit[37]&~bit[36]) | (~bit[40]&~bit[39]&~bit[38]&bit[37]&bit[36]) | (~bit[40]&bit[38]&bit[37]&bit[36]) | (bit[40]&bit[39]&~bit[38]&~bit[37]&bit[36]) | (bit[39]&~bit[38]&~bit[37]&~bit[36]) | (~bit[39]&bit[38]&bit[36]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((bit[40]&~bit[39]&~bit[38]&bit[37]&bit[36]) | (~bit[40]&~bit[39]&bit[38]&bit[37]&bit[36]) | (~bit[40]&bit[39]&~bit[38]&bit[37]&bit[36]) | (~bit[40]&bit[39]&bit[38]&~bit[37]&bit[36]) | (~bit[40]&~bit[39]&~bit[38]&~bit[37]&bit[36]) | (bit[39]&~bit[38]&bit[37]&~bit[36]) | (bit[40]&bit[39]&bit[38]&bit[37]&bit[36]) | (~bit[39]&bit[38]&~bit[37]&~bit[36]) | (bit[40]&bit[39]&~bit[38]&~bit[37]&bit[36]) | (bit[39]&~bit[38]&~bit[37]&~bit[36]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((bit[40]&bit[39]&bit[38]&~bit[37]&bit[36]) | (bit[39]&bit[38]&~bit[37]&~bit[36]) | (bit[39]&~bit[38]&bit[37]&~bit[36]) | (bit[40]&bit[39]&bit[38]&bit[37]&bit[36]) | (~bit[40]&~bit[39]&~bit[37]&bit[36]) | (~bit[39]&~bit[38]&~bit[37]&~bit[36]) | (~bit[40]&~bit[38]&~bit[37]&bit[36]) | (bit[40]&~bit[38]&bit[37]&bit[36]) | (~bit[39]&bit[38]&bit[37]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[35]);
		control=control & bit[35];

		//-------------36-1--------------

		out[5] = (~disable_update & ((bit[40]&bit[39]&bit[38]&~bit[37]) | (~bit[40]&~bit[38]&bit[37]) | (~bit[41]&~bit[38]&bit[37]) | (~bit[39]&~bit[38]&bit[37]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((bit[41]&bit[40]&bit[39]&~bit[38]&bit[37]) | (bit[40]&~bit[39]&bit[38]&bit[37]) | (~bit[40]&bit[38]&bit[37]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((bit[41]&bit[40]&bit[39]&~bit[38]&bit[37]) | (bit[41]&bit[40]&bit[39]&bit[38]&bit[37]) | (bit[40]&bit[39]&bit[38]&~bit[37]) | (~bit[41]&bit[40]&bit[39]&bit[38]) | (~bit[40]&~bit[39]&bit[38]&bit[37]) | (~bit[39]&~bit[38]&~bit[37]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((bit[41]&bit[40]&bit[39]&~bit[38]&bit[37]) | (~bit[40]&~bit[39]&bit[38]&~bit[37]) | (~bit[41]&bit[40]&bit[39]&bit[38]) | (~bit[41]&bit[39]&bit[38]&bit[37]) | (bit[40]&~bit[39]&bit[38]&bit[37]) | (bit[39]&~bit[38]&~bit[37]) | (~bit[39]&~bit[38]&bit[37]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((bit[41]&bit[40]&bit[39]&bit[38]&bit[37]) | (bit[40]&~bit[39]&bit[38]&~bit[37]) | (~bit[41]&~bit[39]&bit[38]&bit[37]) | (bit[39]&~bit[38]&~bit[37]) | (~bit[40]&~bit[38]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((bit[41]&bit[40]&bit[39]&bit[38]&bit[37]) | (~bit[41]&bit[39]&bit[38]&~bit[37]) | (~bit[41]&~bit[40]&~bit[38]&bit[37]) | (bit[41]&~bit[39]&bit[38]&bit[37]) | (~bit[41]&~bit[39]&~bit[38]&bit[37]) | (~bit[40]&bit[39]&~bit[37]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[36]);
		control=control & bit[36];
		//-------------37--------------

		out[5] = (~disable_update & ((~bit[41]&~bit[40]&~bit[39]&bit[38]) | (bit[40]&bit[39]&~bit[38]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((~bit[41]&~bit[40]&bit[39]&bit[38]) | (~bit[41]&bit[40]&~bit[39]&bit[38]) | (bit[41]&~bit[39]&bit[38]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((~bit[42]&bit[40]&bit[39]&bit[38]) | (~bit[41]&~bit[40]&~bit[39]&~bit[38]) | (~bit[41]&bit[40]&bit[39]) | (~bit[41]&bit[40]&~bit[39]&bit[38]) | (bit[41]&~bit[40]&bit[38]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((bit[42]&bit[41]&bit[40]&bit[39]) | (~bit[41]&bit[40]&~bit[39]&~bit[38]) | (~bit[42]&bit[40]&~bit[38]) | (~bit[41]&bit[40]&~bit[39]&bit[38]) | (~bit[42]&~bit[40]&bit[38]) | (bit[41]&~bit[39]&~bit[38])| ( bit[41]&~bit[40]&bit[38]) | (bit[41]&~bit[39]&bit[38]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((~bit[42]&bit[41]&bit[40]&bit[39]&~bit[38]) | (~bit[42]&~bit[41]&bit[40]&bit[39]&bit[38]) | (bit[42]&~bit[41]&~bit[40]&~bit[39]&bit[38]) | (~bit[42]&bit[41]&~bit[40]&bit[38]) | (bit[42]&bit[41]&bit[40]&bit[39]) | (~bit[41]&~bit[40]&bit[39]&~bit[38]) | (~bit[41]&bit[40]&~bit[39]&~bit[38]) | (bit[41]&~bit[40]&~bit[39]) | (bit[41]&~bit[39]&bit[38]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((bit[42]&~bit[41]&bit[40]&bit[39]&bit[38]) | (~bit[42]&bit[41]&bit[40]&bit[39]&~bit[38]) | (bit[42]&~bit[41]&~bit[40]&~bit[39]&bit[38]) | (~bit[41]&~bit[40]&~bit[39]&~bit[38]) | (~bit[41]&~bit[40]&bit[39]&~bit[38]) | (~bit[42]&~bit[39]&bit[38]) | (bit[41]&~bit[39]&~bit[38]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[37]);
		control=control & bit[37];

		//-------------38--------------

		out[5] = (~disable_update & ((~bit[42]&~bit[41]&~bit[40]&bit[39]) | (bit[43]&~bit[42]&~bit[41]&bit[40]&~bit[39]) | (bit[42]&bit[41]&bit[40]&~bit[39]) | (bit[43]&~bit[42]&bit[41]&bit[40]&~bit[39]) | (~bit[43]&bit[41]&bit[40]&~bit[39]) | (bit[42]&~bit[41]&bit[40]&~bit[39]) | (~bit[43]&~bit[41]&bit[40]&~bit[39]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((bit[43]&bit[42]&bit[41]&~bit[40]&bit[39]) | (bit[43]&~bit[42]&bit[41]&~bit[40]&bit[39]) | (~bit[43]&bit[41]&~bit[40]&bit[39]) | (bit[42]&~bit[41]&~bit[40]&bit[39]) | (~bit[41]&bit[40]&bit[39]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((bit[43]&~bit[42]&bit[41]&~bit[40]&bit[39]) | (bit[43]&~bit[42]&~bit[41]&bit[40]&~bit[39]) | (bit[43]&bit[42]&bit[41]&bit[40]&bit[39]) | (~bit[43]&bit[41]&~bit[40]&bit[39]) | (bit[42]&~bit[41]&~bit[40]&bit[39]) | (~bit[43]&bit[41]&bit[40]&bit[39]) | (~bit[43]&~bit[42]&bit[41]&bit[40]) | (bit[42]&~bit[41]&bit[40]&~bit[39]) | (~bit[43]&~bit[41]&bit[40]&~bit[39]) | (~bit[42]&bit[41]&bit[40]&bit[39]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((bit[43]&bit[42]&bit[41]&~bit[40]&bit[39]) | (bit[42]&bit[41]&bit[40]&~bit[39]) | (bit[43]&~bit[42]&bit[41]&bit[40]&~bit[39]) | (bit[42]&~bit[41]&~bit[40]&bit[39]) | (~bit[43]&bit[41]&bit[40]&bit[39]) | (~bit[42]&~bit[41]&bit[40]) | (~bit[42]&bit[41]&bit[40]&bit[39]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((bit[43]&bit[42]&bit[41]&~bit[40]&bit[39]) | (~bit[43]&~bit[42]&~bit[40]&bit[39]) | (bit[43]&bit[42]&bit[41]&bit[40]&bit[39]) | (bit[43]&~bit[42]&bit[41]&bit[40]&~bit[39]) | (~bit[41]&~bit[40]&~bit[39]) | (~bit[43]&bit[42]&~bit[41]&bit[40]) | (bit[42]&~bit[41]&bit[40]&~bit[39]) | (~bit[43]&~bit[41]&bit[40]&~bit[39]) | (~bit[42]&bit[41]&bit[40]&bit[39]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((~bit[43]&bit[42]&~bit[41]&bit[39]) | (~bit[43]&~bit[41]&bit[40]&bit[39]) | (bit[43]&~bit[42]&bit[41]&~bit[40]&bit[39]) | (bit[43]&~bit[42]&~bit[41]&bit[40]&~bit[39]) | (bit[43]&bit[42]&bit[41]&bit[40]&bit[39]) | (bit[43]&~bit[42]&bit[41]&bit[40]&~bit[39]) | (~bit[43]&bit[41]&bit[40]&~bit[39]) | (~bit[42]&~bit[40]&~bit[39]) | (~bit[43]&bit[42]&~bit[41]&bit[40]) | (~bit[43]&~bit[42]&bit[41]&bit[40]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[38]);
		control=control & bit[38];

		//-------------39--------------

		out[5] = (~disable_update & ((~bit[43]&~bit[41]&bit[40]) | (~bit[42]&~bit[41]&bit[40]) | (bit[42]&bit[41]&~bit[40]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((bit[44]&~bit[42]&bit[41]&bit[40]) | (~bit[44]&~bit[42]&bit[41]&bit[40]) | (bit[43]&bit[42]&~bit[41]&bit[40]) | (~bit[43]&bit[41]&bit[40]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((~bit[44]&~bit[43]&~bit[42]&bit[40]) | (~bit[43]&~bit[42]&bit[41]&bit[40]) | (~bit[42]&~bit[41]&~bit[40]) | (bit[42]&bit[41]&~bit[40]) | (bit[43]&bit[42]&bit[40]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((bit[44]&~bit[43]&~bit[42]&~bit[41]&bit[40]) | (bit[43]&bit[42]&~bit[41]&~bit[40]) | (~bit[44]&~bit[42]&bit[41]&bit[40]) | (~bit[44]&bit[43]&bit[40]) | (~bit[43]&bit[42]&~bit[40]) | (bit[43]&bit[42]&bit[40]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((bit[44]&~bit[43]&~bit[42]&~bit[41]&bit[40]) | (bit[44]&bit[43]&~bit[42]&bit[40]) | (~bit[43]&bit[42]&~bit[41]&~bit[40]) | (~bit[43]&~bit[42]&bit[41]&~bit[40]) | (bit[43]&bit[42]&~bit[41]&bit[40]) | (~bit[44]&bit[42]&bit[41]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((bit[44]&bit[42]&bit[41]&~bit[40]) | (~bit[43]&~bit[42]&~bit[41]&~bit[40]) | (~bit[44]&bit[42]&~bit[41]&bit[40]) | (bit[43]&bit[42]&~bit[41]&~bit[40]) | (bit[44]&~bit[42]&bit[41]&bit[40]) | (bit[43]&bit[42]&bit[41]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[39]);
		control=control & bit[39];

		//-------------40--------------

		out[5] = (~disable_update & ((~bit[44]&bit[43]&~bit[42]&bit[41]) | (~bit[45]&bit[44]&~bit[42]&bit[41]) | (bit[44]&~bit[43]&~bit[42]&bit[41]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((~bit[45]&~bit[44]&bit[42]&bit[41]) | (bit[45]&bit[44]&bit[43]&~bit[42]&bit[41]) | (~bit[43]&bit[42]&bit[41]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((~bit[45]&~bit[43]&bit[42]&bit[41]) | (~bit[45]&bit[44]&bit[43]&bit[42]&bit[41]) | (bit[45]&bit[43]&bit[42]&bit[41]) | (bit[45]&bit[44]&bit[43]&~bit[42]&bit[41]) | (~bit[44]&~bit[43]&bit[42]&bit[41]) | (bit[44]&~bit[43]&~bit[42]&~bit[41]) | (~bit[44]&~bit[42]&~bit[41]) | (bit[44]&~bit[43]&~bit[42]&bit[41]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((bit[45]&~bit[44]&bit[43]&bit[42]&bit[41]) | (~bit[45]&~bit[44]&bit[43]&~bit[42]&bit[41]) | (~bit[44]&~bit[43]&~bit[41]) | (~bit[45]&bit[44]&bit[43]&bit[42]&bit[41]) | (bit[44]&bit[43]&~bit[42]&~bit[41]) | (bit[45]&bit[44]&bit[43]&~bit[42]&bit[41]) | (~bit[43]&bit[42]&~bit[41]) | (bit[44]&~bit[43]&~bit[42]&bit[41]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((~bit[45]&bit[44]&~bit[43]&~bit[42]) | (bit[45]&bit[43]&bit[42]&bit[41]) | (~bit[44]&bit[43]&~bit[42]&bit[41]) | (~bit[44]&~bit[43]&bit[42]&bit[41]) | (bit[44]&~bit[43]&~bit[42]&~bit[41]) | (bit[43]&~bit[41]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((bit[45]&bit[44]&bit[42]&bit[41]) | (~bit[45]&~bit[44]&~bit[43]&bit[41]) | (~bit[45]&bit[44]&bit[43]&bit[42]&bit[41]) | (bit[44]&bit[43]&~bit[42]&~bit[41]) | (bit[45]&bit[44]&bit[43]&~bit[42]&bit[41]) | (~bit[45]&bit[44]&~bit[42]&bit[41]) | (bit[44]&~bit[43]&~bit[42]&~bit[41]) | (~bit[44]&bit[42]&~bit[41]) | (~bit[44]&~bit[42]&bit[41]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[40]);
		control=control & bit[40];

		//-------------41--------------

		out[5] = (~disable_update & ((~bit[46]&bit[45]&~bit[44]&bit[43]&~bit[42]) | (bit[46]&~bit[45]&bit[44]&bit[43]&~bit[42]) | (bit[46]&bit[45]&~bit[44]&~bit[43]&bit[42]) | (bit[46]&bit[45]&bit[43]&~bit[42]) | (~bit[46]&~bit[44]&~bit[43]&bit[42]) | (~bit[46]&bit[44]&bit[43]&~bit[42]) | (~bit[45]&~bit[43]&bit[42]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((~bit[46]&bit[45]&~bit[44]&bit[43]&~bit[42]) | (bit[45]&bit[44]&~bit[43]&bit[42]) | (bit[43]&bit[42]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((~bit[46]&~bit[45]&~bit[44]&bit[42]) | (bit[46]&~bit[45]&bit[44]&bit[43]&~bit[42]) | (~bit[45]&~bit[44]&~bit[43]&~bit[42]) | (~bit[46]&~bit[44]&bit[43]&bit[42]) | (bit[46]&bit[45]&bit[43]&~bit[42]) | (~bit[45]&~bit[44]&bit[43]&bit[42]) | (~bit[46]&bit[44]&bit[43]&~bit[42]) | (bit[45]&bit[44]&~bit[43]&bit[42]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((bit[46]&~bit[45]&~bit[44]&~bit[43]&bit[42]) | (bit[45]&~bit[44]&~bit[43]&~bit[42]) | (~bit[46]&bit[45]&~bit[43]&bit[42]) | (~bit[45]&bit[44]&bit[43]) | (bit[46]&bit[45]&~bit[44]&bit[43]) | (bit[45]&bit[44]&~bit[43]&bit[42]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((~bit[46]&~bit[45]&~bit[44]&bit[43]&bit[42]) | (bit[46]&~bit[45]&~bit[44]&~bit[43]&bit[42]) | (~bit[45]&bit[44]&~bit[43]&~bit[42]) | (~bit[46]&~bit[45]&bit[44]&~bit[43]) | (bit[46]&bit[45]&~bit[44]&~bit[43]&bit[42]) | (~bit[46]&bit[45]&bit[44]&bit[42]) | (bit[45]&~bit[44]&~bit[43]&~bit[42]) | (bit[46]&bit[45]&bit[43]&~bit[42]) | (bit[46]&bit[45]&~bit[44]&bit[43]) | (~bit[46]&bit[44]&bit[43]&~bit[42]) | (bit[45]&bit[44]&~bit[43]&bit[42]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((~bit[46]&bit[45]&bit[44]&~bit[42]) | (~bit[46]&~bit[45]&bit[43]&bit[42]) | (bit[46]&~bit[45]&bit[44]&bit[43]&~bit[42]) | (bit[46]&bit[45]&bit[43]&bit[42]) | (bit[46]&bit[45]&~bit[44]&~bit[43]&bit[42]) | (~bit[46]&bit[45]&~bit[43]&bit[42]) | (~bit[46]&~bit[44]&~bit[43]&bit[42]) | (bit[44]&~bit[43]&~bit[42]) | (~bit[45]&~bit[44]&bit[43]&bit[42]) | (bit[46]&bit[45]&~bit[44]&bit[43]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[41]);
		control=control & bit[41];

		//-------------42--------------
		out[5] = (~disable_update & ((bit[46]&bit[45]&~bit[44]&bit[43]) | (~bit[46]&bit[45]&~bit[44]&bit[43]) | (~bit[45]&bit[44]&bit[43]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((~bit[46]&bit[45]&bit[44]&bit[43]) | (~bit[46]&~bit[45]&~bit[44]&~bit[43]) | (~bit[47]&bit[45]&bit[44]&bit[43]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((~bit[46]&~bit[45]&bit[44]&~bit[43]) | (~bit[47]&~bit[46]&bit[44]&bit[43]) | (bit[47]&bit[46]&bit[45]&bit[44]&bit[43]) | (bit[46]&~bit[45]&~bit[44]&~bit[43]) | (bit[45]&~bit[44]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((bit[47]&bit[46]&bit[45]&bit[44]&bit[43]) | (bit[46]&~bit[45]&~bit[44]&~bit[43]) | (~bit[46]&bit[45]&~bit[44]&bit[43]) | (bit[46]&~bit[45]&bit[44]) | (~bit[46]&bit[45]&~bit[43]) | (bit[47]&~bit[46]&bit[44]&bit[43]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((bit[47]&bit[46]&bit[45]&bit[44]&bit[43]) | (~bit[47]&bit[45]&~bit[44]&bit[43]) | (~bit[47]&bit[46]&bit[44]) | (bit[47]&~bit[46]&bit[44]&bit[43]) | (~bit[46]&~bit[44]&bit[43]) | (bit[46]&~bit[43]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((bit[46]&bit[45]&bit[44]&~bit[43]) | (bit[46]&bit[45]&~bit[44]&bit[43]) | (bit[46]&~bit[45]&~bit[44]&~bit[43]) | (~bit[47]&bit[45]&~bit[44]&bit[43]) | (bit[47]&~bit[46]&bit[44]&bit[43]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[42]);
		control=control & bit[42];

		//-------------43--------------

		out[5] = (~disable_update & ((~bit[47]&~bit[46]&bit[44]) | (~bit[45]&bit[44]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((~bit[48]&~bit[47]&~bit[46]&~bit[45]&bit[44]) | (bit[46]&bit[45]&bit[44]) | (bit[47]&~bit[46]&bit[45]&bit[44]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((~bit[47]&~bit[46]&~bit[45]&~bit[44]) | (bit[48]&~bit[47]&~bit[46]&~bit[45]&bit[44]) | (bit[47]&~bit[46]&~bit[45]&bit[44]) | (bit[47]&~bit[46]&bit[45]&bit[44]) | (~bit[47]&bit[46]&~bit[45]) | (bit[47]&~bit[46]&~bit[45]&~bit[44]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((~bit[48]&~bit[47]&bit[46]&bit[45]&bit[44]) | (~bit[47]&~bit[46]&~bit[45]&~bit[44]) | (bit[47]&bit[46]&~bit[45]&~bit[44]) | (~bit[46]&bit[45]&~bit[44]) | (bit[48]&~bit[47]&~bit[46]&~bit[45]&bit[44]) | (bit[47]&~bit[46]&~bit[45]&bit[44]) | (~bit[48]&bit[47]&~bit[45]&bit[44]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((~bit[48]&bit[46]&~bit[45]&bit[44]) | (bit[48]&~bit[47]&bit[46]&bit[45]) | (~bit[48]&bit[47]&bit[45]&bit[44]) | (bit[46]&bit[45]&~bit[44]) | (bit[48]&~bit[47]&~bit[46]&~bit[45]&bit[44]) | (bit[47]&bit[46]&~bit[45]) | (bit[47]&~bit[46]&~bit[45]&~bit[44]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((bit[48]&~bit[47]&bit[46]&bit[45]) | (~bit[48]&~bit[47]&bit[46]&bit[45]&bit[44]) | (bit[47]&bit[46]&~bit[45]&~bit[44]) | (bit[48]&~bit[47]&~bit[46]&~bit[45]&bit[44]) | (~bit[48]&bit[47]&~bit[45]&bit[44]) | (bit[47]&~bit[46]&bit[45]&bit[44]) | (bit[47]&~bit[46]&~bit[45]&~bit[44]) | (~bit[48]&~bit[46]&bit[44]) | (~bit[47]&~bit[44]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[43]);
		control=control & bit[43];

		//-------------44--------------

		out[5] = (~disable_update & ((~bit[49]&bit[48]&bit[47]&bit[46]&~bit[45]) | (bit[49]&~bit[48]&~bit[46]&bit[45]) | (~bit[49]&~bit[48]&~bit[46]&bit[45]) | (bit[49]&bit[48]&bit[47]&bit[46]&~bit[45]) | (~bit[47]&~bit[46]&bit[45]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((~bit[49]&bit[48]&bit[47]&bit[46]&~bit[45]) | (~bit[49]&bit[48]&~bit[47]&bit[46]&bit[45]) | (~bit[48]&bit[46]&bit[45]) | (bit[49]&~bit[47]&bit[46]&bit[45]) | (bit[48]&bit[47]&~bit[46]&bit[45]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((~bit[49]&~bit[48]&~bit[47]&bit[45]) | (~bit[48]&~bit[47]&bit[46]&~bit[45]) | (bit[49]&bit[48]&bit[47]&bit[46]&~bit[45]) | (bit[48]&bit[47]&~bit[46]&~bit[45]) | (bit[48]&bit[47]&~bit[46]&bit[45]) | (bit[48]&bit[47]&bit[46]&bit[45]) | (bit[48]&~bit[47]&~bit[46]&~bit[45]) | (~bit[48]&~bit[46]&~bit[45]) | (~bit[47]&~bit[46]&bit[45]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((bit[49]&~bit[48]&~bit[47]&bit[46]&bit[45]) | (~bit[49]&bit[47]&~bit[46]&bit[45]) | (~bit[49]&bit[48]&~bit[47]&bit[46]&bit[45]) | (~bit[49]&~bit[48]&~bit[46]&bit[45]) | (bit[48]&~bit[47]&bit[46]&~bit[45]) | (bit[49]&bit[48]&bit[47]&bit[46]&~bit[45]) | (bit[48]&bit[47]&bit[46]&bit[45]) | (bit[48]&~bit[47]&~bit[46]&~bit[45]) | (~bit[48]&~bit[46]&~bit[45]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((~bit[48]&~bit[47]&~bit[46]&~bit[45]) | (bit[49]&~bit[48]&~bit[46]&bit[45]) | (~bit[48]&bit[47]&bit[46]&~bit[45]) | (bit[48]&~bit[47]&bit[46]&~bit[45]) | (bit[49]&bit[48]&bit[47]&bit[46]&~bit[45]) | (bit[49]&~bit[47]&bit[46]&bit[45]) | (bit[48]&bit[47]&~bit[46]&~bit[45]) | (bit[48]&bit[47]&bit[46]&bit[45]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((bit[49]&~bit[48]&~bit[47]&bit[45]) | (~bit[49]&bit[48]&~bit[46]) | (~bit[48]&bit[47]&bit[46]&~bit[45]) | (~bit[48]&~bit[47]&bit[46]&~bit[45]) | (bit[49]&bit[48]&bit[47]&bit[46]&~bit[45]) | (~bit[49]&bit[47]&bit[45]) | (bit[49]&~bit[47]&bit[46]&bit[45]) | (bit[48]&bit[47]&~bit[46]&~bit[45]) | (bit[48]&bit[47]&~bit[46]&bit[45]) | (bit[48]&~bit[47]&~bit[46]&~bit[45]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[44]);
		control=control & bit[44];

		//-------------45--------------

		out[5] = (~disable_update & ((bit[49]&~bit[48]&bit[47]&~bit[46]) | (~bit[48]&~bit[47]&bit[46]) | (bit[48]&bit[47]&~bit[46]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((bit[49]&~bit[48]&bit[47]&~bit[46]) | (~bit[48]&bit[47]&bit[46]) | (bit[48]&~bit[47]&bit[46]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((bit[50]&bit[49]&bit[48]&bit[47]) | (~bit[49]&~bit[48]&~bit[47]&~bit[46]) | (bit[49]&~bit[48]&~bit[47]&~bit[46]) | (~bit[49]&bit[48]&~bit[47]&~bit[46]) | (bit[48]&bit[47]&~bit[46]) | (bit[48]&~bit[47]&bit[46]) | (~bit[50]&bit[48]&bit[47]) | (~bit[49]&bit[48]&bit[46]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((~bit[49]&~bit[48]&~bit[47]&~bit[46]) | (~bit[50]&~bit[49]&~bit[48]&bit[46]) | (bit[49]&bit[48]&~bit[47]&~bit[46]) | (~bit[49]&bit[47]&bit[46]) | (~bit[50]&bit[48]&bit[47]) | (~bit[49]&bit[48]&bit[47]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((bit[50]&bit[49]&bit[47]&bit[46]) | (~bit[50]&bit[49]&~bit[48]&bit[47]) | (bit[49]&~bit[48]&~bit[47]&~bit[46]) | (~bit[50]&~bit[49]&~bit[48]&bit[46]) | (~bit[49]&~bit[48]&~bit[47]&bit[46]) | (bit[49]&bit[48]&~bit[47]&~bit[46]) | (~bit[49]&bit[48]&bit[47]) | (~bit[49]&bit[48]&bit[46]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((bit[50]&bit[49]&bit[48]&bit[47]) | (bit[50]&bit[49]&bit[47]&~bit[46]) | (~bit[50]&~bit[49]&bit[48]) | (~bit[50]&~bit[48]&bit[46]) | (~bit[49]&bit[48]&~bit[47]&~bit[46]) | (~bit[49]&~bit[48]&~bit[47]&bit[46]) | (bit[49]&bit[48]&~bit[47]&~bit[46]) | (~bit[50]&~bit[47]&bit[46]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[45]);
		control=control & bit[45];

		//-------------46--------------

		out[5] = (~disable_update & ((~bit[51]&~bit[50]&~bit[49]&bit[48]&bit[47]) | (~bit[51]&~bit[50]&bit[49]&~bit[48]&bit[47]) | (bit[51]&bit[49]&~bit[48]&bit[47]) | (~bit[51]&bit[50]&~bit[48]&bit[47]) | (bit[50]&~bit[49]&~bit[48]&bit[47]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((~bit[51]&bit[50]&~bit[49]&bit[48]&bit[47]) | (bit[51]&~bit[49]&bit[48]&bit[47]) | (bit[49]&bit[48]&bit[47]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((~bit[51]&~bit[50]&bit[49]&~bit[48]&bit[47]) | (~bit[50]&~bit[49]&~bit[48]&~bit[47]) | (~bit[51]&bit[50]&~bit[49]&bit[48]&bit[47]) | (bit[50]&~bit[49]&~bit[48]) | (bit[51]&~bit[49]&bit[48]&bit[47]) | (bit[49]&bit[48]&bit[47]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((bit[51]&~bit[50]&bit[49]&~bit[48]) | (~bit[51]&~bit[50]&bit[49]&bit[48]&bit[47]) | (~bit[50]&~bit[49]&bit[48]&~bit[47]) | (~bit[51]&bit[50]&~bit[49]&bit[48]&bit[47]) | (bit[49]&~bit[48]&~bit[47]) | (~bit[51]&bit[50]&~bit[48]&bit[47]) | (bit[50]&~bit[49]&~bit[48]&bit[47]) | (bit[51]&~bit[49]&bit[48]&bit[47]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((bit[50]&~bit[49]&bit[48]&~bit[47]) | (~bit[50]&~bit[49]&~bit[48]&~bit[47]) | (bit[51]&~bit[50]&bit[48]&bit[47]) | (~bit[51]&bit[50]&~bit[49]&bit[48]&bit[47]) | (~bit[50]&bit[49]&~bit[47]) | (bit[51]&bit[49]&~bit[48]&bit[47]) | (bit[50]&~bit[49]&~bit[48]&bit[47]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((~bit[51]&bit[50]&bit[49]) | (bit[51]&~bit[50]&bit[48]&bit[47]) | (bit[51]&bit[49]&~bit[48]&bit[47]) | (~bit[51]&bit[50]&~bit[48]&bit[47]) | (bit[51]&~bit[49]&bit[48]&bit[47]) | (bit[50]&~bit[47]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[46]);
		control=control & bit[46];

		//-------------47--------------

		out[5] = (~disable_update & ((~bit[52]&~bit[51]&bit[50]&bit[49]&bit[48]) | (bit[52]&bit[50]&bit[49]&bit[48]) | (~bit[51]&~bit[50]&bit[49]&bit[48]) | (bit[51]&bit[49]&bit[48]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((~bit[51]&~bit[50]&bit[49]&~bit[48]) | (~bit[50]&~bit[49]&~bit[48]) | (bit[50]&~bit[49]&~bit[48]) | (~bit[51]&~bit[50]&bit[49]&bit[48]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((bit[52]&~bit[51]&bit[50]&bit[49]&bit[48]) | (~bit[52]&~bit[51]&bit[50]&bit[49]&bit[48]) | (~bit[51]&~bit[50]&~bit[49]&bit[48]) | (~bit[51]&bit[50]&bit[49]&~bit[48]) | (bit[51]&~bit[50]&bit[49]) | (bit[51]&bit[49]&~bit[48]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((~bit[52]&~bit[51]&bit[50]&bit[49]&bit[48]) | (~bit[51]&bit[50]&bit[49]&~bit[48]) | (~bit[50]&~bit[49]&~bit[48]) | (bit[51]&bit[49]&bit[48]) | (bit[51]&~bit[50]&~bit[48]) | (bit[51]&~bit[50]&bit[48]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((bit[52]&~bit[51]&bit[50]&bit[49]&bit[48]) | (~bit[52]&bit[51]&bit[49]) | (~bit[51]&bit[50]&~bit[49]) | (~bit[51]&~bit[49]&~bit[48]) | (bit[50]&~bit[49]&~bit[48]) | (~bit[51]&~bit[50]&bit[49]&bit[48]) | (bit[51]&~bit[50]&bit[48]) | (bit[51]&bit[49]&~bit[48]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((bit[52]&bit[50]&bit[49]&bit[48]) | (~bit[51]&~bit[50]&~bit[49]&bit[48]) | (~bit[51]&~bit[50]&bit[49]&~bit[48]) | (~bit[52]&~bit[50]&bit[49]) | (~bit[51]&bit[50]&~bit[48]) | (bit[51]&~bit[50]&~bit[48]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[47]);
		control=control & bit[47];

		//-------------48--------------

		out[5] = (~disable_update & ((~bit[53]&bit[52]&bit[51]&~bit[50]&bit[49]) | (~bit[53]&~bit[52]&~bit[51]&bit[50]&bit[49]) | (~bit[53]&~bit[52]&bit[51]&bit[50]&bit[49]) | (~bit[53]&bit[52]&~bit[51]&bit[50]&bit[49]) | (bit[53]&~bit[52]&bit[50]&bit[49]) | (bit[53]&bit[52]&bit[51]&~bit[50]&bit[49]) | (~bit[52]&~bit[51]&~bit[50]&~bit[49]) | (bit[53]&~bit[51]&bit[50]&bit[49]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((~bit[53]&bit[52]&bit[51]&~bit[50]&bit[49]) | (~bit[53]&bit[52]&bit[51]&bit[50]&bit[49]) | (bit[52]&~bit[51]&~bit[50]&~bit[49]) | (bit[53]&bit[52]&bit[51]&bit[50]&bit[49]) | (bit[51]&~bit[50]&~bit[49]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((~bit[53]&~bit[52]&~bit[51]&bit[50]&bit[49]) | (~bit[53]&bit[52]&bit[51]&bit[50]&bit[49]) | (~bit[52]&bit[50]&~bit[49]) | (bit[53]&bit[52]&bit[51]&~bit[50]&bit[49]) | (bit[52]&~bit[51]&~bit[49]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((~bit[52]&~bit[51]&bit[50]&~bit[49]) | (~bit[52]&~bit[51]&~bit[50]&bit[49]) | (bit[52]&~bit[51]&~bit[50]&~bit[49]) | (~bit[53]&bit[52]&~bit[51]&bit[50]&bit[49]) | (~bit[52]&bit[51]&~bit[50]&~bit[49]) | (bit[53]&bit[52]&bit[51]&~bit[50]&bit[49]) | (bit[53]&bit[52]&bit[51]&bit[50]&bit[49]) | (bit[53]&~bit[51]&bit[50]&bit[49]) | (bit[52]&bit[51]&bit[50]&~bit[49]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((~bit[53]&bit[52]&~bit[51]&bit[50]&bit[49]) | (bit[53]&~bit[52]&bit[50]&bit[49]) | (~bit[52]&bit[51]&~bit[50]&~bit[49]) | (bit[53]&bit[52]&bit[51]&~bit[50]&bit[49]) | (bit[52]&~bit[51]&~bit[50]&bit[49]) | (bit[53]&bit[52]&bit[51]&bit[50]&bit[49]) | (~bit[52]&bit[51]&bit[49]) | (~bit[52]&~bit[51]&~bit[50]&~bit[49]) | (bit[52]&bit[51]&bit[50]&~bit[49]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((~bit[53]&~bit[52]&bit[51]&bit[50]&bit[49]) | (~bit[53]&bit[52]&bit[51]&bit[50]&bit[49]) | (bit[52]&~bit[51]&~bit[50]&bit[49]) | (bit[53]&bit[52]&bit[51]&bit[50]&bit[49]) | (bit[51]&~bit[50]&~bit[49]) | (bit[52]&~bit[51]&~bit[49]) | (~bit[52]&~bit[51]&~bit[50]&~bit[49]) | (bit[53]&~bit[51]&bit[50]&bit[49]) | (bit[52]&bit[51]&bit[50]&~bit[49]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[48]);
		control=control & bit[48];

		//-------------49--------------

		out[5] = (~disable_update & ((~bit[54]&bit[53]&bit[52]&bit[51]&~bit[50]) | (bit[54]&bit[53]&bit[52]&bit[51]&~bit[50]) | (bit[54]&~bit[53]&~bit[52]&~bit[51]&bit[50]) | (bit[54]&~bit[53]&bit[52]&~bit[51]&bit[50]) | (~bit[54]&~bit[52]&~bit[51]&bit[50]) | (~bit[54]&bit[52]&~bit[51]&bit[50]) | (bit[54]&bit[53]&~bit[52]&~bit[51]&bit[50]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((~bit[54]&bit[53]&bit[52]&bit[51]&~bit[50]) | (bit[54]&bit[53]&~bit[52]&bit[51]&bit[50]) | (bit[54]&bit[53]&bit[52]&bit[51]&~bit[50]) | (bit[54]&~bit[53]&~bit[52]&~bit[51]&bit[50]) | (~bit[52]&~bit[51]&~bit[50]) | (~bit[54]&~bit[52]&~bit[51]&bit[50]) | (~bit[54]&~bit[52]&bit[51]&bit[50]) | (~bit[54]&~bit[53]&bit[52]&bit[51]&bit[50]) | (bit[54]&bit[53]&bit[52]&~bit[51]&bit[50]) | (bit[54]&~bit[53]&~bit[52]&bit[51]&bit[50]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((~bit[54]&~bit[53]&bit[52]&~bit[51]) | (~bit[54]&bit[53]&bit[52]&bit[51]&bit[50]) | (bit[52]&~bit[51]&~bit[50]) | (bit[54]&bit[52]&bit[51]&bit[50]) | (~bit[54]&~bit[52]&bit[51]&bit[50]) | (bit[54]&bit[53]&bit[52]&~bit[51]&bit[50]) | (bit[54]&~bit[53]&~bit[52]&bit[51]&bit[50]) | (bit[54]&bit[53]&~bit[52]&~bit[51]&bit[50]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((~bit[54]&~bit[53]&~bit[52]&bit[51]) | (~bit[54]&bit[53]&bit[52]&bit[51]&~bit[50]) | (bit[54]&~bit[53]&bit[52]&bit[51]&bit[50]) | (bit[54]&bit[53]&~bit[52]&bit[51]&bit[50]) | (~bit[53]&~bit[52]&~bit[50]) | (bit[54]&bit[53]&bit[52]&~bit[51]&bit[50]) | (bit[54]&bit[53]&~bit[52]&~bit[51]&bit[50]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((~bit[54]&~bit[53]&~bit[51]&bit[50]) | (bit[53]&~bit[52]&bit[51]&~bit[50]) | (~bit[54]&bit[53]&bit[52]&bit[51]&bit[50]) | (bit[54]&~bit[53]&bit[52]&bit[51]&bit[50]) | (~bit[53]&~bit[52]&~bit[51]&~bit[50]) | (bit[54]&bit[53]&bit[52]&bit[51]&~bit[50]) | (bit[54]&~bit[53]&bit[52]&~bit[51]&bit[50]) | (~bit[54]&~bit[53]&bit[52]&bit[51]&bit[50]) | (~bit[54]&bit[52]&~bit[51]&bit[50]) | (bit[54]&bit[53]&bit[52]&~bit[51]&bit[50]) | (bit[54]&~bit[53]&~bit[52]&bit[51]&bit[50]) | (bit[54]&bit[53]&~bit[52]&~bit[51]&bit[50]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((~bit[53]&bit[52]&~bit[51]&~bit[50]) | (bit[54]&bit[53]&~bit[52]&bit[51]&bit[50]) | (bit[54]&bit[53]&bit[52]&bit[51]&~bit[50]) | (bit[54]&~bit[53]&~bit[52]&~bit[51]&bit[50]) | (bit[54]&~bit[53]&bit[52]&~bit[51]&bit[50]) | (bit[54]&bit[52]&bit[51]&bit[50]) | (~bit[54]&~bit[53]&bit[52]&bit[51]&bit[50]) | (bit[54]&~bit[53]&~bit[52]&bit[51]&bit[50]) | (bit[54]&bit[53]&~bit[52]&~bit[51]&bit[50]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[49]);
		control=control & bit[49];

		//-------------50--------------

		out[5] = (~disable_update & ((~bit[55]&~bit[53]&bit[52]&~bit[51]) | (~bit[54]&bit[53]&bit[52]&~bit[51]) | (~bit[54]&~bit[53]&bit[52]&~bit[51]) | (bit[54]&bit[53]&bit[52]&~bit[51]) | (bit[55]&bit[54]&~bit[53]&bit[52]&~bit[51]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((bit[55]&~bit[53]&~bit[52]&bit[51]) | (~bit[54]&bit[53]&~bit[52]&bit[51]) | (~bit[55]&~bit[53]&bit[52]&~bit[51]) | (~bit[55]&bit[54]&~bit[52]&bit[51]) | (~bit[54]&~bit[53]&bit[52]&~bit[51]) | (~bit[55]&~bit[53]&~bit[52]&bit[51]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((bit[55]&bit[54]&bit[53]&~bit[52]&bit[51]) | (~bit[54]&bit[53]&bit[52]&~bit[51]) | (bit[55]&bit[54]&~bit[53]&bit[52]&~bit[51]) | (~bit[54]&~bit[53]&~bit[52]) | (~bit[55]&~bit[53]&~bit[52]&bit[51]) | (~bit[53]&bit[52]&bit[51]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((bit[55]&bit[54]&bit[53]&~bit[52]&bit[51]) | (~bit[54]&bit[53]&~bit[52]&~bit[51]) | (~bit[55]&bit[54]&bit[53]&bit[52]) | (bit[55]&~bit[53]&~bit[52]&bit[51]) | (bit[54]&~bit[53]&~bit[52]&~bit[51]) | (bit[55]&bit[54]&~bit[53]&bit[52]&~bit[51]) | (~bit[55]&~bit[54]&bit[51]) | (bit[53]&bit[52]&bit[51]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((~bit[55]&~bit[53]&bit[52]&bit[51]) | (bit[55]&bit[54]&~bit[52]&bit[51]) | (~bit[54]&bit[53]&~bit[52]&bit[51]) | (bit[55]&bit[54]&~bit[53]&bit[52]&~bit[51]) | (bit[54]&bit[53]&~bit[51]) | (~bit[54]&bit[52]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((bit[55]&~bit[54]&~bit[53]&bit[52]&bit[51]) | (~bit[55]&~bit[54]&bit[52]&~bit[51]) | (bit[55]&bit[54]&~bit[52]&bit[51]) | (~bit[55]&bit[54]&~bit[52]&bit[51]) | (bit[54]&bit[53]&bit[52]&~bit[51]) | (bit[54]&~bit[53]&~bit[52]&~bit[51]) | (bit[55]&bit[54]&~bit[53]&bit[52]&~bit[51]) | (~bit[55]&~bit[53]&~bit[52]&bit[51]) | (~bit[55]&bit[53]&bit[52]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[50]);
		control=control & bit[50];

		//-------------51--------------


		out[5] = (~disable_update & ((bit[56]&~bit[55]&~bit[54]&~bit[53]&bit[52]) | (bit[55]&bit[54]&~bit[52]) | (~bit[56]&~bit[54]&~bit[53]&bit[52]) | (bit[53]&~bit[52]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((~bit[56]&~bit[54]&bit[53]&bit[52]) | (bit[56]&bit[55]&~bit[53]&bit[52]) | (~bit[55]&bit[54]&~bit[53]&bit[52]) | (bit[55]&bit[54]&~bit[53]) | (~bit[55]&~bit[54]&bit[53]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((~bit[55]&bit[54]&~bit[53]&bit[52]) | (bit[56]&bit[55]&~bit[54]&bit[52]) | (bit[55]&~bit[54]&bit[53]&~bit[52]) | (~bit[55]&bit[54]&bit[53]) | (bit[54]&bit[53]&bit[52]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((~bit[56]&~bit[55]&~bit[54]&~bit[53]&bit[52]) | (~bit[56]&~bit[55]&bit[54]&bit[53]&bit[52]) | (bit[55]&bit[54]&bit[53]&~bit[52]) | (bit[56]&bit[55]&~bit[54]&bit[52]) | (bit[55]&~bit[54]&bit[53]&~bit[52]) | (bit[55]&bit[54]&~bit[53]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((bit[56]&~bit[55]&~bit[54]&~bit[53]&bit[52]) | (bit[56]&~bit[55]&bit[53]&bit[52]) | (~bit[56]&bit[55]&bit[54]&bit[53]) | (bit[55]&bit[54]&bit[53]&~bit[52]) | (bit[56]&bit[55]&~bit[54]&bit[52]) | (~bit[56]&bit[54]&~bit[53]&bit[52]) | (~bit[54]&~bit[53]&~bit[52]) | (~bit[56]&bit[53]&~bit[52]) | (~bit[55]&~bit[54]&bit[53]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((~bit[56]&bit[55]&bit[54]&~bit[52]) | (bit[56]&bit[55]&bit[54]&bit[52]) | (bit[56]&~bit[55]&bit[54]&bit[53]) | (bit[56]&bit[55]&~bit[53]&bit[52]) | (bit[55]&~bit[54]&bit[53]&~bit[52]) | (~bit[56]&bit[54]&~bit[53]&bit[52]) | (~bit[56]&~bit[55]&~bit[54]) | (~bit[55]&~bit[54]&~bit[53]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[51]);
		control=control & bit[51];

		//-------------52--------------

		out[5] = (~disable_update & ((~bit[57]&~bit[56]&bit[55]&bit[54]&~bit[53]) | (~bit[57]&~bit[56]&bit[55]&~bit[54]&bit[53]) | (bit[57]&bit[56]&~bit[55]&bit[54]&~bit[53]) | (~bit[57]&bit[56]&~bit[55]&bit[54]&~bit[53]) | (bit[57]&~bit[56]&bit[55]&bit[54]&~bit[53]) | (~bit[55]&~bit[54]&bit[53]) | (bit[56]&bit[55]&bit[54]&~bit[53]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((~bit[57]&~bit[56]&bit[55]&bit[54]&~bit[53]) | (bit[57]&bit[56]&~bit[55]&bit[54]&~bit[53]) | (~bit[57]&bit[56]&~bit[55]&bit[54]&~bit[53]) | (~bit[57]&bit[56]&bit[55]&~bit[54]&bit[53]) | (bit[57]&bit[55]&~bit[54]&bit[53]) | (~bit[55]&bit[54]&bit[53]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((~bit[56]&~bit[55]&~bit[54]&~bit[53]) | (~bit[57]&~bit[56]&~bit[55]&bit[53]) | (~bit[57]&bit[56]&bit[55]&~bit[54]&bit[53]) | (bit[57]&bit[55]&~bit[54]&bit[53]) | (bit[57]&~bit[56]&bit[55]&bit[54]&~bit[53]) | (~bit[57]&bit[55]&bit[54]&bit[53]) | (bit[56]&bit[55]&bit[54]&~bit[53]) | (~bit[56]&bit[55]&bit[54]&bit[53]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((~bit[57]&bit[56]&~bit[55]&~bit[54]&bit[53]) | (~bit[57]&bit[56]&~bit[55]&bit[54]&~bit[53]) | (bit[57]&bit[56]&bit[55]&bit[54]&bit[53]) | (bit[56]&~bit[55]&~bit[54]&~bit[53]) | (~bit[56]&bit[55]&~bit[54]&~bit[53]) | (bit[57]&~bit[56]&bit[55]&bit[54]&~bit[53]) | (bit[57]&~bit[56]&~bit[54]&bit[53]) | (~bit[57]&bit[55]&bit[54]&bit[53]) | (bit[56]&bit[55]&bit[54]&~bit[53]) | (~bit[56]&bit[55]&bit[54]&bit[53]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((~bit[57]&bit[56]&~bit[55]&bit[54]&bit[53]) | (~bit[57]&~bit[56]&bit[55]&bit[54]&bit[53]) | (bit[57]&~bit[56]&~bit[55]&bit[54]&bit[53]) | (bit[57]&bit[56]&~bit[55]&bit[54]&~bit[53]) | (bit[57]&bit[56]&bit[55]&bit[54]&bit[53]) | (bit[57]&~bit[55]&~bit[54]&bit[53]) | (~bit[57]&bit[56]&bit[55]&~bit[54]&bit[53]) | (bit[57]&~bit[56]&bit[55]&bit[54]&~bit[53]) | (bit[57]&~bit[56]&~bit[54]&bit[53]) | (bit[56]&~bit[54]&~bit[53]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((~bit[57]&bit[56]&bit[55]&bit[54]&~bit[53]) | (bit[57]&bit[56]&bit[55]&~bit[54]&bit[53]) | (~bit[57]&~bit[56]&bit[55]&~bit[54]&bit[53]) | (~bit[57]&bit[56]&~bit[55]&~bit[54]&bit[53]) | (bit[57]&~bit[56]&~bit[55]&bit[54]&bit[53]) | (~bit[56]&~bit[55]&bit[54]&~bit[53]) | (~bit[56]&bit[55]&bit[54]&bit[53]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[52]);
		control=control & bit[52];

		//-------------53--------------

		out[5] = (~disable_update & ((~bit[58]&~bit[57]&~bit[56]&~bit[55]&bit[54]) | (bit[57]&bit[56]&~bit[55]&~bit[54]) | (bit[55]&~bit[54]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((~bit[58]&~bit[57]&~bit[56]&bit[55]&~bit[54]) | (bit[58]&~bit[57]&~bit[56]&bit[55]) | (bit[58]&~bit[56]&~bit[55]&bit[54]) | (~bit[57]&bit[56]&~bit[55]&bit[54]) | (~bit[58]&bit[57]&~bit[56]&bit[55]) | (bit[58]&bit[56]&~bit[55]&bit[54]) | (bit[57]&bit[56]&~bit[55]&~bit[54]) | (~bit[56]&bit[55]&bit[54]) | (~bit[58]&bit[57]&~bit[55]&bit[54]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((bit[58]&bit[57]&~bit[56]&bit[55]&~bit[54]) | (bit[58]&~bit[57]&~bit[56]&~bit[55]&bit[54]) | (~bit[57]&bit[56]&bit[55]&~bit[54]) | (~bit[58]&~bit[57]&bit[56]&bit[55]) | (~bit[57]&bit[56]&~bit[55]&bit[54]) | (bit[57]&~bit[56]&~bit[55]&bit[54]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((bit[58]&bit[57]&~bit[56]&bit[55]&~bit[54]) | (bit[58]&~bit[57]&~bit[56]&~bit[55]&bit[54]) | (~bit[58]&~bit[57]&~bit[56]&bit[55]&~bit[54]) | (~bit[58]&bit[57]&bit[56]&bit[55]&~bit[54]) | (bit[58]&bit[57]&bit[56]&~bit[55]) | (~bit[57]&~bit[56]&~bit[55]&~bit[54]) | (bit[57]&bit[56]&~bit[55]&~bit[54]) | (~bit[58]&bit[57]&~bit[55]&bit[54]) | (bit[56]&bit[55]&bit[54]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((bit[58]&bit[57]&bit[56]&bit[55]&~bit[54]) | (~bit[58]&~bit[57]&~bit[56]&bit[55]&bit[54]) | (bit[58]&~bit[57]&bit[56]&bit[55]&bit[54]) | (~bit[58]&bit[57]&bit[56]&bit[54]) | (~bit[57]&bit[56]&~bit[55]&~bit[54]) | (bit[58]&~bit[57]&~bit[56]&bit[55]) | (bit[58]&~bit[56]&~bit[55]&bit[54]) | (bit[57]&~bit[56]&~bit[55]&~bit[54]) | (~bit[58]&bit[56]&~bit[55]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((~bit[58]&bit[57]&bit[56]&bit[55]&~bit[54]) | (~bit[58]&~bit[57]&~bit[56]&bit[55]&bit[54]) | (bit[58]&~bit[57]&bit[56]&bit[55]&bit[54]) | (~bit[58]&~bit[57]&bit[56]&bit[55]) | (~bit[58]&bit[57]&~bit[56]&bit[55]) | (bit[57]&~bit[56]&~bit[55]&bit[54]) | (bit[58]&bit[57]&bit[56]&~bit[55]) | (bit[57]&~bit[56]&~bit[55]&~bit[54]) | (bit[58]&bit[56]&~bit[55]&bit[54]) | (~bit[57]&~bit[56]&~bit[55]&~bit[54]) | (~bit[58]&bit[57]&~bit[55]&bit[54]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[53]);
		control=control & bit[53];

		//-------------54--------------

		out[5] = (~disable_update & ((bit[59]&bit[58]&bit[57]&bit[56]&bit[55]) | (bit[57]&~bit[56]&~bit[55]) | (bit[56]&~bit[55]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((bit[59]&bit[58]&bit[57]&bit[56]&bit[55]) | (bit[57]&~bit[56]&~bit[55]) | (~bit[56]&bit[55]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((bit[59]&~bit[58]&~bit[57]&bit[56]&~bit[55]) | (~bit[58]&~bit[57]&bit[55]) | (~bit[57]&bit[56]&bit[55]) | (~bit[59]&~bit[57]&~bit[56]&bit[55]) | (~bit[59]&~bit[57]&bit[56]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((bit[59]&bit[58]&bit[57]&bit[56]&bit[55]) | (bit[59]&bit[58]&~bit[57]&bit[56]&~bit[55]) | (bit[59]&~bit[57]&~bit[56]&bit[55]) | (~bit[58]&bit[57]&~bit[55]) | (~bit[58]&~bit[57]&bit[56]&bit[55]) | (~bit[59]&~bit[58]&bit[56]) | (~bit[58]&~bit[56]&bit[55]) | (~bit[59]&~bit[57]&~bit[56]&bit[55]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((bit[59]&~bit[58]&~bit[57]&bit[56]&~bit[55]) | (~bit[59]&bit[58]&bit[57]) | (bit[59]&bit[58]&bit[57]&bit[56]&bit[55]) | (bit[59]&bit[58]&~bit[57]&bit[56]&~bit[55]) | (~bit[59]&~bit[58]&~bit[57]&bit[55]) | (bit[59]&~bit[57]&~bit[56]&bit[55]) | (~bit[58]&~bit[57]&~bit[56]&~bit[55]) | (~bit[58]&bit[57]&bit[56]&bit[55]) | (~bit[59]&bit[57]&bit[56]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((~bit[60]&bit[59]&bit[57]&bit[56]&bit[55]) | (~bit[59]&bit[58]&bit[57]&~bit[55]) | (bit[59]&bit[58]&~bit[57]&bit[56]&~bit[55]) | (~bit[58]&~bit[57]&~bit[56]&~bit[55]) | (~bit[59]&bit[57]&~bit[56]) | (~bit[58]&~bit[57]&bit[56]&bit[55]) | (~bit[58]&bit[57]&bit[56]&bit[55]) | (~bit[59]&~bit[57]&~bit[56]&bit[55]) | (~bit[59]&~bit[57]&bit[56]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[54]);
		control=control & bit[54];

		//-------------55--------------

		out[5] = (~disable_update & ((bit[59]&~bit[58]&bit[57]&~bit[56]) | (~bit[60]&bit[59]&~bit[58]&~bit[56]) | (bit[60]&bit[59]&~bit[57]&~bit[56]) | (bit[59]&bit[58]&~bit[57]&~bit[56]) | (~bit[59]&~bit[56]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((~bit[60]&~bit[59]&~bit[57]) | (bit[59]&bit[58]&bit[57]&~bit[56]) | (~bit[57]&bit[56]) | (~bit[58]&~bit[57]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((~bit[60]&~bit[59]&~bit[58]&bit[57]) | (bit[60]&bit[58]&~bit[57]&~bit[56]) | (bit[59]&bit[58]&~bit[57]&~bit[56]) | (bit[59]&bit[58]&bit[57]&~bit[56]) | (~bit[58]&bit[56]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((bit[60]&~bit[59]&~bit[58]&bit[57]&~bit[56]) | (bit[60]&~bit[59]&~bit[57]) | (~bit[60]&bit[59]&bit[58]&~bit[57]) | (bit[59]&~bit[58]&bit[57]&~bit[56]) | (~bit[60]&~bit[59]&~bit[58]&~bit[57]) | (bit[59]&bit[58]&bit[57]&~bit[56]) | (~bit[59]&bit[56]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((~bit[59]&bit[58]&~bit[57]&bit[56]) | (bit[60]&~bit[59]&~bit[58]&bit[57]&~bit[56]) | (~bit[60]&bit[59]&~bit[58]&~bit[56]) | (bit[59]&~bit[58]&bit[56]) | (~bit[60]&bit[58]&bit[57]) | (bit[60]&bit[58]&~bit[57]) | (bit[59]&bit[58]&bit[57]&~bit[56]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((~bit[60]&bit[59]&bit[58]&bit[57]) | (bit[60]&~bit[59]&~bit[58]&bit[57]&~bit[56]) | (bit[60]&~bit[59]&bit[58]&~bit[56]) | (bit[60]&bit[59]&~bit[57]&~bit[56]) | (~bit[60]&~bit[59]&~bit[58]&~bit[57]) | (bit[58]&bit[57]&bit[56]) | (~bit[60]&bit[56]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[55]);
		control=control & bit[55];

		//-------------56--------------

		out[5] = (~disable_update & ((bit[60]&~bit[59]&bit[58]&~bit[57]) | (bit[60]&~bit[59]&~bit[58]&~bit[57]) | (bit[59]&~bit[58]&~bit[57]) | (~bit[60]&~bit[57]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((bit[60]&bit[59]&bit[58]&~bit[57]) | (bit[60]&~bit[59]&~bit[58]&~bit[57]) | (~bit[60]&~bit[58]&bit[57]) | (~bit[60]&~bit[59]&~bit[58]) | (~bit[59]&~bit[58]&bit[57]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((bit[60]&bit[59]&~bit[58]&bit[57]) | (bit[60]&bit[59]&bit[58]&~bit[57]) | (~bit[59]&bit[58]&bit[57]) | (~bit[60]&~bit[59]&bit[58]) | (bit[59]&~bit[58]&~bit[57]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((bit[60]&bit[59]&~bit[58]&bit[57]) | (bit[61]&~bit[60]&~bit[58]&~bit[57]) | (~bit[60]&bit[59]&bit[58]&bit[57]) | (bit[60]&bit[59]&bit[58]&~bit[57]) | (bit[60]&~bit[59]&bit[58]&~bit[57]) | (~bit[61]&bit[59]&~bit[57]) | (~bit[60]&~bit[59]&~bit[58]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((bit[61]&~bit[60]&bit[59]&bit[58]&~bit[57]) | (bit[61]&bit[60]&bit[59]&~bit[58]&~bit[57]) | (~bit[61]&bit[60]&~bit[58]&bit[57]) | (~bit[61]&~bit[59]&~bit[58]) | (~bit[61]&bit[59]&bit[58]&bit[57]) | (~bit[61]&bit[60]&bit[58]&~bit[57]) | (~bit[61]&~bit[60]&~bit[58]&~bit[57]) | (~bit[60]&~bit[59]&bit[57]) | (~bit[59]&~bit[58]&bit[57]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((~bit[61]&~bit[59]&bit[58]) | (~bit[61]&~bit[60]&bit[57]) | (bit[61]&~bit[60]&~bit[58]&~bit[57]) | (~bit[60]&bit[59]&bit[58]&bit[57]) | (bit[61]&bit[60]&bit[59]) | (bit[60]&~bit[59]&bit[58]&~bit[57]) | (bit[60]&~bit[59]&~bit[58]&~bit[57]) | (~bit[61]&~bit[59]&bit[57]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[56]);
		control=control & bit[56];

		//-------------57--------------

		out[5] = (~disable_update & ((bit[61]&bit[60]&~bit[59]&~bit[58]) | (bit[61]&~bit[60]&~bit[58]) | (~bit[61]&~bit[58]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((~bit[61]&~bit[59]&~bit[58]) | (bit[61]&bit[60]&bit[59]&~bit[58]) | (~bit[62]&~bit[61]&~bit[59]) | (~bit[60]&~bit[59]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((bit[61]&bit[60]&bit[59]&~bit[58]) | (bit[62]&~bit[61]&~bit[59]&bit[58]) | (~bit[62]&~bit[60]&bit[58]) | (~bit[61]&~bit[60]&bit[59]) | (~bit[62]&~bit[61]&~bit[60]) | (bit[61]&bit[60]&~bit[59]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((bit[62]&bit[60]&~bit[59]&bit[58]) | (~bit[62]&~bit[61]&~bit[60]&~bit[59]&bit[58]) | (~bit[61]&bit[60]&bit[59]&bit[58]) | (bit[62]&bit[61]&~bit[60]&bit[58]) | (bit[61]&bit[60]&bit[59]&~bit[58]) | (bit[62]&~bit[60]&~bit[59]&~bit[58]) | (bit[61]&~bit[60]&~bit[58]) | (bit[61]&bit[60]&~bit[59]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((~bit[62]&~bit[61]&bit[59]&bit[58]) | (bit[62]&~bit[61]&~bit[60]&~bit[59]) | (~bit[62]&bit[60]&~bit[58]) | (bit[61]&bit[60]&~bit[59]&~bit[58]) | (bit[62]&~bit[61]&~bit[59]&bit[58]) | (bit[62]&bit[61]&bit[59]) | (~bit[62]&bit[60]&bit[59]) | (~bit[62]&bit[61]&~bit[58]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((~bit[62]&~bit[60]&bit[59]&~bit[58]) | (bit[62]&~bit[61]&bit[60]
&~bit[58]) | (~bit[61]&~bit[60]&bit[59]&bit[58]) | (bit[62]&bit[61]&~bit[60]&bit[58]) | (bit[62]&~bit[60]&~bit[59]&~bit[58]) | (~bit[62]&bit[61]&bit[60]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[57]);
		control=control & bit[57];

		//-------------58--------------

		out[5] = (~disable_update & ((~bit[63]&bit[61]&~bit[60]&~bit[59]) | (bit[63]&bit[62]&bit[61]&~bit[60]&~bit[59]) | (~bit[62]&bit[61]&~bit[60]&~bit[59]) | (bit[62]&~bit[61]&~bit[60]&~bit[59]) | (bit[60]&~bit[59]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((~bit[63]&bit[61]&~bit[60]&~bit[59]) | (~bit[62]&bit[61]&~bit[60]&~bit[59]) | (bit[62]&~bit[61]&~bit[60]&~bit[59]) | (~bit[60]&bit[59]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((~bit[63]&bit[62]&~bit[61]&bit[60]&~bit[59]) | (bit[63]&bit[62]&bit[61]&~bit[60]&~bit[59]) | (~bit[63]&~bit[61]&~bit[60]&bit[59]) | (~bit[63]&~bit[62]&bit[60]&bit[59]) | (~bit[62]&~bit[61]&bit[59]) | (~bit[62]&~bit[61]&bit[60]) | (~bit[61]&bit[60]&bit[59]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((bit[63]&bit[62]&~bit[61]&bit[60]&~bit[59]) | (~bit[63]&bit[62]&bit[61]&bit[59]) | (bit[63]&bit[62]&bit[61]&~bit[60]&~bit[59]) | (~bit[63]&~bit[62]&bit[60]&~bit[59]) | (bit[63]&bit[62]&~bit[61]&~bit[60]&bit[59]) | (bit[62]&~bit[61]&~bit[60]&~bit[59]) | (bit[63]&~bit[62]&bit[60]&bit[59]) | (~bit[62]&~bit[60]&bit[59]) | (~bit[61]&bit[60]&bit[59]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((bit[63]&bit[62]&bit[61]&bit[60]&bit[59]) | (~bit[63]&bit[62]&~bit[61]&bit[60]&~bit[59]) | (~bit[63]&bit[62]&~bit[61]&~bit[60]&~bit[59]) | (~bit[63]&~bit[62]&bit[61]&~bit[60]) | (bit[63]&bit[62]&bit[61]&~bit[60]&~bit[59]) | (bit[63]&bit[62]&~bit[61]&~bit[60]&bit[59]) | (~bit[62]&~bit[61]&bit[60]&bit[59]) | (~bit[63]&~bit[61]&~bit[60]&bit[59]) | (bit[63]&~bit[62]&bit[60]&~bit[59]) | (bit[63]&~bit[62]&bit[60]&bit[59]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((~bit[63]&bit[62]&bit[61]&bit[60]&~bit[59]) | (bit[63]&bit[62]&bit[61]&bit[60]&bit[59]) | (bit[63]&bit[62]&~bit[61]&bit[60]&~bit[59]) | (~bit[63]&bit[62]&~bit[61]&~bit[60]&~bit[59]) | (bit[63]&~bit[62]&~bit[60]&bit[59]) | (bit[63]&bit[62]&~bit[61]&~bit[60]&bit[59]) | (~bit[63]&~bit[61]&bit[60]&bit[59]) | (~bit[62]&bit[61]&~bit[60]&~bit[59]) | (bit[63]&~bit[62]&bit[60]&~bit[59]) | (~bit[63]&~bit[62]&bit[60]&bit[59]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[58]);
		control=control & bit[58];

		//-------------59--------------


		out[5] = (~disable_update & ((~bit[64]&~bit[63]&~bit[62]&bit[61]&~bit[60]) | (bit[64]&bit[63]&bit[61]&~bit[60]) | (~bit[64]&bit[62]&bit[61]&~bit[60]) | (~bit[63]&bit[62]&~bit[61]&~bit[60]) | (bit[64]&~bit[63]&bit[61]&~bit[60]) | (~bit[62]&~bit[61]&bit[60]) | (bit[63]&~bit[61]&~bit[60]) | (bit[63]&~bit[62]&~bit[60]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((~bit[64]&~bit[63]&~bit[62]&bit[61]&~bit[60]) | (~bit[62]&bit[61]&bit[60]) | (bit[63]&~bit[61]&~bit[60]) | (bit[62]&~bit[61]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((~bit[64]&bit[62]&bit[61]&~bit[60]) | (bit[64]&~bit[63]&bit[61]&~bit[60]) | (bit[63]&~bit[62]&~bit[60]) | (bit[62]&bit[60]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((~bit[64]&bit[63]&~bit[62]&bit[61]&~bit[60]) | (bit[64]&bit[63]&bit[61]&~bit[60]) | (bit[64]&~bit[63]&~bit[62]&bit[61]) | (~bit[64]&~bit[63]&bit[61]&bit[60]) | (~bit[64]&~bit[63]&bit[62]&~bit[61]) | (~bit[63]&bit[62]&~bit[61]&~bit[60]) | (~bit[63]&~bit[62]&bit[60]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((bit[64]&bit[62]&bit[61]&~bit[60]) | (~bit[64]&bit[63]&bit[62]&~bit[61]&~bit[60]) | (~bit[64]&bit[63]&~bit[62]&bit[61]&~bit[60]) | (bit[64]&~bit[63]&bit[62]&bit[60]) | (~bit[64]&~bit[63]&bit[62]&bit[61]) | (bit[64]&~bit[63]&bit[61]&~bit[60]) | (~bit[64]&~bit[62]&bit[60]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((bit[64]&bit[63]&bit[62]&~bit[60]) | (~bit[63]&~bit[62]&~bit[61]&~bit[60]) | (~bit[64]&~bit[62]&~bit[61]&~bit[60]) | (bit[64]&~bit[63]&~bit[62]&bit[61]) | (~bit[64]&~bit[63]&bit[61]&bit[60]) | (~bit[64]&~bit[63]&bit[62]&~bit[61]) | (~bit[64]&~bit[63]&bit[62]&bit[61]) | (~bit[64]&bit[62]&bit[60]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[59]);
		control=control & bit[59];


		//-------------60--------------

		out[5] = (~disable_update & ((bit[64]&~bit[63]&bit[62]&~bit[61]) | (~bit[63]&~bit[62]&bit[61]) | (bit[63]&bit[62]&~bit[61]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((bit[65]&bit[64]&~bit[63]&bit[62]&bit[61]) | (~bit[65]&~bit[64]&bit[63]&bit[62]) | (bit[64]&~bit[63]&bit[62]&~bit[61]) | (bit[65]&~bit[64]&bit[62]&bit[61]) | (~bit[65]&~bit[63]&bit[62]&bit[61]) | (bit[63]&~bit[62]&bit[61]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((~bit[64]&~bit[63]&bit[62]&bit[61]) | (bit[65]&bit[63]&bit[62]&~bit[61]) | (~bit[65]&~bit[64]&~bit[62]&bit[61]) | (~bit[65]&~bit[63]&bit[62]&bit[61]) | (bit[64]&bit[63]&bit[62]) | (bit[63]&~bit[62]&bit[61]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((bit[65]&~bit[64]&bit[63]&~bit[61]) | (bit[65]&bit[64]&~bit[63]&bit[62]&bit[61]) | (bit[64]&bit[63]&bit[62]&bit[61]) | (bit[65]&~bit[64]&~bit[62]&bit[61]) | (~bit[65]&bit[64]&~bit[61]) | (~bit[65]&bit[64]&~bit[62]) | (~bit[65]&bit[63]&bit[61]) | (~bit[62]&~bit[61]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((~bit[65]&bit[64]&bit[63]&bit[62]&bit[61]) | (bit[65]&bit[64]&~bit[63]&bit[62]&bit[61]) | (~bit[65]&~bit[64]&~bit[63]&bit[62]) | (bit[65]&bit[64]&~bit[62]&bit[61]) | (~bit[64]&bit[63]&~bit[62]&bit[61]) | (bit[65]&bit[63]&bit[62]&~bit[61]) | (bit[65]&~bit[64]&~bit[62]&bit[61]) | (~bit[63]&~bit[61]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((bit[65]&bit[64]&~bit[62]&bit[61]) | (~bit[65]&~bit[64]&~bit[62]&bit[61]) | (bit[65]&~bit[64]&bit[62]&bit[61]) | (~bit[64]&~bit[63]&~bit[61]) | (~bit[64]&~bit[62]&~bit[61]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[60]);
		control=control & bit[60];

		//-------------61--------------

		out[5] = (~disable_update & ((~bit[66]&bit[65]&~bit[64]&bit[63]&~bit[62]) | (~bit[66]&~bit[65]&bit[63]&~bit[62]) | (bit[66]&~bit[64]&bit[63]&~bit[62]) | (~bit[64]&~bit[63]&bit[62]) | (bit[64]&bit[63]&~bit[62]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((~bit[66]&bit[65]&~bit[64]&bit[63]&~bit[62]) | (~bit[65]&~bit[64]&bit[63]) | (bit[64]&~bit[63]&bit[62]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((bit[66]&bit[65]&bit[63]&~bit[62]) | (bit[65]&bit[63]&bit[62]) | (~bit[66]&~bit[65]&bit[63]&~bit[62]) | (bit[64]&bit[63]&~bit[62]) | (~bit[65]&bit[64]&bit[62]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((bit[66]&~bit[65]&bit[64]&bit[63]&~bit[62]) | (~bit[66]&~bit[65]&bit[64]&bit[63]) | (bit[65]&~bit[64]&bit[63]&bit[62]) | (bit[66]&~bit[64]&bit[63]&~bit[62]) | (~bit[64]&~bit[63]&bit[62]) | (~bit[65]&~bit[64]&~bit[63]) | (bit[64]&~bit[63]&bit[62]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((bit[66]&~bit[65]&bit[63]&bit[62]) | (~bit[66]&bit[64]&~bit[63]&bit[62]) | (~bit[66]&bit[65]&bit[64]&bit[63]) | (bit[65]&~bit[64]&~bit[63]&~bit[62]) | (bit[65]&~bit[64]&bit[63]&bit[62]) | (~bit[66]&~bit[65]&bit[63]&~bit[62]) | (bit[66]&~bit[64]&bit[63]&~bit[62]) | (~bit[65]&~bit[64]&~bit[63]) | (~bit[65]&~bit[64]&bit[63]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((~bit[66]&bit[65]&~bit[64]&bit[63]&~bit[62]) | (bit[66]&~bit[65]&bit[64]&bit[63]&~bit[62]) | (bit[66]&bit[65]&bit[63]&~bit[62]) | (bit[65]&~bit[64]&~bit[63]&~bit[62]) | (~bit[66]&~bit[64]&bit[62]) | (~bit[65]&bit[64]&~bit[63]) | (~bit[65]&bit[64]&bit[62]) | (bit[64]&~bit[63]&bit[62]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[61]);
		control=control & bit[61];

		//-------------62--------------

		out[5] = (~disable_update & ((~bit[67]&~bit[66]&~bit[65]&~bit[64]&bit[63]) | (bit[67]&bit[66]&bit[65]&~bit[64]&~bit[63]) | (~bit[67]&bit[66]&bit[65]&~bit[64]&~bit[63]) | (bit[65]&bit[64]&~bit[63]) | (~bit[65]&bit[64]&~bit[63]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((bit[67]&bit[66]&bit[65]&~bit[64]&~bit[63]) | (~bit[67]&bit[66]&~bit[64]&bit[63]) | (bit[67]&bit[66]&bit[65]&~bit[64]&bit[63]) | (bit[67]&~bit[65]&~bit[64]&bit[63]) | (~bit[67]&bit[66]&bit[65]&~bit[64]&~bit[63]) | (bit[67]&~bit[66]&~bit[64]&bit[63]) | (~bit[66]&~bit[65]&bit[64]) | (~bit[67]&bit[65]&~bit[64]&bit[63]) | (~bit[65]&bit[64]&~bit[63]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((bit[67]&~bit[65]&~bit[64]&bit[63]) | (~bit[67]&bit[66]&bit[65]&~bit[64]&~bit[63]) | (~bit[66]&bit[65]&bit[64]) | (bit[66]&~bit[65]&bit[63]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((bit[67]&bit[66]&bit[65]&~bit[64]&~bit[63]) | (bit[67]&~bit[66]&~bit[64]&bit[63]) | (~bit[66]&~bit[65]&~bit[64]&~bit[63]) | (bit[66]&bit[64]&bit[63]) | (bit[65]&bit[64]&~bit[63]) | (~bit[67]&bit[65]&~bit[64]&bit[63]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((~bit[67]&~bit[66]&~bit[65]&~bit[64]&bit[63]) | (~bit[67]&bit[65]&bit[64]&~bit[63]) | (bit[67]&bit[66]&bit[65]&~bit[64]&bit[63]) | (~bit[66]&bit[65]&~bit[64]&bit[63]) | (~bit[66]&~bit[65]&~bit[64]&~bit[63]) | (~bit[67]&bit[66]&bit[64]&bit[63]) | (~bit[66]&bit[64]&~bit[63]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((~bit[67]&bit[66]&~bit[64]&bit[63]) | (bit[67]&bit[66]&bit[65]&~bit[64]&bit[63]) | (bit[66]&~bit[65]&~bit[64]&~bit[63]) | (~bit[67]&bit[66]&bit[65]&~bit[64]&~bit[63]) | (~bit[67]&bit[66]&bit[64]&bit[63]) | (~bit[67]&~bit[66]&bit[64]) | (~bit[67]&~bit[65]&bit[64]) | (~bit[67]&bit[65]&~bit[64]&bit[63]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[62]);
		control=control & bit[62];

		//-------------63--------------

		out[5] = (~disable_update & ((bit[68]&~bit[67]&~bit[66]&~bit[65]&bit[64]) | (~bit[68]&~bit[66]&~bit[65]&bit[64]) | (bit[66]&~bit[65]&~bit[64]) | (bit[65]&~bit[64]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((bit[68]&bit[67]&~bit[65]&bit[64]) | (~bit[68]&~bit[66]&bit[65]&bit[64]) | (~bit[67]&bit[66]&~bit[65]&bit[64]) | (~bit[68]&bit[67]&bit[66]&~bit[65]) | (bit[66]&~bit[65]&~bit[64]) | (~bit[67]&~bit[66]&bit[65]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((bit[68]&bit[67]&~bit[66]&bit[64]) | (bit[67]&~bit[66]&bit[65]&~bit[64]) | (~bit[68]&bit[67]&bit[66]&bit[65]) | (~bit[68]&~bit[67]&bit[66]) | (bit[66]&bit[65]&bit[64]) | (~bit[67]&bit[66]&bit[65]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((bit[68]&bit[67]&bit[66]&~bit[64]) | (bit[68]&bit[67]&~bit[65]&bit[64]) | (~bit[68]&~bit[67]&~bit[65]&bit[64]) | (bit[67]&~bit[66]&bit[65]&~bit[64]) | (bit[68]&bit[67]&~bit[66]&bit[65]) | (~bit[68]&~bit[67]&bit[66]&bit[65]) | (bit[68]&~bit[67]&bit[66]&~bit[65]) | (~bit[68]&bit[67]&bit[66]&~bit[65]) | (~bit[67]&bit[66]&bit[64]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((bit[68]&~bit[67]&~bit[66]&~bit[65]&bit[64]) | (bit[68]&bit[66]&bit[65]&~bit[64]) | (~bit[67]&~bit[66]&~bit[65]&~bit[64]) | (~bit[68]&~bit[66]&bit[65]&~bit[64]) | (bit[68]&bit[67]&~bit[66]&bit[64]) | (~bit[68]&~bit[67]&bit[65]&bit[64]) | (~bit[67]&~bit[66]&bit[65]&bit[64]) | (bit[68]&~bit[67]&bit[66]&~bit[65]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((~bit[68]&~bit[67]&~bit[66]&bit[65]&bit[64]) | (bit[68]&~bit[67]&~bit[66]&~bit[65]&bit[64]) | (~bit[68]&bit[67]&bit[66]&bit[65]) | (bit[68]&bit[67]&~bit[66]&bit[65]) | (~bit[67]&bit[66]&~bit[65]&bit[64]) | (~bit[68]&bit[67]&bit[66]&~bit[65]) | (bit[67]&~bit[66]&~bit[65]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[63]);
		control=control & bit[63];

		//-------------64--------------

		out[5] = (~disable_update & ((bit[69]&bit[68]&~bit[67]&~bit[66]&bit[65]) | (bit[69]&~bit[68]&~bit[67]&~bit[66]&bit[65]) | (~bit[69]&~bit[68]&~bit[66]&bit[65]) | (~bit[69]&~bit[67]&~bit[66]&bit[65]) | (bit[66]&~bit[65]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((bit[69]&~bit[68]&bit[67]&~bit[66]&bit[65]) | (bit[68]&bit[67]&~bit[66]&bit[65]) | (~bit[69]&~bit[68]&~bit[67]&bit[66]) | (~bit[69]&~bit[68]&bit[67]&bit[66]) | (~bit[68]&bit[66]&~bit[65]) | (bit[69]&~bit[68]&~bit[67]&bit[66]) | (bit[68]&~bit[67]&bit[66]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((bit[69]&~bit[68]&bit[67]&~bit[66]&bit[65]) | (~bit[69]&bit[68]&bit[67]&bit[66]) | (bit[69]&~bit[68]&bit[67]&bit[66]&bit[65]) | (bit[68]&bit[67]&~bit[66]&bit[65]) | (~bit[69]&~bit[68]&~bit[67]&bit[66]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((bit[69]&~bit[68]&bit[67]&~bit[66]&bit[65]) | (bit[69]&~bit[68]&~bit[67]&~bit[66]&bit[65]) | (bit[69]&~bit[68]&bit[67]&bit[66]&bit[65]) | (~bit[69]&~bit[67]&~bit[66]&bit[65]) | (bit[69]&bit[68]&bit[67]&bit[66]) | (bit[68]&bit[67]&bit[66]&~bit[65]) | (~bit[67]&~bit[66]&~bit[65]) | (bit[69]&~bit[68]&~bit[67]&bit[66]) | (bit[68]&~bit[67]&bit[66]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((~bit[69]&bit[68]&bit[67]&~bit[66]&bit[65]) | (bit[69]&bit[68]&~bit[67]&~bit[66]&bit[65]) | (~bit[69]&~bit[68]&~bit[67]&~bit[66]) | (~bit[69]&~bit[67]&bit[66]&~bit[65]) | (bit[69]&bit[68]&bit[67]&bit[66]) | (~bit[69]&~bit[68]&bit[67]&bit[66]) | (~bit[68]&~bit[67]&~bit[65]) | (bit[69]&~bit[68]&~bit[67]&bit[66]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((bit[69]&~bit[68]&~bit[67]&~bit[66]&bit[65]) | (bit[69]&~bit[68]&bit[67]&bit[66]&bit[65]) | (~bit[68]&bit[67]&~bit[66]&~bit[65]) | (~bit[69]&bit[68]&bit[66]&bit[65]) | (~bit[69]&bit[67]&bit[66]&~bit[65]) | (~bit[68]&~bit[67]&bit[66]&~bit[65]) | (bit[68]&bit[67]&~bit[66]&bit[65]) | (bit[68]&bit[67]&bit[66]&~bit[65]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[64]);
		control=control & bit[64];

		//-------------65--------------

		out[5] = (~disable_update & ((~bit[70]&bit[69]&~bit[68]&~bit[67]&bit[66]) | (bit[70]&~bit[69]&bit[68]&bit[67]&~bit[66]) | (~bit[69]&~bit[68]&bit[67]&~bit[66]) | (bit[70]&bit[69]&bit[68]&bit[67]&~bit[66]) | (~bit[69]&~bit[68]&~bit[67]&bit[66]) | (bit[69]&bit[68]&~bit[67]&~bit[66]) | (~bit[70]&bit[67]&~bit[66]) | (bit[69]&~bit[68]&bit[67]&~bit[66]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((bit[70]&bit[69]&~bit[67]&bit[66]) | (bit[70]&~bit[69]&bit[68]&~bit[67]&bit[66]) | (~bit[70]&~bit[68]&bit[67]&bit[66]) | (bit[70]&~bit[69]&~bit[68]&bit[67]) | (~bit[69]&~bit[68]&bit[67]&~bit[66]) | (~bit[70]&bit[68]&~bit[67]&bit[66]) | (bit[69]&bit[68]&~bit[67]&~bit[66]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((bit[70]&~bit[69]&bit[68]&~bit[67]&bit[66]) | (bit[70]&~bit[69]&bit[68]&bit[67]&~bit[66]) | (~bit[70]&bit[68]&~bit[67]&bit[66]) | (~bit[69]&bit[68]&bit[67]&bit[66]) | (bit[69]&bit[68]&~bit[67]&~bit[66]) | (~bit[70]&bit[68]&bit[67]&bit[66]) | (bit[70]&bit[69]&~bit[68]&bit[66]) | (~bit[70]&bit[67]&~bit[66]) | (bit[69]&~bit[68]&bit[67]&~bit[66]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((~bit[70]&~bit[69]&bit[68]&bit[67]&~bit[66]) | (~bit[70]&~bit[69]&~bit[67]&bit[66]) | (bit[70]&bit[69]&bit[68]&bit[66]) | (bit[70]&bit[69]&bit[68]&bit[67]&~bit[66]) | (~bit[69]&~bit[68]&~bit[67]&bit[66]) | (bit[70]&bit[69]&~bit[68]&bit[66]) | (bit[69]&~bit[68]&bit[67]&~bit[66]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((~bit[70]&bit[69]&bit[68]&~bit[67]&~bit[66]) | (~bit[70]&bit[69]&~bit[68]&~bit[67]&bit[66]) | (~bit[69]&~bit[68]&~bit[67]&~bit[66]) | (bit[70]&~bit[69]&bit[68]&~bit[67]&bit[66]) | (~bit[70]&~bit[69]&bit[67]&bit[66]) | (bit[70]&bit[69]&bit[68]&bit[67]&~bit[66]) | (~bit[69]&bit[68]&bit[67]&bit[66]) | (bit[70]&bit[69]&~bit[68]&bit[66]) | (bit[69]&~bit[68]&bit[67]&~bit[66]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((~bit[70]&bit[69]&~bit[68]&~bit[66]) | (bit[70]&~bit[69]&bit[68]&bit[67]&~bit[66]) | (bit[70]&~bit[68]&bit[67]&bit[66]) | (~bit[70]&~bit[68]&~bit[67]&bit[66]) | (bit[70]&~bit[69]&~bit[68]&bit[67]) | (bit[70]&bit[69]&bit[68]&bit[66]) | (bit[70]&bit[69]&bit[68]&bit[67]&~bit[66]) | (bit[69]&~bit[67]&~bit[66]) | (~bit[70]&bit[68]&bit[67]&bit[66]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[65]);
		control=control & bit[65];

		//-------------66--------------

		out[5] = (~disable_update & ((bit[71]&~bit[69]&bit[68]&~bit[67]) | (~bit[70]&bit[69]&~bit[67]) | (~bit[71]&bit[68]&~bit[67]) | (bit[70]&~bit[68]&~bit[67]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((bit[71]&bit[70]&bit[69]&bit[68]&~bit[67]) | (~bit[70]&bit[69]&~bit[68]) | (bit[70]&~bit[68]&bit[67]) | (bit[70]&~bit[69]&~bit[68]) | (~bit[69]&bit[67]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((~bit[71]&bit[70]&~bit[69]&bit[68]&~bit[67]) | (~bit[70]&~bit[69]&bit[68]&~bit[67]) | (bit[71]&bit[70]&bit[69]&bit[68]&~bit[67]) | (~bit[71]&bit[69]&bit[68]&bit[67]) | (~bit[70]&~bit[68]&bit[67]) | (~bit[70]&bit[69]&bit[68]&bit[67]) | (bit[70]&~bit[69]&~bit[68]) | (bit[70]&~bit[68]&~bit[67]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((bit[71]&bit[70]&~bit[69]&bit[68]&~bit[67]) | (bit[71]&bit[70]&bit[69]&bit[68]&bit[67]) | (~bit[71]&bit[70]&~bit[68]&~bit[67]) | (~bit[71]&~bit[70]&bit[68]) | (bit[71]&bit[70]&bit[69]&bit[68]&~bit[67]) | (~bit[70]&~bit[69]&bit[67]) | (~bit[70]&bit[69]&bit[68]&bit[67]) | (bit[70]&bit[69]&~bit[68]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((bit[71]&bit[70]&bit[69]&bit[68]&bit[67]) | (bit[71]&~bit[69]&bit[68]&~bit[67]) | (bit[71]&bit[70]&bit[69]&bit[68]&~bit[67]) | (~bit[71]&bit[69]&bit[68]&bit[67]) | (bit[70]&~bit[68]&bit[67]) | (bit[70]&bit[69]&~bit[68]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((bit[71]&bit[70]&~bit[69]&bit[68]&~bit[67]) | (~bit[71]&bit[70]&~bit[69]&bit[68]&~bit[67]) | (bit[71]&~bit[70]&bit[69]&bit[68]) | (bit[71]&bit[70]&bit[69]&bit[68]&bit[67]) | (~bit[71]&~bit[69]&bit[67]) | (~bit[71]&bit[69]&~bit[68]) | (~bit[70]&bit[69]&bit[68]&bit[67]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[66]);
		control=control & bit[66];

		//-------------67--------------

		out[5] = (~disable_update & ((bit[72]&~bit[71]&bit[70]&~bit[69]&~bit[68]) | (bit[72]&~bit[71]&bit[70]&bit[69]&~bit[68]) | (bit[72]&bit[71]&bit[70]&~bit[69]&~bit[68]) | (bit[72]&bit[71]&~bit[70]&~bit[69]&~bit[68]) | (bit[71]&bit[70]&bit[69]&~bit[68]) | (~bit[72]&bit[71]&bit[69]&~bit[68]) | (~bit[72]&~bit[71]&bit[69]&~bit[68]) | (bit[72]&~bit[70]&bit[69]&~bit[68]) | (~bit[71]&~bit[70]&~bit[69]) | (~bit[72]&~bit[69]&~bit[68]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((~bit[72]&~bit[71]&~bit[70]&bit[69]&bit[68]) | (bit[72]&~bit[71]&bit[70]&~bit[69]&~bit[68]) | (~bit[72]&~bit[71]&bit[70]&~bit[69]&bit[68]) | (bit[72]&bit[71]&~bit[70]&~bit[69]&~bit[68]) | (bit[72]&~bit[71]&bit[70]&~bit[69]&bit[68]) | (~bit[71]&~bit[70]&~bit[69]&~bit[68]) | (bit[71]&~bit[69]&bit[68]) | (~bit[72]&~bit[69]&~bit[68]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((bit[72]&~bit[71]&~bit[70]&bit[69]&bit[68]) | (~bit[72]&~bit[71]&bit[70]&~bit[69]&bit[68]) | (bit[72]&bit[71]&bit[70]&~bit[69]&~bit[68]) | (bit[71]&~bit[70]&bit[68]) | (~bit[72]&~bit[71]&bit[69]&~bit[68]) | (bit[72]&~bit[70]&bit[69]&~bit[68]) | (~bit[72]&~bit[70]&~bit[68]) | (~bit[71]&~bit[70]&~bit[69]&~bit[68]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((~bit[72]&~bit[71]&bit[70]&~bit[69]&~bit[68]) | (~bit[72]&~bit[71]&~bit[70]&bit[69]&~bit[68]) | (bit[72]&~bit[71]&~bit[70]&bit[69]&bit[68]) | (~bit[71]&bit[70]&bit[69]&bit[68]) | (bit[72]&~bit[71]&bit[70]&bit[69]&~bit[68]) | (bit[72]&bit[71]&bit[70]&~bit[69]&~bit[68]) | (bit[72]&bit[71]&~bit[70]&~bit[69]&~bit[68]) | (~bit[72]&bit[71]&~bit[70]&bit[68]) | (bit[71]&bit[70]&bit[69]&~bit[68]) | (bit[72]&~bit[71]&bit[70]&~bit[69]&bit[68]) | (bit[71]&~bit[69]&bit[68]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((bit[71]&bit[70]&bit[69]&bit[68]) | (bit[72]&~bit[71]&bit[69]&~bit[68]) | (bit[72]&~bit[71]&bit[70]&~bit[69]&~bit[68]) | (bit[72]&~bit[71]&~bit[70]&bit[69]&bit[68]) | (~bit[72]&~bit[71]&~bit[70]&~bit[69]) | (bit[72]&bit[71]&~bit[70]&~bit[69]&~bit[68]) | (bit[72]&~bit[71]&bit[70]&~bit[69]&bit[68]) | (~bit[72]&bit[71]&bit[69]&~bit[68]) | (~bit[72]&bit[70]&bit[68]) | (~bit[72]&~bit[69]&bit[68]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((~bit[72]&~bit[71]&~bit[70]&bit[69]&bit[68]) | (~bit[72]&bit[71]&bit[69]&bit[68]) | (~bit[72]&~bit[71]&bit[70]&~bit[69]&bit[68]) | (bit[72]&~bit[71]&bit[70]&bit[69]&~bit[68]) | (bit[72]&bit[71]&bit[70]&~bit[69]&~bit[68]) | (~bit[72]&bit[71]&~bit[70]&bit[68]) | (bit[72]&~bit[71]&bit[70]&~bit[69]&bit[68]) | (bit[72]&~bit[70]&bit[69]&~bit[68]) | (~bit[71]&~bit[70]&~bit[69]&~bit[68]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[67]);
		control=control & bit[67];

		//-------------68--------------

		out[5] = (~disable_update & ((bit[73]&bit[72]&~bit[71]&bit[70]&~bit[69]) | (bit[73]&bit[72]&~bit[71]&~bit[70]&bit[69]) | (~bit[73]&bit[72]&~bit[71]&~bit[70]&bit[69]) | (bit[73]&~bit[72]&~bit[71]&~bit[70]&bit[69]) | (~bit[73]&~bit[72]&~bit[70]&bit[69]) | (~bit[73]&~bit[71]&bit[70]&~bit[69]) | (bit[72]&bit[71]&bit[70]&~bit[69]) | (~bit[72]&bit[70]&~bit[69]) | (bit[71]&~bit[70]&~bit[69]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((bit[73]&bit[72]&~bit[71]&bit[70]&~bit[69]) | (bit[73]&~bit[72]&bit[71]&~bit[70]&bit[69]) | (~bit[73]&~bit[71]&bit[70]&bit[69]) | (bit[72]&bit[71]&~bit[70]&bit[69]) | (~bit[72]&~bit[71]&bit[70]&bit[69]) | (~bit[73]&~bit[71]&bit[70]&~bit[69]) | (~bit[72]&bit[70]&~bit[69]) | (bit[71]&~bit[70]&~bit[69]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((~bit[73]&bit[72]&bit[71]&~bit[70]) | (bit[73]&~bit[72]&bit[71]&~bit[70]&bit[69]) | (~bit[73]&bit[72]&~bit[71]&~bit[70]&bit[69]) | (~bit[72]&~bit[71]&~bit[70]&bit[69]) | (bit[73]&bit[72]&~bit[71]&bit[70]&bit[69]) | (~bit[72]&bit[71]&bit[70]&bit[69]) | (bit[72]&bit[71]&bit[70]&~bit[69]) | (bit[71]&~bit[70]&~bit[69]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((~bit[72]&~bit[71]&bit[70]&~bit[69]) | (bit[73]&bit[72]&~bit[71]&~bit[70]&bit[69]) | (~bit[73]&~bit[72]&~bit[70]&bit[69]) | (bit[73]&bit[72]&~bit[71]&bit[70]&bit[69]) | (bit[72]&bit[71]&~bit[70]&bit[69]) | (~bit[73]&~bit[71]&bit[70]&~bit[69]) | (~bit[72]&bit[71]&~bit[70]) | (bit[72]&bit[71]&bit[70]&~bit[69]) | (~bit[73]&bit[71]&bit[69]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((bit[73]&bit[72]&~bit[71]&bit[70]&~bit[69]) | (~bit[73]&bit[72]&bit[71]&~bit[69]) | (bit[73]&~bit[72]&bit[71]&~bit[70]&bit[69]) | (~bit[73]&~bit[72]&~bit[71]&bit[70]) | (bit[73]&bit[72]&~bit[71]&~bit[70]&bit[69]) | (bit[73]&~bit[72]&~bit[71]&~bit[70]&bit[69]) | (bit[73]&bit[72]&bit[71]&bit[69]) | (bit[73]&bit[72]&~bit[71]&bit[70]&bit[69]) | (~bit[72]&~bit[71]&bit[70]&bit[69]) | (bit[72]&bit[71]&bit[70]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((~bit[73]&bit[72]&~bit[71]&~bit[70]&bit[69]) | (bit[73]&~bit[72]&~bit[71]&~bit[70]&bit[69]) | (bit[73]&bit[72]&bit[71]&bit[69]) | (~bit[73]&~bit[71]&bit[70]&bit[69]) | (bit[73]&bit[72]&~bit[71]&bit[70]&bit[69]) | (~bit[72]&bit[71]&bit[70]&bit[69]) | (~bit[73]&bit[71]&~bit[69]) | (~bit[72]&~bit[71]&~bit[69]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[68]);
		control=control & bit[68];

		//-------------69--------------

		out[5] = (~disable_update & ((bit[75]&bit[74]&bit[73]&bit[72]&bit[71]&bit[70]) | (~bit[75]&bit[73]&bit[72]&bit[71]&bit[70]) | (~bit[73]&~bit[72]&~bit[71]&bit[70]) | (bit[74]&~bit[73]&bit[71]&~bit[70]) | (~bit[74]&bit[73]&bit[72]&bit[71]&bit[70]) | (bit[73]&bit[72]&~bit[71]&~bit[70]) | (~bit[74]&~bit[72]&bit[71]&~bit[70]) | (bit[73]&bit[72]&bit[71]&~bit[70]) | (bit[74]&bit[73]&~bit[72]&bit[71]&~bit[70]) | (~bit[73]&bit[72]&~bit[70]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((bit[75]&bit[74]&bit[73]&bit[72]&bit[71]&bit[70]) | (~bit[75]&bit[73]&bit[72]&bit[71]&bit[70]) | (~bit[73]&~bit[72]&bit[71]&~bit[70]) | (bit[74]&~bit[73]&bit[72]&~bit[71]) | (~bit[74]&bit[73]&bit[72]&bit[71]&bit[70]) | (bit[74]&bit[72]&~bit[71]&~bit[70]) | (~bit[74]&~bit[72]&bit[71]&~bit[70]) | (bit[73]&~bit[72]&~bit[71]&bit[70]) | (~bit[74]&bit[72]&~bit[71]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((~bit[74]&~bit[73]&~bit[72]&bit[71]&bit[70]) | (~bit[75]&bit[73]&bit[72]&bit[71]&bit[70]) | (bit[74]&bit[73]&bit[72]&~bit[71]&bit[70]) | (~bit[74]&~bit[73]&bit[72]&~bit[71]) | (~bit[74]&bit[72]&~bit[71]&~bit[70]) | (~bit[74]&bit[73]&bit[72]&bit[71]&bit[70]) | (bit[74]&bit[73]&~bit[72]&bit[71]&~bit[70]) | (bit[73]&~bit[72]&~bit[71]&bit[70]) | (~bit[73]&bit[72]&~bit[70]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((bit[75]&bit[74]&bit[73]&bit[72]&bit[71]&bit[70]) | (~bit[74]&~bit[73]&bit[72]&~bit[71]&~bit[70]) | (bit[74]&bit[73]&bit[72]&~bit[71]&bit[70]) | (bit[74]&~bit[72]&bit[71]&bit[70]) | (~bit[74]&bit[73]&~bit[72]&bit[70]) | (bit[73]&bit[72]&bit[71]&~bit[70]) | (bit[74]&bit[73]&~bit[72]&bit[71]&~bit[70]) | (bit[73]&~bit[72]&~bit[71]&bit[70]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((~bit[75]&~bit[74]&bit[73]&bit[71]&bit[70]) | (bit[74]&bit[73]&bit[72]&~bit[71]&bit[70]) | (~bit[74]&~bit[73]&bit[71]&~bit[70]) | (bit[74]&~bit[73]&bit[72]&~bit[71]) | (~bit[74]&bit[72]&bit[71]&~bit[70]) | (~bit[74]&bit[73]&~bit[72]&bit[70]) | (bit[74]&bit[72]&~bit[71]&~bit[70]) | (bit[74]&bit[73]&~bit[72]&bit[71]&~bit[70]) | (~bit[73]&~bit[72]&bit[70]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((bit[75]&bit[74]&bit[73]&bit[72]&bit[71]&bit[70]) | (~bit[73]&~bit[72]&~bit[71]&~bit[70]) | (~bit[74]&~bit[72]&~bit[71]&bit[70]) | (bit[74]&~bit[72]&bit[71]&bit[70]) | (~bit[74]&bit[72]&bit[71]&bit[70]) | (bit[74]&~bit[73]&bit[71]&~bit[70]) | (bit[73]&bit[72]&~bit[71]&~bit[70]) | (bit[74]&bit[73]&~bit[72]&bit[71]&~bit[70]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[69]);
		control=control & bit[69];

		//-------------70--------------

		out[5] = (~disable_update & ((bit[76]&bit[75]&~bit[74]&bit[73]&bit[72]&bit[71]) | (~bit[76]&~bit[75]&bit[73]&bit[72]&bit[71]) | (bit[76]&~bit[75]&bit[73]&bit[72]&bit[71]) | (~bit[76]&~bit[74]&bit[73]&bit[72]&bit[71]) | (bit[75]&bit[74]&bit[73]&bit[72]&bit[71]) | (bit[75]&bit[74]&bit[73]&~bit[72]&~bit[71]) | (~bit[73]&bit[72]&~bit[71]) | (~bit[75]&~bit[72]&~bit[71]) | (~bit[74]&~bit[71]) | (~bit[73]&~bit[72]&~bit[71]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((bit[76]&bit[75]&~bit[74]&bit[73]&bit[72]&bit[71]) | (bit[74]&~bit[73]&~bit[72]&bit[71]) | (~bit[76]&~bit[75]&bit[73]&bit[72]&bit[71]) | (bit[76]&~bit[75]&bit[73]&bit[72]&bit[71]) | (~bit[76]&~bit[74]&bit[73]&bit[72]&bit[71]) | (bit[75]&bit[74]&bit[73]&bit[72]&bit[71]) | (~bit[74]&~bit[72]&bit[71]) | (bit[74]&bit[73]&bit[72]&~bit[71]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((bit[76]&bit[75]&~bit[74]&bit[73]&bit[72]&bit[71]) | (~bit[76]&~bit[75]&bit[73]&bit[72]&bit[71]) | (bit[76]&~bit[75]&bit[73]&bit[72]&bit[71]) | (~bit[76]&~bit[74]&bit[73]&bit[72]&bit[71]) | (~bit[75]&~bit[74]&~bit[73]&~bit[72]) | (bit[74]&bit[73]&~bit[72]&bit[71]) | (~bit[74]&bit[73]&~bit[72]&~bit[71]) | (~bit[75]&~bit[72]&~bit[71]) | (bit[74]&bit[73]&bit[72]&~bit[71]) | (~bit[73]&~bit[72]&~bit[71]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((~bit[75]&~bit[74]&bit[72]&bit[71]) | (bit[75]&~bit[74]&~bit[73]&bit[72]&~bit[71]) | (bit[75]&~bit[74]&~bit[73]&~bit[72]) | (bit[74]&~bit[73]&~bit[72]&bit[71]) | (~bit[76]&~bit[74]&bit[73]&bit[72]&bit[71]) | (bit[75]&bit[74]&bit[73]&bit[72]&bit[71]) | (bit[75]&bit[74]&bit[73]&~bit[72]&~bit[71]) | (bit[74]&bit[73]&~bit[72]&bit[71]) | (~bit[75]&~bit[74]&~bit[73]&~bit[71]) | (~bit[74]&~bit[73]&bit[72]&bit[71]) | (bit[74]&bit[73]&bit[72]&~bit[71]) | (~bit[73]&~bit[72]&~bit[71]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((~bit[75]&bit[74]&bit[72]&~bit[71]) | (bit[76]&bit[75]&~bit[74]&bit[73]&bit[72]&bit[71]) | (~bit[75]&bit[74]&~bit[72]&bit[71]) | (~bit[75]&~bit[74]&bit[73]&~bit[72]) | (bit[75]&~bit[74]&~bit[73]&~bit[72]) | (~bit[76]&~bit[75]&bit[73]&bit[72]&bit[71]) | (bit[75]&bit[74]&bit[73]&bit[72]&bit[71]) | (bit[75]&bit[74]&bit[73]&~bit[72]&~bit[71]) | (~bit[75]&~bit[74]&~bit[73]&~bit[71]) | (~bit[74]&~bit[73]&bit[72]&bit[71]) | (bit[74]&~bit[73]&bit[72]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((~bit[76]&bit[75]&bit[74]&bit[73]&bit[72]) | (bit[75]&~bit[74]&~bit[73]&bit[72]&~bit[71]) | (bit[76]&bit[75]&~bit[74]&bit[73]&bit[72]&bit[71]) | (~bit[75]&bit[74]&~bit[73]&~bit[71]) | (~bit[75]&~bit[73]&bit[72]&bit[71]) | (~bit[75]&~bit[74]&bit[73]&~bit[71]) | (bit[76]&~bit[75]&bit[73]&bit[72]&bit[71]) | (~bit[75]&~bit[74]&~bit[73]&~bit[72]) | (bit[75]&bit[74]&bit[73]&~bit[72]&~bit[71]) | (~bit[74]&bit[73]&~bit[72]&~bit[71]) | (~bit[74]&~bit[72]&bit[71]) | (bit[74]&bit[73]&bit[72]&~bit[71]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[70]);
		control=control & bit[70];

		//-------------71--------------

		out[5] = (~disable_update & ((~bit[75]&~bit[74]&bit[73]&~bit[72]) | (bit[75]&bit[74]&bit[73]&bit[72]) | (bit[76]&bit[74]&bit[73]&bit[72]) | (bit[75]&~bit[74]&bit[73]&~bit[72]) | (~bit[76]&~bit[75]&~bit[72]) | (~bit[73]&~bit[72]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((~bit[76]&~bit[75]&~bit[73]&bit[72]) | (~bit[76]&bit[75]&bit[74]&bit[73]&~bit[72]) | (bit[76]&bit[75]&~bit[74]&~bit[73]&bit[72]) | (bit[76]&bit[75]&bit[74]&bit[73]&~bit[72]) | (bit[75]&bit[74]&bit[73]&bit[72]) | (bit[76]&~bit[75]&~bit[74]&~bit[73]) | (bit[76]&~bit[75]&bit[74]&bit[73]) | (~bit[76]&~bit[74]&~bit[73]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((~bit[76]&~bit[75]&bit[74]&~bit[73]&~bit[72]) | (~bit[76]&~bit[75]&~bit[74]&bit[72]) | (bit[76]&bit[75]&~bit[74]&~bit[73]&~bit[72]) | (bit[76]&bit[75]&bit[74]&bit[73]&~bit[72]) | (bit[76]&~bit[75]&bit[74]&~bit[73]) | (bit[75]&bit[74]&~bit[73]&bit[72]) | (bit[76]&~bit[75]&bit[74]&bit[73]) | (~bit[76]&bit[75]&bit[74]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((~bit[76]&~bit[75]&~bit[74]&~bit[73]&~bit[72]) | (~bit[76]&bit[75]&bit[74]&bit[73]&~bit[72]) | (bit[76]&bit[75]&~bit[74]&~bit[73]&~bit[72]) | (~bit[76]&bit[75]&~bit[74]&bit[72]) | (bit[76]&bit[75]&bit[74]&~bit[73]&~bit[72]) | (~bit[75]&~bit[74]&bit[73]&~bit[72]) | (bit[76]&bit[74]&bit[73]&bit[72]) | (bit[76]&~bit[75]&bit[74]&bit[73]) | (bit[76]&~bit[75]&bit[72]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((~bit[77]&bit[76]&bit[74]&bit[73]&bit[72]) | (~bit[77]&~bit[76]&bit[75]&bit[74]&bit[72]) | (~bit[76]&~bit[75]&bit[74]&~bit[73]&~bit[72]) | (bit[76]&bit[75]&~bit[74]&~bit[73]&bit[72]) | (bit[76]&~bit[75]&~bit[74]&bit[73]&bit[72]) | (bit[76]&bit[75]&~bit[74]&~bit[73]&~bit[72]) | (bit[76]&bit[75]&bit[74]&bit[73]&~bit[72]) | (~bit[76]&~bit[74]&bit[73]&~bit[72]) | (bit[76]&bit[75]&bit[74]&~bit[73]&~bit[72]) | (bit[75]&~bit[74]&bit[73]&~bit[72]) | (bit[75]&bit[74]&~bit[73]&bit[72]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((bit[77]&bit[76]&bit[75]&bit[73]&bit[72]) | (~bit[77]&~bit[76]&bit[75]&bit[74]&bit[72]) | (~bit[76]&~bit[75]&~bit[74]&~bit[73]&~bit[72]) | (bit[76]&~bit[75]&~bit[74]&bit[73]&bit[72]) | (~bit[76]&bit[74]&~bit[73]&bit[72]) | (~bit[76]&bit[75]&~bit[74]&bit[73]) | (bit[76]&bit[75]&bit[74]&~bit[73]&~bit[72]) | (bit[75]&~bit[74]&bit[73]&bit[72]) | (bit[76]&~bit[75]&~bit[74]&~bit[73]) | (bit[76]&~bit[75]&bit[74]&~bit[73]) | (bit[76]&~bit[75]&~bit[72]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[71]);
		control=control & bit[71];

		//-------------72--------------

		out[5] = (~disable_update & ((~bit[78]&~bit[77]&bit[76]&bit[75]&bit[74]&bit[73]) | (~bit[78]&bit[77]&bit[75]&bit[74]&bit[73]) | (bit[78]&bit[76]&bit[75]&bit[74]&bit[73]) | (~bit[76]&~bit[75]&~bit[73]) | (bit[77]&~bit[76]&bit[75]&bit[74]&bit[73]) | (~bit[74]&~bit[73]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((~bit[78]&~bit[77]&bit[76]&bit[75]&bit[74]&bit[73]) | (~bit[77]&~bit[75]&~bit[74]) | (~bit[78]&bit[77]&bit[75]&bit[74]&bit[73]) | (bit[78]&bit[76]&bit[75]&bit[74]&bit[73]) | (~bit[76]&~bit[75]&~bit[74]&~bit[73]) | (~bit[76]&bit[75]&bit[74]&~bit[73]) | (~bit[75]&~bit[74]&bit[73]) | (bit[77]&~bit[76]&bit[75]&bit[74]&bit[73]) | (bit[76]&bit[75]&bit[74]&~bit[73]) | (bit[76]&~bit[75]&bit[74]&~bit[73]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((~bit[78]&~bit[77]&bit[76]&bit[75]&bit[74]&bit[73]) | (~bit[78]&bit[77]&bit[75]&bit[74]&bit[73]) | (bit[78]&bit[76]&bit[75]&bit[74]&bit[73]) | (bit[77]&bit[76]&~bit[75]&~bit[74]&~bit[73]) | (~bit[76]&bit[75]&bit[74]&~bit[73]) | (~bit[77]&bit[75]&~bit[74]) | (bit[77]&~bit[76]&bit[75]&bit[74]&bit[73]) | (bit[77]&~bit[76]&bit[75]&~bit[73]) | (bit[76]&~bit[75]&bit[74]&~bit[73]) | (~bit[76]&bit[75]&~bit[74]&bit[73]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((~bit[78]&~bit[77]&bit[76]&bit[75]&bit[74]&bit[73]) | (~bit[77]&~bit[76]&bit[75]&~bit[73]) | (bit[77]&bit[76]&bit[75]&~bit[74]) | (bit[77]&bit[76]&~bit[75]&~bit[74]&~bit[73]) | (~bit[77]&~bit[76]&bit[75]&~bit[74]) | (~bit[76]&~bit[75]&bit[73]) | (bit[77]&~bit[76]&bit[75]&bit[74]&bit[73]) | (bit[76]&bit[75]&bit[74]&~bit[73]) | (bit[76]&~bit[75]&bit[74]&~bit[73]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((bit[78]&~bit[77]&bit[76]&bit[73]) | (~bit[77]&~bit[76]&~bit[75]&~bit[73]) | (~bit[77]&bit[76]&~bit[74]&bit[73]) | (bit[76]&~bit[75]&bit[74]&bit[73]) | (~bit[78]&bit[77]&bit[75]&bit[74]&bit[73]) | (bit[77]&bit[76]&bit[75]&~bit[74]) | (bit[77]&bit[76]&~bit[75]&~bit[74]&~bit[73]) | (~bit[77]&bit[76]&bit[74]&~bit[73]) | (~bit[77]&~bit[76]&bit[75]&~bit[74]) | (bit[77]&~bit[76]&bit[75]&bit[74]&bit[73]) | (bit[77]&~bit[76]&bit[75]&~bit[73]) | (bit[76]&bit[75]&bit[74]&~bit[73]) | (~bit[76]&bit[75]&~bit[74]&bit[73]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((~bit[78]&bit[77]&~bit[76]&bit[75]) | (~bit[78]&~bit[77]&bit[76]&bit[75]&bit[74]&bit[73]) | (~bit[77]&~bit[75]&bit[74]&bit[73]) | (~bit[77]&~bit[76]&~bit[74]&bit[73]) | (bit[78]&bit[76]&bit[75]&bit[74]&bit[73]) | (bit[77]&bit[76]&~bit[75]&~bit[74]&~bit[73]) | (~bit[76]&~bit[75]&~bit[74]&~bit[73]) | (~bit[77]&bit[76]&bit[74]&~bit[73]) | (bit[77]&~bit[76]&bit[75]&~bit[73]) | (bit[76]&~bit[75]&bit[74]&~bit[73]) | (~bit[76]&bit[75]&~bit[74]&bit[73]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[72]);
		control=control & bit[72];

		//-------------73--------------

		out[5] = (~disable_update & ((bit[77]&bit[76]&~bit[75]&~bit[74]) | (~bit[78]&bit[77]&~bit[76]&~bit[74]) | (bit[78]&~bit[76]&~bit[74]) | (~bit[77]&~bit[74]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((~bit[78]&~bit[77]&~bit[76]&bit[74]) | (bit[77]&bit[76]&bit[75]&~bit[74]) | (~bit[78]&~bit[77]&~bit[75]) | (~bit[76]&~bit[75]) | (~bit[75]&bit[74]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((~bit[78]&~bit[77]&~bit[76]&bit[75]&~bit[74]) | (bit[78]&~bit[77]&bit[76]&~bit[75]&~bit[74]) | (bit[77]&~bit[76]&bit[75]&bit[74]) | (~bit[77]&bit[76]&bit[75]&bit[74]) | (~bit[78]&~bit[76]&~bit[75]&bit[74]) | (~bit[78]&~bit[77]&~bit[76]&~bit[75]) | (bit[77]&bit[76]&bit[75]&~bit[74]) | (bit[78]&~bit[77]&~bit[76]&bit[75]&bit[74]) | (bit[77]&bit[76]&~bit[75]&~bit[74]) | (~bit[77]&~bit[76]&~bit[75]&bit[74]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((bit[78]&bit[77]&~bit[76]&~bit[75]&bit[74]) | (bit[78]&~bit[77]&bit[76]&~bit[75]&~bit[74]) | (bit[78]&~bit[77]&~bit[76]&~bit[74]) | (~bit[77]&bit[76]&~bit[75]&bit[74]) | (bit[77]&bit[76]&bit[75]&~bit[74]) | (bit[78]&~bit[77]&~bit[76]&bit[75]&bit[74]) | (bit[77]&bit[76]&~bit[75]&~bit[74]) | (~bit[78]&bit[77]&~bit[76]&~bit[74]) | (bit[77]&bit[75]&bit[74]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((bit[78]&bit[77]&~bit[76]&~bit[75]&bit[74]) | (~bit[78]&~bit[77]&bit[76]&bit[75]&bit[74]) | (~bit[78]&~bit[77]&~bit[76]&bit[75]&~bit[74]) | (~bit[78]&bit[77]&bit[75]&bit[74]) | (bit[78]&~bit[77]&bit[76]&~bit[75]&~bit[74]) | (~bit[78]&~bit[77]&~bit[75]&bit[74]) | (bit[78]&~bit[77]&~bit[76]&bit[75]&bit[74]) | (~bit[78]&bit[77]&bit[76]) | (bit[78]&~bit[76]&~bit[74]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((~bit[78]&~bit[77]&bit[76]&bit[75]&bit[74]) | (bit[78]&bit[77]&~bit[75]&bit[74]) | (~bit[78]&~bit[77]&bit[76]&~bit[74]) | (bit[78]&~bit[77]&~bit[75]&~bit[74]) | (bit[78]&~bit[77]&~bit[76]&bit[75]&bit[74]) | (~bit[78]&bit[77]&~bit[76]&~bit[74]) | (bit[77]&bit[75]&~bit[74]) | (~bit[77]&~bit[76]&~bit[75]&bit[74]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[73]);
		control=control & bit[73];

		//-------------74--------------

		out[5] = (~disable_update & ((~bit[79]&~bit[78]&bit[77]&~bit[75]) | (~bit[77]&bit[76]&~bit[75]) | (bit[77]&~bit[76]&~bit[75]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((bit[79]&bit[78]&~bit[77]&~bit[76]&bit[75]) | (~bit[78]&~bit[77]&bit[76]&~bit[75]) | (~bit[79]&~bit[78]&~bit[77]&bit[75]) | (~bit[79]&bit[78]&bit[77]&bit[76]&~bit[75]) | (~bit[78]&bit[77]&~bit[76]&bit[75]) | (~bit[79]&~bit[77]&~bit[76]&bit[75]) | (bit[78]&bit[77]&~bit[76]&bit[75]) | (~bit[78]&~bit[77]&~bit[76]&bit[75]) | (bit[79]&bit[77]&bit[76]&~bit[75]) | (bit[77]&~bit[76]&~bit[75]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((bit[79]&~bit[78]&~bit[77]&bit[76]&bit[75]) | (~bit[79]&bit[78]&bit[77]&bit[76]&~bit[75]) | (~bit[79]&~bit[77]&~bit[76]&bit[75]) | (bit[78]&~bit[77]&bit[76]) | (~bit[78]&~bit[77]&~bit[76]&bit[75]) | (bit[79]&bit[77]&bit[76]&~bit[75]) | (bit[77]&~bit[76]&~bit[75]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((bit[79]&bit[78]&~bit[77]&~bit[76]&bit[75]) | (bit[79]&~bit[78]&~bit[77]&bit[76]&bit[75]) | (~bit[79]&~bit[78]&bit[77]&~bit[75]) | (~bit[78]&bit[77]&~bit[76]&bit[75]) | (~bit[78]&bit[77]&bit[76]&bit[75]) | (~bit[79]&bit[76]&~bit[75]) | (bit[79]&bit[77]&bit[76]&~bit[75]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((bit[79]&bit[78]&~bit[77]&~bit[76]&bit[75]) | (~bit[79]&~bit[78]&~bit[77]&~bit[76]) | (bit[79]&~bit[78]&~bit[77]&bit[76]&bit[75]) | (~bit[79]&bit[78]&bit[77]&bit[76]&~bit[75]) | (~bit[79]&bit[77]&bit[76]&bit[75]) | (bit[79]&~bit[78]&~bit[75]) | (bit[78]&bit[77]&~bit[76]&bit[75]) | (~bit[78]&bit[77]&bit[76]&bit[75]) | (~bit[77]&bit[76]&~bit[75]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((bit[79]&bit[78]&~bit[77]&~bit[75]) | (~bit[79]&bit[78]&~bit[76]&~bit[75]) | (bit[78]&bit[77]&bit[76]&bit[75]) | (~bit[79]&bit[78]&bit[76]&bit[75]) | (~bit[79]&bit[77]&bit[75]) | (~bit[78]&~bit[77]&~bit[76]&bit[75]) | (bit[79]&bit[77]&bit[76]&~bit[75]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[74]);
		control=control & bit[74];

		//-------------75--------------

		out[6] = (~disable_update & ((~bit[80]&~bit[79]&~bit[78]&~bit[77]&~bit[76]))) | (disable_update & out[6]);
		out[5] = (~disable_update & ((bit[79]&~bit[78]&~bit[77]&bit[76]) | (~bit[80]&~bit[78]&~bit[77]&bit[76]) | (bit[80]&~bit[78]&bit[77]&~bit[76]) | (bit[80]&~bit[79]&~bit[78]&~bit[77]) | (~bit[79]&bit[78]&~bit[77]) | (bit[79]&~bit[78]&~bit[77]&~bit[76]) | (~bit[80]&~bit[78]&bit[77]&~bit[76]) | (bit[78]&~bit[76]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((bit[80]&~bit[78]&bit[77]&bit[76]) | (~bit[80]&~bit[78]&bit[77]&bit[76]) | (bit[80]&~bit[79]&~bit[78]&~bit[76]) | (~bit[80]&~bit[79]&bit[77]) | (bit[80]&~bit[78]&bit[77]&~bit[76]) | (~bit[79]&bit[78]&~bit[77]&~bit[76]) | (bit[79]&~bit[78]&~bit[77]&~bit[76]) | (bit[79]&bit[78]&~bit[77]) | (~bit[80]&~bit[78]&bit[77]&~bit[76]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((bit[79]&bit[78]&bit[77]&~bit[76]) | (~bit[80]&bit[79]&bit[78]&bit[76]) | (bit[80]&~bit[79]&bit[78]&bit[77]) | (bit[80]&~bit[79]&~bit[78]&~bit[77]) | (~bit[79]&~bit[78]&bit[76]) | (~bit[79]&bit[78]&~bit[77]&~bit[76]) | (bit[79]&~bit[78]&~bit[77]&~bit[76]) | (bit[79]&bit[78]&~bit[77]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((~bit[80]&bit[78]&~bit[77]&bit[76]) | (bit[79]&bit[78]&bit[77]&~bit[76]) | (bit[80]&bit[78]&bit[77]) | (bit[80]&~bit[79]&~bit[78]&~bit[76]) | (bit[79]&~bit[78]&bit[76]) | (bit[79]&~bit[78]&~bit[77]&~bit[76]) | (~bit[80]&~bit[78]&bit[77]&~bit[76]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((~bit[79]&~bit[78]&bit[77]&~bit[76]) | (~bit[80]&bit[79]&~bit[78]&~bit[77]) | (~bit[80]&bit[79]&bit[78]&bit[76]) | (bit[80]&bit[78]&~bit[77]&bit[76]) | (bit[79]&~bit[78]&~bit[77]&bit[76]) | (~bit[80]&~bit[78]&bit[77]&bit[76]) | (bit[80]&~bit[79]&bit[78]&bit[77]) | (bit[80]&~bit[79]&~bit[78]&~bit[76]) | (~bit[80]&~bit[78]&~bit[77]&bit[76]) | (bit[80]&~bit[78]&bit[77]&~bit[76]) | (~bit[79]&bit[78]&~bit[77]&~bit[76]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((~bit[80]&bit[79]&~bit[78]&~bit[77]&bit[76]) | (~bit[79]&bit[78]&bit[76]) | (bit[80]&~bit[78]&bit[77]&bit[76]) | (bit[80]&bit[78]&~bit[77]&bit[76]) | (bit[80]&~bit[79]&bit[78]&bit[77]) | (bit[80]&~bit[79]&~bit[78]&~bit[77]) | (~bit[80]&bit[78]&~bit[76]) | (~bit[80]&bit[78]&bit[77]) | (~bit[80]&~bit[78]&bit[77]&~bit[76]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[75]);
		control=control & bit[75];

		//-------------76--------------

		out[6] = (~disable_update & (out[6]&0)) | (disable_update & out[6]);		
		out[5] = (~disable_update & ((~bit[81]&bit[80]&~bit[79]&~bit[78]&bit[77]) | (~bit[81]&~bit[80]&~bit[78]&bit[77]) | (~bit[81]&bit[80]&~bit[79]&bit[78]&~bit[77]) | (bit[81]&~bit[79]&~bit[78]&bit[77]) | (bit[81]&bit[80]&~bit[79]&bit[78]&~bit[77]) | (~bit[80]&~bit[79]&bit[78]&~bit[77]) | (bit[80]&bit[79]&~bit[78]&~bit[77]) | (bit[79]&bit[78]&~bit[77]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((bit[81]&~bit[80]&bit[79]&~bit[78]&bit[77]) | (bit[81]&bit[80]&bit[79]&~bit[78]&bit[77]) | (~bit[81]&bit[80]&~bit[79]&bit[78]&~bit[77]) | (bit[81]&~bit[79]&bit[78]&bit[77]) | (~bit[81]&~bit[80]&bit[78]&bit[77]) | (~bit[80]&~bit[79]&bit[78]&~bit[77]) | (bit[80]&bit[79]&~bit[78]&~bit[77]) | (~bit[81]&~bit[79]&bit[78]&bit[77]) | (~bit[81]&bit[80]&bit[79]&~bit[78]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((~bit[81]&~bit[80]&~bit[79]&~bit[78]&bit[77]) | (bit[81]&~bit[80]&bit[79]&~bit[78]&bit[77]) | (~bit[81]&~bit[80]&bit[78]&~bit[77]) | (bit[81]&bit[80]&~bit[79]&bit[78]&~bit[77]) | (bit[81]&bit[79]&bit[78]&bit[77]) | (bit[80]&bit[79]&~bit[78]&~bit[77]) | (~bit[81]&bit[80]&bit[79]&bit[78]) | (~bit[81]&bit[80]&bit[79]&~bit[78]) | (bit[79]&bit[78]&~bit[77]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((~bit[81]&bit[80]&bit[79]&~bit[78]&~bit[77]) | (~bit[81]&bit[80]&~bit[79]&~bit[78]&bit[77]) | (~bit[81]&~bit[80]&bit[79]&bit[78]&~bit[77]) | (bit[81]&bit[80]&bit[79]&~bit[78]&bit[77]) | (bit[81]&bit[80]&~bit[79]&bit[78]&~bit[77]) | (bit[81]&~bit[80]&bit[77]) | (~bit[81]&~bit[79]&bit[78]&bit[77]) | (bit[81]&~bit[80]&bit[78]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((~bit[81]&~bit[80]&~bit[79]&bit[78]&bit[77]) | (~bit[81]&~bit[80]&bit[79]&bit[78]&~bit[77]) | (~bit[80]&~bit[79]&~bit[78]&~bit[77]) | (bit[81]&bit[80]&bit[79]&~bit[78]&bit[77]) | (~bit[81]&bit[80]&~bit[79]&bit[78]&~bit[77]) | (bit[81]&~bit[79]&~bit[78]&bit[77]) | (bit[81]&bit[80]&~bit[79]&bit[78]&~bit[77]) | (bit[81]&bit[79]&bit[78]&bit[77]) | (~bit[81]&bit[80]&bit[79]&bit[78]) | (~bit[81]&bit[80]&bit[79]&~bit[78]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((bit[81]&bit[80]&bit[79]&~bit[78]&bit[77]) | (bit[81]&~bit[79]&bit[78]&bit[77]) | (~bit[81]&bit[80]&bit[79]&bit[78]) | (bit[80]&~bit[77]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[76]);
		control=control & bit[76];

		//-------------77--------------

		out[6] = (~disable_update & ((~bit[83]&~bit[82]&bit[81]&bit[80]&bit[79]&bit[78]))) | (disable_update & out[6]);		
		out[5] = (~disable_update & ((~bit[82]&~bit[81]&~bit[80]&~bit[79]&bit[78]) | (bit[82]&~bit[81]&bit[80]&bit[79]&~bit[78]) | (bit[83]&~bit[82]&bit[81]&bit[80]&bit[79]&bit[78]) | (~bit[82]&bit[81]&~bit[80]&bit[79]&~bit[78]) | (~bit[82]&bit[81]&bit[80]&bit[79]&~bit[78]) | (bit[82]&bit[81]&bit[80]&bit[79]&bit[78]) | (bit[82]&bit[81]&bit[79]&~bit[78]) | (~bit[81]&~bit[80]&bit[79]&~bit[78]) | (~bit[82]&~bit[81]&bit[79]&~bit[78]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((~bit[82]&bit[81]&~bit[80]&~bit[79]&bit[78]) | (bit[83]&~bit[82]&bit[81]&bit[80]&bit[79]&bit[78]) | (~bit[82]&bit[81]&~bit[80]&bit[79]&~bit[78]) | (bit[82]&~bit[80]&~bit[79]&bit[78]) | (bit[82]&bit[81]&bit[80]&bit[79]&bit[78]) | (bit[80]&~bit[79]&bit[78]) | (~bit[81]&~bit[80]&bit[79]&~bit[78]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((bit[82]&bit[81]&~bit[80]&bit[79]) | (bit[82]&~bit[81]&bit[80]&bit[79]&~bit[78]) | (bit[83]&~bit[82]&bit[81]&bit[80]&bit[79]&bit[78]) | (bit[82]&~bit[81]&~bit[80]&bit[78]) | (bit[82]&bit[81]&bit[80]&bit[79]&bit[78]) | (~bit[82]&~bit[80]&bit[79]&bit[78]) | (bit[81]&~bit[80]&bit[78]) | (~bit[82]&~bit[81]&bit[79]&~bit[78]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((~bit[82]&bit[81]&~bit[80]&~bit[79]&bit[78]) | (bit[83]&~bit[82]&bit[81]&bit[80]&bit[79]&bit[78]) | (~bit[82]&bit[81]&bit[80]&bit[79]&~bit[78]) | (bit[82]&~bit[81]&~bit[80]&bit[78]) | (bit[82]&bit[81]&bit[80]&bit[79]&bit[78]) | (~bit[82]&~bit[81]&bit[80]&bit[78]) | (~bit[81]&~bit[79]&~bit[78]) | (~bit[82]&~bit[81]&bit[79]&bit[78]) | (bit[82]&bit[81]&bit[79]&~bit[78]) | (~bit[81]&~bit[80]&bit[79]&~bit[78]) | (~bit[82]&~bit[81]&bit[79]&~bit[78]) | (~bit[80]&~bit[79]&~bit[78]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((~bit[81]&~bit[80]&~bit[79]&~bit[78]) | (bit[82]&~bit[81]&bit[80]&bit[79]&~bit[78]) | (bit[83]&~bit[82]&bit[81]&bit[80]&bit[79]&bit[78]) | (~bit[81]&bit[80]&~bit[79]&bit[78]) | (~bit[82]&bit[81]&bit[80]&bit[79]&~bit[78]) | (bit[82]&~bit[80]&bit[79]&~bit[78]) | (~bit[82]&~bit[81]&bit[80]&bit[78]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((~bit[82]&bit[81]&bit[80]&~bit[79]&bit[78]) | (~bit[83]&bit[80]&bit[79]&bit[78]) | (bit[82]&~bit[81]&bit[80]&bit[78]) | (~bit[82]&bit[81]&~bit[80]&bit[79]&~bit[78]) | (bit[82]&~bit[80]&~bit[79]&bit[78]) | (bit[82]&~bit[80]&bit[79]&~bit[78]) | (~bit[82]&~bit[80]&bit[79]&bit[78]) | (~bit[82]&~bit[81]&bit[79]&bit[78]) | (bit[82]&bit[81]&bit[79]&~bit[78]) | (~bit[80]&~bit[79]&~bit[78]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[77]);
		control=control & bit[77];

		//-------------78--------------

		out[6] = (~disable_update & ((~bit[84]&~bit[83]&bit[82]&bit[81]&bit[80]&bit[79]))) | (disable_update & out[6]);		
		out[5] = (~disable_update & ((bit[83]&bit[82]&bit[81]&bit[80]&bit[79]) | (bit[84]&~bit[83]&bit[82]&bit[81]&bit[80]&bit[79]) | (bit[83]&bit[82]&~bit[80]&~bit[79]) | (~bit[82]&bit[81]&~bit[80]&~bit[79]) | (~bit[83]&~bit[82]&~bit[79]) | (~bit[83]&~bit[80]&~bit[79]) | (~bit[81]&~bit[79]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((bit[83]&bit[82]&bit[81]&bit[80]&bit[79]) | (bit[84]&~bit[83]&bit[82]&bit[81]&bit[80]&bit[79]) | (bit[83]&~bit[82]&bit[81]&bit[80]&~bit[79]) | (~bit[82]&bit[81]&~bit[80]&~bit[79]) | (~bit[83]&~bit[82]&~bit[80]) | (bit[82]&bit[81]&bit[80]&~bit[79]) | (~bit[83]&~bit[80]&~bit[79]) | (~bit[81]&~bit[80]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((bit[83]&~bit[82]&bit[81]&~bit[80]&bit[79]) | (bit[84]&~bit[83]&bit[82]&bit[81]&bit[80]&bit[79]) | (~bit[83]&bit[82]&bit[81]&~bit[80]&bit[79]) | (bit[83]&~bit[82]&bit[81]&bit[80]&~bit[79]) | (bit[82]&bit[81]&bit[80]&~bit[79]) | (~bit[83]&~bit[81]&~bit[79]) | (bit[83]&bit[82]&bit[81]) | (~bit[82]&~bit[81]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((~bit[83]&~bit[82]&~bit[81]&bit[80]&~bit[79]) | (bit[83]&~bit[82]&bit[81]&~bit[80]&bit[79]) | (bit[84]&~bit[83]&bit[82]&bit[81]&bit[80]&bit[79]) | (bit[82]&~bit[81]&bit[80]&bit[79]) | (~bit[83]&bit[82]&bit[81]&~bit[80]&bit[79]) | (bit[83]&bit[82]&~bit[81]&~bit[79]) | (bit[83]&~bit[82]&bit[81]&bit[80]&~bit[79]) | (~bit[82]&bit[81]&~bit[80]&~bit[79]) | (bit[82]&bit[81]&bit[80]&~bit[79]) | (bit[83]&bit[82]&bit[81]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((~bit[84]&bit[83]&bit[81]&bit[80]&bit[79]) | (bit[83]&~bit[82]&bit[81]&~bit[80]&bit[79]) | (bit[84]&~bit[83]&bit[82]&bit[81]&bit[80]&bit[79]) | (bit[83]&bit[82]&~bit[81]&~bit[79]) | (bit[83]&bit[82]&~bit[80]&~bit[79]) | (~bit[83]&~bit[81]&bit[79]) | (~bit[83]&~bit[82]&~bit[81]) | (~bit[82]&bit[80]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((~bit[84]&~bit[83]&bit[82]&bit[81]&bit[80]&bit[79]) | (~bit[83]&~bit[82]&bit[81]&~bit[80]&~bit[79]) | (bit[84]&~bit[83]&bit[82]&bit[81]&bit[80]&bit[79]) | (bit[83]&~bit[81]&~bit[80]&~bit[79]) | (bit[82]&~bit[81]&~bit[80]&bit[79]) | (~bit[83]&bit[82]&bit[81]&~bit[80]&bit[79]) | (~bit[83]&~bit[82]&bit[80]&bit[79]) | (~bit[83]&bit[82]&bit[80]&~bit[79]) | (bit[83]&~bit[82]&bit[81]&bit[80]&~bit[79]) | (bit[83]&bit[82]&~bit[80]&~bit[79]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[78]);
		control=control & bit[78];

		//-------------79--------------

		out[6] = (~disable_update & ((bit[84]&bit[83]&~bit[82]&bit[81]&bit[80]))) | (disable_update & out[6]);		
		out[5] = (~disable_update & ((~bit[84]&bit[83]&~bit[82]&bit[81]&~bit[80]) | (bit[84]&bit[83]&~bit[82]&~bit[80]) | (~bit[84]&~bit[83]&bit[82]&~bit[80]) | (~bit[83]&~bit[82]&bit[81]&~bit[80]) | (bit[83]&bit[82]&~bit[81]&~bit[80]) | (~bit[84]&~bit[82]&~bit[81]&~bit[80]) | (~bit[83]&~bit[81]&~bit[80]) | (bit[82]&bit[81]&bit[80]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((~bit[84]&~bit[82]&~bit[81]&bit[80]) | (bit[84]&~bit[83]&bit[82]&bit[81]&~bit[80]) | (~bit[84]&~bit[83]&~bit[81]&bit[80]) | (bit[84]&~bit[82]&~bit[81]) | (bit[83]&bit[82]&bit[81]&~bit[80]) | (~bit[84]&~bit[82]&~bit[81]&~bit[80]) | (~bit[83]&~bit[81]&~bit[80]) | (bit[82]&bit[81]&bit[80]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((bit[85]&~bit[83]&bit[82]&bit[81]&bit[80]) | (~bit[85]&~bit[83]&bit[82]&bit[81]&bit[80]) | (bit[84]&~bit[83]&bit[82]&~bit[81]&bit[80]) | (~bit[84]&~bit[82]&~bit[81]&bit[80]) | (~bit[83]&~bit[82]&bit[81]&~bit[80]) | (~bit[84]&~bit[83]&~bit[82]&~bit[81]) | (bit[84]&~bit[83]&bit[82]&bit[81]&~bit[80]) | (~bit[83]&~bit[82]&~bit[81]&bit[80]) | (bit[83]&bit[82]&~bit[81]&~bit[80]) | (bit[83]&bit[82]&bit[81]&~bit[80]) | (~bit[84]&bit[83]&bit[82]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((bit[84]&bit[83]&bit[82]) | (bit[84]&~bit[83]&~bit[82]&~bit[81]&~bit[80]) | (~bit[84]&bit[83]&~bit[82]&bit[81]&~bit[80]) | (~bit[85]&~bit[83]&bit[82]&bit[81]&bit[80]) | (~bit[84]&~bit[83]&bit[81]&bit[80]) | (bit[84]&~bit[83]&bit[82]&~bit[81]&bit[80]) | (~bit[83]&~bit[82]&bit[81]&bit[80]) | (bit[84]&bit[83]&~bit[81]&bit[80]) | (bit[84]&~bit[83]&bit[82]&bit[81]&~bit[80]) | (bit[83]&bit[82]&~bit[81]&~bit[80]) | (bit[83]&bit[82]&bit[81]&~bit[80]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((~bit[85]&~bit[84]&bit[82]&bit[81]&bit[80]) | (bit[85]&~bit[83]&bit[82]&bit[81]&bit[80]) | (~bit[85]&bit[84]&bit[83]&bit[80]) | (~bit[84]&bit[83]&bit[82]&~bit[80]) | (bit[84]&bit[83]&~bit[82]&~bit[80]) | (bit[84]&~bit[83]&bit[82]&~bit[81]&bit[80]) | (bit[84]&bit[83]&~bit[81]&bit[80]) | (bit[84]&~bit[83]&bit[82]&bit[81]&~bit[80]) | (~bit[84]&~bit[83]&~bit[81]&bit[80]) | (~bit[83]&~bit[82]&~bit[81]&bit[80]) | (~bit[84]&~bit[83]&~bit[82]) | (~bit[84]&~bit[82]&~bit[81]&~bit[80]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((bit[84]&~bit[83]&~bit[82]&~bit[81]&~bit[80]) | (bit[85]&bit[83]&bit[82]&bit[80]) | (~bit[85]&~bit[83]&bit[82]&bit[81]&bit[80]) | (~bit[84]&bit[83]&~bit[82]&bit[81]) | (~bit[84]&~bit[83]&bit[82]&~bit[80]) | (~bit[84]&~bit[83]&~bit[82]&~bit[81]) | (bit[83]&bit[82]&~bit[81]) | (bit[84]&~bit[83]&bit[81]) | (~bit[84]&~bit[82]&~bit[81]&~bit[80]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[79]);
		control=control & bit[79];

		//-------------80--------------

		out[6] = (~disable_update & ((~bit[86]&bit[85]&bit[84]&bit[83]&bit[82]&bit[81]))) | (disable_update & out[6]);		
		out[5] = (~disable_update & ((bit[86]&bit[85]&bit[84]&bit[83]&bit[82]&bit[81]) | (~bit[82]&~bit[81]) | (~bit[83]&~bit[81]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((bit[86]&bit[85]&bit[84]&bit[83]&bit[82]&bit[81]) | (bit[83]&bit[82]&~bit[81]) | (~bit[85]&~bit[83]&~bit[82]) | (bit[85]&~bit[84]&~bit[83]&~bit[82]) | (~bit[82]&bit[81]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((bit[86]&bit[85]&bit[84]&bit[83]&bit[82]&bit[81]) | (bit[85]&~bit[84]&~bit[83]&bit[82]&bit[81]) | (~bit[85]&~bit[84]&~bit[83]&bit[81]) | (bit[85]&bit[84]&~bit[83]&~bit[82]&~bit[81]) | (~bit[85]&bit[83]&~bit[81]) | (bit[85]&bit[83]&~bit[81]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((bit[85]&~bit[83]&~bit[82]&bit[81]) | (bit[86]&bit[85]&bit[84]&bit[83]&bit[82]&bit[81]) | (~bit[85]&bit[84]&~bit[83]&bit[81]) | (bit[85]&bit[84]&~bit[83]&~bit[82]&~bit[81]) | (bit[85]&bit[84]&~bit[83]&bit[81]) | (~bit[84]&bit[82]&~bit[81]) | (~bit[85]&~bit[84]&~bit[81]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((bit[86]&bit[85]&bit[84]&bit[83]&bit[82]&bit[81]) | (~bit[85]&bit[84]&~bit[83]&bit[81]) | (bit[85]&bit[84]&~bit[83]&~bit[82]&~bit[81]) | (~bit[84]&bit[83]&~bit[82]) | (~bit[85]&~bit[84]&bit[82]) | (bit[85]&~bit[84]&~bit[83]&~bit[82]) | (~bit[85]&bit[83]&~bit[81]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((~bit[85]&bit[84]&~bit[83]&bit[82]&~bit[81]) | (bit[86]&bit[85]&bit[84]&bit[83]&bit[82]&bit[81]) | (bit[85]&~bit[84]&~bit[83]&bit[82]&bit[81]) | (~bit[85]&bit[83]&~bit[82]&bit[81]) | (bit[85]&bit[84]&~bit[83]&~bit[82]&~bit[81]) | (bit[85]&bit[84]&~bit[83]&bit[81]) | (bit[85]&~bit[84]&~bit[83]&~bit[82]) | (~bit[84]&bit[83]&bit[82]) | (bit[85]&bit[83]&~bit[81]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[80]);
		control=control & bit[80];

		//-------------81--------------

		out[6] = (~disable_update & (out[6]&0)) | (disable_update & out[6]);		
		out[5] = (~disable_update & ((bit[86]&~bit[85]&bit[84]&~bit[82]) | (bit[86]&bit[85]&bit[84]&~bit[83]&~bit[82]) | (~bit[86]&~bit[82]) | (~bit[84]&~bit[82]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((bit[86]&bit[85]&bit[84]&bit[83]&~bit[82]) | (~bit[86]&bit[85]&bit[84]&~bit[83]) | (~bit[85]&bit[84]&~bit[83]) | (~bit[83]&bit[82]) | (~bit[84]&~bit[83]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((bit[86]&bit[85]&bit[84]&bit[83]&~bit[82]) | (bit[86]&bit[85]&~bit[84]&bit[83]) | (bit[86]&bit[85]&bit[84]&~bit[83]&~bit[82]) | (bit[86]&~bit[85]&~bit[84]&~bit[83]) | (~bit[85]&~bit[84]&bit[83]) | (~bit[86]&~bit[84]) | (~bit[84]&~bit[82]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((bit[86]&bit[85]&~bit[84]&~bit[83]&bit[82]) | (bit[86]&bit[85]&bit[84]&~bit[83]&~bit[82]) | (~bit[85]&bit[84]&~bit[83]) | (~bit[86]&bit[83]&bit[82]) | (~bit[85]&~bit[84]&bit[83]) | (bit[84]&bit[83]) | (~bit[86]&~bit[85]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((~bit[86]&bit[85]&bit[84]&~bit[83]&bit[82]) | (bit[86]&~bit[85]&~bit[84]&~bit[83]&~bit[82]) | (bit[86]&bit[85]&~bit[84]&~bit[83]&bit[82]) | (bit[86]&bit[85]&bit[84]&bit[83]&~bit[82]) | (~bit[86]&bit[85]&~bit[84]&~bit[82]) | (~bit[86]&~bit[85]&bit[84]&~bit[82]) | (~bit[86]&~bit[85]&~bit[84]&bit[82]) | (bit[86]&bit[85]&bit[84]&~bit[83]&~bit[82]) | (~bit[85]&bit[83]&bit[82]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((bit[86]&bit[85]&bit[84]&bit[83]&~bit[82]) | (bit[86]&~bit[85]&bit[84]&~bit[82]) | (~bit[86]&~bit[85]&~bit[84]&bit[83]) | (bit[86]&bit[85]&~bit[84]&bit[83]) | (~bit[86]&bit[85]&bit[84]&~bit[83]) | (bit[86]&bit[85]&bit[84]&~bit[83]&~bit[82]) | (bit[86]&~bit[85]&~bit[84]&~bit[83]) | (~bit[86]&bit[84]&bit[82]) | (~bit[86]&bit[83]&bit[82]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[81]);
		control=control & bit[81];

		//-------------82--------------

		out[6] = (~disable_update & ((~bit[88]&bit[86]&bit[85]&bit[84]&bit[83]) | (~bit[87]&bit[86]&bit[85]&bit[84]&bit[83]))) | (disable_update & out[6]);		
		out[5] = (~disable_update & ((bit[88]&bit[87]&bit[86]&bit[85]&bit[84]&bit[83]) | (bit[87]&~bit[86]&bit[84]&~bit[83]) | (~bit[87]&bit[85]&~bit[84]&~bit[83]) | (~bit[87]&bit[85]&bit[84]&~bit[83]) | (bit[87]&bit[86]&~bit[85]&~bit[83]) | (~bit[87]&~bit[85]&~bit[83]) | (bit[87]&~bit[84]&~bit[83]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((bit[88]&bit[87]&bit[86]&bit[85]&bit[84]&bit[83]) | (~bit[87]&~bit[86]&bit[85]&~bit[84]) | (~bit[87]&~bit[85]&~bit[84]&bit[83]) | (bit[87]&bit[86]&bit[85]&bit[84]&~bit[83]) | (bit[87]&~bit[85]&~bit[84]) | (~bit[84]&~bit[83]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((bit[88]&bit[87]&bit[86]&bit[85]&bit[84]&bit[83]) | (~bit[87]&bit[86]&bit[85]&~bit[84]&bit[83]) | (bit[87]&bit[85]&~bit[84]&bit[83]) | (bit[87]&bit[86]&bit[85]&bit[84]&~bit[83]) | (~bit[87]&~bit[85]&~bit[83]) | (~bit[86]&~bit[85]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((bit[87]&~bit[86]&bit[85]&~bit[84]) | (bit[88]&bit[87]&bit[86]&bit[85]&bit[84]&bit[83]) | (bit[86]&~bit[85]&bit[84]&bit[83]) | (~bit[87]&bit[86]&bit[85]&~bit[84]&bit[83]) | (~bit[87]&~bit[86]&bit[85]&bit[84]) | (~bit[87]&~bit[85]&~bit[84]&bit[83]) | (~bit[87]&~bit[86]&~bit[83]) | (bit[87]&bit[86]&~bit[85]&~bit[83]) | (bit[87]&bit[86]&bit[85]&bit[84]&~bit[83]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((~bit[88]&~bit[87]&bit[86]&bit[84]&bit[83]) | (bit[88]&bit[87]&bit[86]&bit[85]&bit[84]&bit[83]) | (~bit[87]&~bit[86]&bit[85]&~bit[84]) | (bit[87]&~bit[86]&bit[84]&~bit[83]) | (~bit[87]&bit[85]&~bit[84]&~bit[83]) | (bit[87]&bit[85]&~bit[84]&bit[83]) | (~bit[86]&~bit[85]&~bit[83]) | (bit[87]&bit[86]&~bit[85]&~bit[83]) | (bit[87]&bit[86]&bit[85]&bit[84]&~bit[83]) | (~bit[87]&~bit[85]&bit[84]) | (bit[87]&~bit[85]&~bit[84]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((~bit[87]&bit[86]&bit[85]&bit[84]&bit[83]) | (bit[88]&bit[87]&bit[86]&bit[85]&bit[84]&bit[83]) | (~bit[87]&bit[86]&bit[85]&~bit[84]&bit[83]) | (~bit[86]&~bit[85]&bit[84]&~bit[83]) | (~bit[87]&bit[85]&bit[84]&~bit[83]) | (~bit[87]&~bit[85]&~bit[84]&bit[83]) | (bit[87]&bit[86]&~bit[85]) | (bit[87]&bit[85]&~bit[84]&bit[83]) | (bit[87]&bit[86]&bit[85]&bit[84]&~bit[83]) | (bit[87]&~bit[84]&~bit[83]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[82]);
		control=control & bit[82];

		//-------------83--------------

		out[6] = (~disable_update & ((bit[88]&~bit[87]&bit[86]&bit[85]&bit[84]))) | (disable_update & out[6]);		
		out[5] = (~disable_update & ((bit[87]&bit[86]&bit[85]&bit[84]) | (bit[86]&~bit[85]&~bit[84]) | (~bit[86]&~bit[84]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((~bit[88]&~bit[86]&~bit[85]) | (bit[88]&~bit[87]&~bit[86]&~bit[85]) | (bit[87]&bit[86]&bit[85]&bit[84]) | (~bit[86]&~bit[85]&bit[84]) | (bit[86]&bit[85]&~bit[84]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((bit[88]&bit[87]&~bit[86]&~bit[85]&~bit[84]) | (~bit[87]&bit[86]&~bit[84]) | (~bit[87]&bit[86]&~bit[85]&bit[84]) | (bit[87]&bit[86]&~bit[85]&bit[84]) | (~bit[88]&bit[87]&bit[86]) | (bit[86]&bit[85]&~bit[84]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((~bit[89]&~bit[88]&bit[87]&bit[86]&bit[85]&bit[84]) | (bit[88]&bit[87]&~bit[86]&~bit[85]&~bit[84]) | (bit[88]&bit[87]&bit[86]&bit[85]&bit[84]) | (bit[88]&bit[87]&bit[86]&~bit[85]&~bit[84]) | (~bit[87]&~bit[86]&bit[84]) | (~bit[87]&bit[86]&~bit[85]&bit[84]) | (~bit[87]&bit[85]&~bit[84]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((~bit[89]&~bit[88]&bit[87]&bit[86]&bit[85]&bit[84]) | (~bit[88]&~bit[87]&bit[86]&~bit[84]) | (bit[88]&~bit[87]&bit[86]&bit[85]&bit[84]) | (bit[88]&bit[87]&bit[86]&~bit[85]&~bit[84]) | (bit[87]&~bit[86]&bit[85]&bit[84]) | (~bit[88]&bit[87]&~bit[85]&bit[84]) | (~bit[88]&~bit[87]&~bit[85]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((~bit[89]&~bit[88]&bit[87]&bit[86]&bit[85]&bit[84]) | (~bit[89]&bit[88]&bit[86]&bit[85]&bit[84]) | (~bit[88]&bit[87]&bit[85]&~bit[84]) | (bit[88]&~bit[87]&~bit[86]&~bit[85]) | (bit[88]&bit[86]&~bit[85]&~bit[84]) | (~bit[88]&bit[86]&~bit[85]&bit[84]) | (~bit[88]&bit[87]&~bit[85]&bit[84]) | (~bit[88]&~bit[86]&bit[85]) | (bit[87]&bit[86]&~bit[85]&bit[84]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[83]);
		control=control & bit[83];

		//-------------84--------------

		out[6] = (~disable_update & ((~bit[90]&~bit[89]&bit[88]&bit[87]&bit[86]&bit[85]))) | (disable_update & out[6]);		
		out[5] = (~disable_update & ((bit[90]&~bit[89]&bit[88]&bit[87]&bit[86]&bit[85]) | (~bit[89]&~bit[88]&~bit[87]&~bit[85]) | (bit[89]&bit[88]&bit[87]&bit[86]&bit[85]) | (bit[88]&~bit[87]&~bit[86]&~bit[85]) | (bit[87]&~bit[86]&~bit[85]) | (~bit[88]&~bit[86]&~bit[85]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((bit[90]&~bit[89]&bit[88]&bit[87]&bit[86]&bit[85]) | (~bit[88]&bit[87]&bit[86]&~bit[85]) | (~bit[89]&~bit[87]&~bit[86]&bit[85]) | (bit[89]&bit[88]&bit[87]&bit[86]&bit[85]) | (bit[88]&~bit[87]&bit[86]&~bit[85]) | (bit[89]&~bit[88]&bit[86]&~bit[85]) | (bit[88]&bit[87]&bit[86]&~bit[85]) | (~bit[88]&~bit[87]&~bit[86]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((bit[90]&~bit[89]&bit[88]&bit[87]&bit[86]&bit[85]) | (~bit[88]&bit[87]&bit[86]&~bit[85]) | (bit[89]&bit[88]&bit[87]&bit[86]&bit[85]) | (~bit[88]&bit[87]&~bit[86]&bit[85]) | (~bit[89]&bit[87]&~bit[86]&bit[85]) | (bit[88]&~bit[87]&bit[86]&~bit[85]) | (bit[89]&~bit[88]&bit[86]&~bit[85]) | (bit[89]&bit[88]&~bit[87]&~bit[86]) | (bit[88]&~bit[87]&~bit[86]&~bit[85]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((~bit[88]&~bit[87]&bit[86]&bit[85]) | (~bit[89]&~bit[88]&bit[87]&~bit[86]) | (bit[89]&bit[88]&bit[87]&bit[86]&bit[85]) | (bit[89]&~bit[88]&~bit[87]&bit[86]) | (bit[89]&bit[88]&~bit[86]&bit[85]) | (~bit[89]&bit[88]&~bit[85]) | (~bit[89]&bit[88]&bit[86]) | (bit[87]&~bit[86]&~bit[85]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((bit[90]&~bit[89]&bit[88]&bit[87]&bit[86]&bit[85]) | (bit[89]&bit[88]&~bit[87]&bit[86]&bit[85]) | (bit[89]&bit[88]&~bit[87]&~bit[85]) | (bit[89]&bit[87]&~bit[86]&bit[85]) | (~bit[89]&~bit[88]&~bit[87]&~bit[86]) | (~bit[89]&~bit[88]&bit[86]&bit[85]) | (bit[88]&~bit[87]&~bit[86]&~bit[85]) | (bit[88]&bit[87]&bit[86]&~bit[85]) | (~bit[88]&~bit[86]&~bit[85]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((bit[89]&bit[88]&~bit[87]&bit[86]&bit[85]) | (~bit[89]&~bit[88]&~bit[86]&~bit[85]) | (bit[89]&~bit[88]&~bit[87]&bit[86]) | (bit[89]&bit[88]&~bit[87]&~bit[86]) | (~bit[90]&bit[89]&bit[85]) | (bit[89]&bit[88]&~bit[86]&bit[85]) | (bit[88]&bit[87]&bit[86]&~bit[85]) | (~bit[89]&bit[87]&~bit[85]) | (bit[89]&~bit[88]&bit[85]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[84]);
		control=control & bit[84];

		//-------------85--------------

		out[6] = (~disable_update & (out[6]&0)) | (disable_update & out[6]);		
		out[5] = (~disable_update & ((bit[90]&~bit[89]&~bit[88]&bit[87]&~bit[86]) | (~bit[90]&~bit[89]&~bit[88]&~bit[87]) | (~bit[90]&~bit[89]&bit[88]&~bit[87]&~bit[86]) | (~bit[89]&bit[88]&bit[87]&~bit[86]) | (~bit[90]&bit[89]&bit[88]&~bit[86]) | (bit[90]&bit[88]&~bit[87]&~bit[86]) | (bit[90]&bit[89]&~bit[88]&~bit[87]&~bit[86]) | (~bit[90]&~bit[89]&~bit[88]&~bit[86]) | (~bit[90]&~bit[88]&~bit[87]&~bit[86]) | (bit[89]&bit[87]&~bit[86]) | (~bit[89]&~bit[88]&~bit[87]&~bit[86]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((~bit[89]&~bit[88]&bit[87]&bit[86]) | (bit[90]&~bit[89]&~bit[88]&bit[86]) | (bit[90]&bit[89]&~bit[88]&~bit[87]&~bit[86]) | (~bit[90]&~bit[89]&~bit[88]&~bit[86]) | (bit[89]&~bit[88]&~bit[87]&bit[86]) | (~bit[90]&~bit[88]&~bit[87]&~bit[86]) | (~bit[89]&~bit[88]&~bit[87]&~bit[86]) | (bit[88]&~bit[87]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((~bit[90]&bit[89]&bit[88]&~bit[87]&bit[86]) | (bit[90]&~bit[89]&~bit[88]&bit[87]&~bit[86]) | (bit[90]&~bit[89]&~bit[88]&~bit[87]&bit[86]) | (bit[90]&~bit[89]&bit[88]&bit[86]) | (~bit[90]&~bit[89]&bit[88]&bit[86]) | (bit[89]&~bit[88]&bit[87]) | (bit[89]&~bit[88]&~bit[87]&bit[86]) | (~bit[90]&~bit[88]&~bit[87]&~bit[86]) | (~bit[89]&~bit[88]&~bit[87]&~bit[86]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((bit[90]&bit[89]&~bit[87]&bit[86]) | (bit[90]&~bit[89]&~bit[88]&bit[87]&~bit[86]) | (bit[90]&~bit[89]&~bit[88]&~bit[87]&bit[86]) | (~bit[90]&~bit[89]&bit[88]&~bit[87]&~bit[86]) | (~bit[89]&bit[88]&bit[87]&~bit[86]) | (bit[90]&bit[89]&~bit[88]&bit[86]) | (~bit[90]&~bit[89]&bit[88]&bit[86]) | (bit[90]&bit[89]&~bit[88]&~bit[87]&~bit[86]) | (~bit[90]&bit[87]&bit[86]) | (bit[89]&~bit[88]&~bit[87]&bit[86]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((~bit[90]&~bit[89]&~bit[88]&~bit[87]&~bit[86]) | (~bit[90]&bit[88]&bit[87]&~bit[86]) | (~bit[90]&bit[89]&~bit[88]&bit[86]) | (bit[90]&~bit[89]&~bit[88]&~bit[87]&bit[86]) | (bit[90]&bit[89]&bit[88]&bit[86]) | (bit[90]&~bit[89]&bit[88]&bit[86]) | (~bit[90]&bit[89]&bit[88]&~bit[86]) | (bit[90]&~bit[89]&bit[88]&~bit[87]) | (bit[90]&bit[89]&~bit[88]&~bit[87]&~bit[86]) | (bit[89]&bit[87]&~bit[86]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((~bit[90]&bit[89]&bit[88]&~bit[87]&bit[86]) | (bit[90]&~bit[89]&bit[88]&~bit[86]) | (~bit[90]&~bit[89]&bit[88]&~bit[87]&~bit[86]) | (bit[90]&bit[89]&bit[88]&bit[86]) | (bit[90]&bit[89]&~bit[88]&bit[86]) | (bit[90]&~bit[89]&~bit[88]&bit[86]) | (bit[90]&bit[88]&~bit[87]&~bit[86]) | (bit[90]&~bit[89]&bit[88]&~bit[87]) | (~bit[90]&bit[89]&bit[87]) | (~bit[89]&~bit[88]&~bit[87]&~bit[86]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[85]);
		control=control & bit[85];

		//-------------86--------------
/*
		out[6] = (~disable_update & ()) | (disable_update & out[6]);		
		out[5] = (~disable_update & ()) | (disable_update & out[5]);
		out[4] = (~disable_update & ()) | (disable_update & out[4]);
		out[3] = (~disable_update & ()) | (disable_update & out[3]);
		out[2] = (~disable_update & ()) | (disable_update & out[2]);
		out[1] = (~disable_update & ()) | (disable_update & out[1]);
		out[0] = (~disable_update & ()) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[45]);
		control=control & bit[45];
*/
		out[6] = (~disable_update & ((~bit[91]&~bit[90]&~bit[89]&~bit[88]&~bit[87]))) | (disable_update & out[6]);		
		out[5] = (~disable_update & ((~bit[91]&bit[90]&bit[88]&~bit[87]) | (bit[90]&~bit[88]&~bit[87]) | (~bit[90]&bit[88]&~bit[87]) | (bit[89]&~bit[87]) | (bit[91]&~bit[89]&~bit[87]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((bit[91]&~bit[90]&~bit[89]&~bit[88]) | (bit[90]&~bit[88]&~bit[87]) | (~bit[89]&~bit[88]&bit[87]) | (~bit[91]&bit[89]&~bit[88]) | (~bit[90]&bit[89]&~bit[88]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((bit[91]&~bit[90]&~bit[89]&bit[88]&bit[87]) | (bit[91]&bit[90]&bit[89]&~bit[88]&bit[87]) | (~bit[89]&~bit[88]&bit[87]) | (~bit[91]&bit[90]&~bit[89]) | (~bit[91]&~bit[89]&bit[88]) | (bit[91]&~bit[89]&~bit[87]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((bit[91]&bit[90]&~bit[89]&bit[88]&bit[87]) | (bit[91]&~bit[90]&bit[89]&bit[88]&bit[87]) | (~bit[91]&bit[89]&bit[88]&bit[87]) | (bit[90]&~bit[89]&~bit[88]&~bit[87]) | (bit[91]&~bit[90]&~bit[89]&~bit[88]) | (bit[91]&bit[90]&bit[89]&~bit[88]&bit[87]) | (~bit[90]&bit[88]&~bit[87]) | (~bit[91]&~bit[90]&bit[87]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((bit[91]&~bit[90]&~bit[89]&bit[88]&bit[87]) | (bit[91]&bit[90]&bit[88]&bit[87]) | (bit[91]&bit[90]&bit[89]&~bit[88]&bit[87]) | (~bit[91]&~bit[90]&bit[89]&~bit[87]) | (~bit[90]&~bit[88]&~bit[87]) | (~bit[91]&bit[90]&~bit[89]) | (~bit[90]&bit[89]&~bit[88]) | (~bit[91]&~bit[89]&bit[88]) | (~bit[91]&~bit[90]&bit[87]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((bit[91]&~bit[90]&bit[89]&bit[88]&bit[87]) | (bit[91]&~bit[90]&~bit[89]&bit[88]&bit[87]) | (~bit[91]&~bit[90]&~bit[88]&bit[87]) | (~bit[91]&bit[90]&bit[88]&~bit[87]) | (bit[91]&bit[90]&bit[88]&bit[87]) | (bit[91]&bit[90]&bit[89]&~bit[88]&bit[87]) | (~bit[91]&~bit[90]&bit[89]&~bit[87]) | (~bit[91]&bit[89]&~bit[88]) | (bit[91]&~bit[89]&~bit[87]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[86]);
		control=control & bit[86];

		//-------------87--------------

		out[6] = (~disable_update & ((~bit[92]&bit[91]&~bit[90]&~bit[89]&~bit[88]))) | (disable_update & out[6]);		
		out[5] = (~disable_update & ((~bit[92]&bit[91]&~bit[90]&bit[89]&~bit[88]) | (~bit[91]&~bit[90]&bit[89]&~bit[88]) | (~bit[91]&bit[90]&bit[89]&~bit[88]) | (bit[92]&bit[91]&~bit[90]&~bit[88]) | (~bit[92]&bit[90]&~bit[88]) | (bit[92]&bit[90]&~bit[89]&~bit[88]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((~bit[92]&~bit[91]&~bit[90]&bit[88]) | (~bit[92]&~bit[91]&bit[90]&~bit[89]&bit[88]) | (~bit[92]&~bit[91]&bit[90]&~bit[89]&~bit[88]) | (bit[92]&bit[91]&bit[90]&bit[89]&~bit[88]) | (bit[92]&bit[91]&~bit[90]&~bit[89]&~bit[88]) | (~bit[92]&~bit[90]&~bit[89]&bit[88]) | (bit[92]&~bit[89]&bit[88]) | (~bit[92]&bit[91]&bit[90]&~bit[89]) | (bit[92]&~bit[91]&bit[90]&~bit[89]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((~bit[92]&~bit[91]&bit[90]&bit[89]&bit[88]) | (bit[92]&~bit[91]&~bit[90]&~bit[89]&bit[88]) | (~bit[92]&bit[91]&~bit[90]&bit[89]&~bit[88]) | (bit[92]&~bit[90]&bit[89]&bit[88]) | (~bit[92]&bit[91]&~bit[90]&bit[89]&bit[88]) | (~bit[92]&~bit[91]&bit[90]&~bit[89]&~bit[88]) | (bit[92]&bit[91]&~bit[90]&~bit[89]&~bit[88]) | (~bit[91]&~bit[90]&bit[89]&~bit[88]) | (bit[92]&bit[91]&bit[90]&~bit[88]) | (~bit[92]&~bit[90]&~bit[89]&bit[88]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((bit[92]&bit[91]&~bit[90]&~bit[89]&bit[88]) | (~bit[92]&bit[91]&bit[90]&bit[89]&bit[88]) | (~bit[92]&~bit[91]&~bit[90]&~bit[89]&bit[88]) | (~bit[92]&~bit[91]&bit[89]&~bit[88]) | (bit[92]&~bit[91]&bit[89]&bit[88]) | (~bit[92]&~bit[91]&bit[90]&~bit[89]&bit[88]) | (~bit[92]&~bit[91]&bit[90]&~bit[89]&~bit[88]) | (bit[92]&bit[91]&bit[90]&bit[89]&~bit[88]) | (bit[92]&bit[91]&~bit[90]&~bit[88]) | (bit[92]&bit[90]&~bit[89]&~bit[88]) | (bit[92]&~bit[91]&bit[90]&~bit[89]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((~bit[92]&bit[91]&~bit[90]&~bit[89]&~bit[88]) | (bit[92]&~bit[91]&~bit[90]&~bit[89]&bit[88]) | (bit[92]&bit[91]&~bit[90]&~bit[89]&bit[88]) | (~bit[92]&~bit[91]&bit[90]&~bit[89]&bit[88]) | (~bit[92]&bit[91]&~bit[90]&bit[89]&bit[88]) | (bit[92]&bit[91]&bit[90]&~bit[88]) | (bit[92]&bit[90]&bit[89]) | (bit[92]&bit[89]&~bit[88]) | (~bit[92]&bit[91]&bit[90]&~bit[89]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((~bit[92]&bit[91]&bit[90]&bit[89]&bit[88]) | (~bit[92]&~bit[91]&~bit[90]&~bit[89]&bit[88]) | (~bit[92]&bit[91]&~bit[90]&bit[89]&~bit[88]) | (~bit[92]&bit[91]&~bit[90]&bit[89]&bit[88]) | (~bit[91]&~bit[90]&~bit[89]&~bit[88]) | (bit[92]&bit[91]&bit[90]&bit[89]&~bit[88]) | (bit[92]&bit[91]&~bit[90]&~bit[89]&~bit[88]) | (~bit[91]&bit[90]&bit[89]&~bit[88]) | (~bit[92]&bit[91]&bit[90]&~bit[89]) | (bit[92]&bit[91]&bit[88]) | (bit[92]&bit[90]&~bit[89]&~bit[88]) | (bit[92]&~bit[91]&bit[90]&~bit[89]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[87]);
		control=control & bit[87];

		//-------------88--------------

		out[6] = (~disable_update & ((~bit[93]&bit[91]&~bit[90]&~bit[89]) | (~bit[92]&bit[91]&~bit[90]&~bit[89]))) | (disable_update & out[6]);		
		out[5] = (~disable_update & ((~bit[93]&~bit[92]&~bit[91]&~bit[90]&bit[89]) | (bit[93]&bit[92]&bit[90]&~bit[89]) | (~bit[93]&~bit[92]&~bit[91]&bit[90]&~bit[89]) | (~bit[93]&bit[92]&bit[90]&~bit[89]) | (bit[93]&bit[92]&bit[91]&~bit[90]&~bit[89]) | (~bit[92]&bit[91]&bit[90]&~bit[89]) | (bit[93]&~bit[92]&~bit[91]&bit[90]&~bit[89]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((~bit[93]&~bit[92]&~bit[91]&bit[90]&~bit[89]) | (~bit[92]&~bit[91]&bit[90]&bit[89]) | (bit[93]&~bit[92]&~bit[91]&~bit[90]&bit[89]) | (bit[93]&bit[92]&bit[91]&~bit[90]&~bit[89]) | (~bit[92]&bit[91]&~bit[90]&bit[89]) | (bit[92]&~bit[90]&bit[89]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((~bit[93]&bit[92]&~bit[91]&bit[89]) | (bit[93]&~bit[92]&~bit[91]&~bit[90]&bit[89]) | (bit[93]&bit[92]&bit[91]&~bit[90]&~bit[89]) | (~bit[92]&bit[91]&bit[90]&~bit[89]) | (bit[92]&~bit[91]&bit[90]) | (bit[93]&~bit[92]&~bit[91]&bit[90]&~bit[89]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((bit[93]&bit[92]&~bit[91]&~bit[90]&bit[89]) | (~bit[93]&~bit[92]&~bit[91]&bit[90]&~bit[89]) | (~bit[93]&~bit[92]&bit[91]&~bit[90]) | (bit[93]&~bit[92]&~bit[91]&~bit[90]&bit[89]) | (~bit[93]&bit[92]&bit[90]&~bit[89]) | (bit[93]&bit[92]&bit[91]&~bit[90]&~bit[89]) | (~bit[93]&~bit[92]&bit[91]&bit[89]) | (bit[93]&~bit[92]&~bit[91]&bit[90]&~bit[89]) | (~bit[92]&bit[91]&~bit[90]&bit[89]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((bit[92]&~bit[91]&bit[89]) | (bit[93]&bit[92]&bit[90]&~bit[89]) | (~bit[92]&bit[91]&bit[90]&bit[89]) | (bit[93]&~bit[92]&~bit[91]&~bit[90]&bit[89]) | (bit[93]&~bit[92]&~bit[91]&bit[90]&~bit[89]) | (bit[92]&~bit[90]&bit[89]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((bit[93]&bit[92]&~bit[91]&~bit[90]&bit[89]) | (bit[93]&bit[92]&bit[91]&~bit[90]&~bit[89]) | (bit[93]&~bit[92]&~bit[91]&bit[90]&~bit[89]) | (~bit[92]&~bit[90]&~bit[89]) | (~bit[93]&bit[90]&bit[89]) | (~bit[93]&bit[91]&bit[89]) | (~bit[93]&~bit[92]&bit[91]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[88]);
		control=control & bit[88];

		//-------------89--------------

		out[6] = (~disable_update & ((~bit[95]&~bit[94]&bit[93]&bit[92]&bit[91]&bit[90]) | (~bit[94]&~bit[93]&~bit[92]&~bit[91]&~bit[90]))) | (disable_update & out[6]);		
		out[5] = (~disable_update & ((~bit[95]&bit[94]&bit[93]&bit[92]&bit[91]&bit[90]) | (~bit[94]&~bit[93]&~bit[92]&bit[91]&~bit[90]) | (~bit[94]&~bit[93]&bit[92]&~bit[91]&~bit[90]) | (bit[93]&~bit[92]&bit[91]&~bit[90]) | (bit[95]&bit[93]&bit[92]&bit[91]&bit[90]) | (bit[94]&~bit[93]&~bit[92]&~bit[90]) | (bit[94]&~bit[91]&~bit[90]) | (~bit[94]&bit[93]&~bit[91]&~bit[90]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((~bit[95]&bit[94]&bit[93]&bit[92]&bit[91]&bit[90]) | (bit[94]&bit[93]&~bit[92]&~bit[91]&bit[90]) | (~bit[94]&~bit[93]&~bit[92]&bit[91]&~bit[90]) | (bit[95]&bit[93]&bit[92]&bit[91]&bit[90]) | (~bit[93]&~bit[92]&~bit[91]&bit[90]) | (~bit[94]&bit[93]&~bit[92]&~bit[91]) | (bit[94]&~bit[91]&~bit[90]) | (~bit[93]&bit[92]&~bit[90]) | (~bit[94]&bit[93]&~bit[91]&~bit[90]) | (bit[92]&bit[91]&~bit[90]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((~bit[95]&bit[94]&bit[93]&bit[92]&bit[91]&bit[90]) | (~bit[94]&~bit[93]&bit[92]&~bit[91]&~bit[90]) | (bit[93]&~bit[92]&~bit[91]&~bit[90]) | (~bit[94]&~bit[93]&~bit[92]&bit[90]) | (~bit[93]&bit[92]&~bit[91]&bit[90]) | (bit[95]&bit[93]&bit[92]&bit[91]&bit[90]) | (~bit[93]&~bit[92]&~bit[91]&bit[90]) | (bit[93]&bit[92]&~bit[91]&bit[90]) | (bit[94]&~bit[93]&~bit[92]&~bit[90]) | (bit[92]&bit[91]&~bit[90]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((bit[94]&~bit[93]&~bit[92]&bit[91]&bit[90]) | (~bit[95]&bit[94]&bit[93]&bit[92]&bit[91]&bit[90]) | (~bit[94]&bit[93]&bit[92]&bit[91]) | (~bit[93]&bit[92]&~bit[91]&bit[90]) | (~bit[94]&bit[93]&~bit[92]&~bit[90]) | (~bit[94]&bit[93]&~bit[92]&~bit[91]) | (bit[94]&~bit[91]&~bit[90]) | (~bit[94]&bit[93]&~bit[91]&~bit[90]) | (bit[92]&bit[91]&~bit[90]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((bit[94]&~bit[93]&~bit[92]&bit[91]&bit[90]) | (~bit[94]&~bit[93]&~bit[92]&~bit[91]&~bit[90]) | (~bit[95]&bit[94]&bit[93]&bit[92]&bit[91]&bit[90]) | (bit[93]&~bit[92]&bit[91]&bit[90]) | (bit[95]&bit[93]&bit[92]&bit[91]&bit[90]) | (~bit[94]&bit[93]&~bit[92]&~bit[90]) | (~bit[94]&bit[92]&~bit[91]&bit[90]) | (bit[93]&bit[92]&~bit[91]&bit[90]) | (bit[94]&~bit[93]&~bit[92]&~bit[90]) | (~bit[93]&bit[92]&~bit[90]) | (~bit[94]&bit[93]&~bit[91]&~bit[90]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((bit[94]&bit[93]&~bit[92]&~bit[91]&bit[90]) | (~bit[94]&~bit[93]&~bit[91]&bit[90]) | (bit[93]&~bit[92]&bit[91]&~bit[90]) | (bit[95]&bit[93]&bit[92]&bit[91]&bit[90]) | (~bit[94]&bit[92]&~bit[91]&bit[90]) | (~bit[94]&bit[93]&bit[91]) | (~bit[93]&bit[92]&~bit[91]) | (~bit[94]&~bit[93]&bit[92]) | (bit[94]&~bit[93]&~bit[92]&~bit[90]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[89]);
		control=control & bit[89];

		//-------------90--------------

		out[6] = (~disable_update & ((bit[96]&bit[95]&~bit[94]&bit[93]&bit[92]&bit[91]) | (~bit[96]&bit[95]&~bit[94]&bit[93]&bit[92]&bit[91]))) | (disable_update & out[6]);		
		out[5] = (~disable_update & ((bit[95]&bit[94]&~bit[93]&~bit[91]) | (bit[95]&~bit[94]&~bit[92]&~bit[91]) | (~bit[94]&~bit[93]&bit[92]&~bit[91]) | (bit[94]&bit[93]&bit[92]&bit[91]) | (~bit[94]&bit[93]&~bit[92]&~bit[91]) | (~bit[95]&~bit[93]&~bit[91]) | (bit[94]&bit[93]&~bit[92]&~bit[91]))) | (disable_update & out[5]);
		out[4] = (~disable_update & ((bit[94]&bit[93]&bit[92]&bit[91]) | (~bit[95]&~bit[94]&~bit[92]) | (bit[93]&bit[92]&~bit[91]) | (~bit[93]&~bit[92]))) | (disable_update & out[4]);
		out[3] = (~disable_update & ((bit[95]&~bit[94]&~bit[92]&~bit[91]) | (~bit[95]&~bit[94]&~bit[93]&~bit[92]) | (bit[95]&bit[93]&~bit[92]&bit[91]) | (bit[94]&bit[93]&bit[92]&bit[91]) | (bit[94]&bit[93]&~bit[92]&~bit[91]) | (~bit[94]&~bit[93]&bit[91]) | (bit[93]&bit[92]&~bit[91]) | (~bit[95]&bit[94]&bit[93]))) | (disable_update & out[3]);
		out[2] = (~disable_update & ((~bit[96]&bit[95]&~bit[94]&bit[93]&bit[92]&bit[91]) | (bit[95]&~bit[94]&bit[93]&~bit[92]) | (~bit[94]&~bit[93]&bit[92]&~bit[91]) | (~bit[95]&~bit[94]&bit[92]&~bit[91]) | (bit[95]&~bit[94]&bit[93]&~bit[91]) | (~bit[94]&bit[93]&~bit[92]&~bit[91]) | (bit[94]&~bit[93]&bit[91]) | (bit[94]&~bit[93]&~bit[92]) | (~bit[95]&bit[94]&bit[93]))) | (disable_update & out[2]);
		out[1] = (~disable_update & ((bit[95]&bit[94]&~bit[92]&bit[91]) | (bit[95]&~bit[94]&bit[93]&~bit[91]) | (~bit[95]&bit[94]&~bit[93]) | (~bit[95]&bit[92]&bit[91]) | (bit[95]&bit[93]&bit[92]) | (~bit[94]&bit[93]&bit[92]))) | (disable_update & out[1]);
		out[0] = (~disable_update & ((bit[96]&bit[95]&~bit[94]&bit[93]&bit[92]&bit[91]) | (~bit[96]&bit[94]&bit[93]&bit[92]&bit[91]) | (~bit[95]&~bit[93]&~bit[92]&bit[91]) | (bit[95]&bit[94]&~bit[93]&~bit[91]) | (~bit[95]&~bit[94]&bit[93]&bit[91]) | (~bit[95]&~bit[94]&bit[92]&~bit[91]) | (~bit[95]&~bit[94]&~bit[93]&~bit[92]) | (bit[95]&bit[93]&~bit[92]&bit[91]) | (bit[94]&bit[93]&~bit[92]&~bit[91]))) | (disable_update & out[0]);

		disable_update= disable_update | (control & ~bit[90]);
		control=control & bit[90];

		//-------------91--------------


		for(k=0;k<64;k++){
			sample[k]=0;
			sample[k]=(out[5])&1;
			sample[k]=(sample[k]<<1)|((out[4])&1);
			sample[k]=(sample[k]<<1)|((out[3])&1);
			sample[k]=(sample[k]<<1)|((out[2])&1);
			sample[k]=(sample[k]<<1)|((out[1])&1);
			sample[k]=(sample[k]<<1)|((out[0])&1);

			out[5]=out[5]>>1;			
			out[4]=out[4]>>1;			
			out[3]=out[3]>>1;
			out[2]=out[2]>>1;
			out[1]=out[1]>>1;
			out[0]=out[0]>>1;

		}
		clock2 = cpucycles();

		clock3=clock3+(clock2-clock1);

//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------

		

//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------

		//printf("%lld%lld%lld%lld%lld%lld%lld%lld \n",bit[7]&1,bit[6]&1,bit[5]&1,bit[4]&1,out[3]&1,out[2]&1,out[1]&1,out[0]&1);
		printf("%ld %lld : %lld %lld %lld %lld %lld %lld\n",i,clock3,out[5]&1,out[4]&1,out[3]&1,out[2]&1,out[1]&1,out[0]&1);
		for(k=0;k<64;k++){
			hist[sample[k]]++;
			printf("  %lld  ,",sample[k]);
		}
		printf("\n %ld %lld\n",i,clock3);

		if(disable_update!=0xffffffffffffffff)//even if the sample is not found within depth 40
			break;
	}

	printf("Time : %lld\n",clock3/repeat);
	//printf("\nbit : %d\n",lsb);
//---------------------------print the histogram------------------------------
	for(i=0;i<84;i++)
		printf(" %ld : %llu",i,hist[i]);
//--------------------------print the histogram ends--------------------------

	printf("\n");
	
	return 1;
}








