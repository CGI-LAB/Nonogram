#ifndef _LINEMASK_H_
#define _LINEMASK_H_
#ifdef USE_SSE
#include <emmintrin.h>
#endif
#include <cstdio>

#define LINEMASK_SIZE 64

#ifdef USE_SSE
//SSE operations
#define SSE_LOAD(x) _mm_load_si128(x)
#define SSE_STORE(x, y) _mm_store_si128(x, y)

#define SSE_OR(x, y) _mm_or_si128( SSE_LOAD(x), SSE_LOAD(y))
#define SSE_AND(x, y) _mm_and_si128( SSE_LOAD(x), SSE_LOAD(y))
#define SSE_XOR(x, y) _mm_xor_si128( SSE_LOAD(x), SSE_LOAD(y))
#endif

//DEBUG
#define PRINT_BIT(x) (x.print(#x))

///////////////////////////////////////////////////////////////////////////////
// 128-bit linemask

class LineMask_128{
public:
	//union type data
	typedef union{
#ifdef USE_SSE
        __m128i v;
#endif
		unsigned long long int n[2];
    } bitmask;

	//member variable
	bitmask mBit;

	//constructor
	LineMask_128(){
		mBit.n[0] = 0;
		mBit.n[1] = 0;
	}

	//constructor 2
	LineMask_128(unsigned long long int high, unsigned long long int low){
		mBit.n[0] = low;
		mBit.n[1] = high;
	}

	//constructor 3
	//set the index "idx" square to be "mask"
	LineMask_128( int idx, int mask ){
		mBit.n[0] = 0;
		mBit.n[1] = 0;
		*(mBit.n + (idx >> 5)) = (unsigned long long int)mask << ((idx & 31) << 1);
	}

	inline void clear_0(){
		mBit.n[0] = 0;
		mBit.n[1] = 0;
	}

	inline void clear_1(){
		mBit.n[0] = (unsigned long long int) -1 ;
		mBit.n[1] = (unsigned long long int) -1 ;
	}

	//assign n[0] = number, n[1] = number
	//inline const LineMask_128 operator=(const LineMask_128 other) {
	//	(*this).mBit.n[0] = other.mBit.n[0];
	//	(*this).mBit.n[1] = other.mBit.n[1];
	//	return *this;
	//}
	
	//overload bitwise NOT
	inline LineMask_128 operator~()
	{
		LineMask_128 result = (*this);
		result.mBit.n[0] = ~result.mBit.n[0];
		result.mBit.n[1] = ~result.mBit.n[1];
		return result;
	}
	
	//overload MOD, but NOOOOOOOT really MOD, just used in HASH
	inline const unsigned long long int operator%(const unsigned long long int& number) const
	{
		//the formula of MOD in BigInt is shown as follow:
		//A % B = (AH * 2^64 + AL) % B = (((AH % B) * ((2^64 - B) % B)) + (AL % B)) % B
		return ((int)((*this).mBit.n[0] % number + (*this).mBit.n[1] % number) % number);
	}

	//overload compound assignment operators OR
	inline LineMask_128& operator|=(const LineMask_128& other) 
	{
#ifdef USE_SSE
		SSE_STORE(&(*this).mBit.v, SSE_OR(&(*this).mBit.v, &other.mBit.v));
#else
		(*this).mBit.n[0] |=other.mBit.n[0];
		(*this).mBit.n[1] |=other.mBit.n[1];
#endif
		 return *this;
	}

	//overload compound assignment operators AND
	inline LineMask_128& operator&=(const LineMask_128& other) 
	{
#ifdef USE_SSE
		SSE_STORE(&(*this).mBit.v, SSE_AND(&(*this).mBit.v, &other.mBit.v));
#else
		(*this).mBit.n[0] &=other.mBit.n[0];
		(*this).mBit.n[1] &=other.mBit.n[1];
#endif
		 return *this;
	}

	//overload compound assignment operators AND
	inline LineMask_128& operator^=(const LineMask_128& other) 
	{
#ifdef USE_SSE
		SSE_STORE(&(*this).mBit.v, SSE_XOR(&(*this).mBit.v, &other.mBit.v));
#else
		(*this).mBit.n[0] ^=other.mBit.n[0];
		(*this).mBit.n[1] ^=other.mBit.n[1];
#endif
		 return *this;
	}

	//overload bitwise OR
	inline const LineMask_128 operator|(const LineMask_128& other) const
	{
		LineMask_128 result = (*this);
		result |= other;
		return result;
	}

	//overload bitwise AND
	inline const LineMask_128 operator&(const LineMask_128& other) const
	{
		LineMask_128 result = (*this);
		result &= other;
		return result;
	}

	//overload bitwise XOR
	inline const LineMask_128 operator^(const LineMask_128& other) const
	{
		LineMask_128 result = (*this);
		result ^= other;
		return result;
	}

	//overload comparison operator ==
	inline bool operator==(const LineMask_128& other) const {
		return (((*this).mBit.n[0] == other.mBit.n[0]) && ((*this).mBit.n[1] == other.mBit.n[1]));		
	}

	//overload comparison operator !=
	inline bool operator!=(const LineMask_128& other) const {
		return (((*this).mBit.n[0] != other.mBit.n[0]) || ((*this).mBit.n[1] != other.mBit.n[1]));		
	}

	//overload index operator [], which returns 2 bit value of corresponding square
	//ex, 11001000, if we want the square of index 1, then s[1] = "10" = 2.
	inline int operator[](const int idx) const
	{
		return *(mBit.n + (idx >> 5)) >> ((idx & 31) << 1) & 3;
	}


	//For debugging
	inline void print(){
		bool bIsOne;
		for( int i = 1;i >= 0; i-- ){
			printf("(part: %d) ", 2 - i);
			for(int j = 63; j >= 0; j--){
				bIsOne = ((( (unsigned long long int) 1 << j ) & mBit.n[i]) != 0);
				printf("%d", bIsOne ? 1 : 0);
			}
			printf("\n");
		}
		printf("\n");
	}
	//For debugging with variable name
	inline void print( char* name ){
		printf("(variable: %s)\n", name);
		print();
	}
	inline void setBit(int idx){
		*(mBit.n + (idx >> 6)) |= (unsigned long long int)1 << (idx & 63);
	}
};


///////////////////////////////////////////////////////////////////////////////
// For metaprogramming

template <int T, int MASK>
struct template_LineMask {
    template_LineMask<T - 1, MASK> rest;
	LineMask_128 value;
    template_LineMask() { value = LineMask_128(T - 1, MASK); }
};

template <int MASK>
struct template_LineMask<1, MASK> {
    LineMask_128 value;
    template_LineMask(){ value = LineMask_128(0, MASK); }
};

///////////////////////////////////////////////////////////////////////////////
// Square mask table
static template_LineMask<LINEMASK_SIZE, 0> template_error_square_mask;
static template_LineMask<LINEMASK_SIZE, 1> template_black_square_mask;
static template_LineMask<LINEMASK_SIZE, 2> template_white_square_mask;
static template_LineMask<LINEMASK_SIZE, 3> template_unknown_square_mask;

// "00", ex: 0000000000
static LineMask_128 *ERROR_SQUARE_MASK = reinterpret_cast<LineMask_128*>(&template_error_square_mask);
// "01", ex: 0000000100
static LineMask_128 *BLACK_SQUARE_MASK = reinterpret_cast<LineMask_128*>(&template_black_square_mask);
// "10", ex: 0000001000
static LineMask_128 *WHITE_SQUARE_MASK = reinterpret_cast<LineMask_128*>(&template_white_square_mask);
// "11", ex: 0000001100
static LineMask_128 *UNKNOWN_SQUARE_MASK = reinterpret_cast<LineMask_128*>(&template_unknown_square_mask);

static LineMask_128* SQUARE_MASK[4] = {ERROR_SQUARE_MASK, BLACK_SQUARE_MASK, WHITE_SQUARE_MASK, UNKNOWN_SQUARE_MASK};

static LineMask_128 ZERO;

void initialLineMask( );

#endif