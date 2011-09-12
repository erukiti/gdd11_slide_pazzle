#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* 
   A C-program for MT19937, with initialization improved 2002/1/26.
   Coded by Takuji Nishimura and Makoto Matsumoto.

   Before using, initialize the state by using init_genrand(seed)  
   or init_by_array(init_key, key_length).

   Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
   All rights reserved.                          

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

     1. Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.

     2. Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

     3. The names of its contributors may not be used to endorse or promote 
        products derived from this software without specific prior written 
        permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


   Any feedback is very welcome.
   http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/emt.html
   email: m-mat @ math.sci.hiroshima-u.ac.jp (remove space)
*/

#include <stdio.h>

/* Period parameters */  
#define N 624
#define M 397
#define MATRIX_A 0x9908b0dfUL   /* constant vector a */
#define UPPER_MASK 0x80000000UL /* most significant w-r bits */
#define LOWER_MASK 0x7fffffffUL /* least significant r bits */

static unsigned long mt[N]; /* the array for the state vector  */
static int mti=N+1; /* mti==N+1 means mt[N] is not initialized */

/* initializes mt[N] with a seed */
void init_genrand(unsigned long s)
{
    mt[0]= s & 0xffffffffUL;
    for (mti=1; mti<N; mti++) {
        mt[mti] = 
	    (1812433253UL * (mt[mti-1] ^ (mt[mti-1] >> 30)) + mti); 
        /* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
        /* In the previous versions, MSBs of the seed affect   */
        /* only MSBs of the array mt[].                        */
        /* 2002/01/09 modified by Makoto Matsumoto             */
        mt[mti] &= 0xffffffffUL;
        /* for >32 bit machines */
    }
}

/* initialize by an array with array-length */
/* init_key is the array for initializing keys */
/* key_length is its length */
/* slight change for C++, 2004/2/26 */
void init_by_array(unsigned long init_key[], int key_length)
{
    int i, j, k;
    init_genrand(19650218UL);
    i=1; j=0;
    k = (N>key_length ? N : key_length);
    for (; k; k--) {
        mt[i] = (mt[i] ^ ((mt[i-1] ^ (mt[i-1] >> 30)) * 1664525UL))
          + init_key[j] + j; /* non linear */
        mt[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
        i++; j++;
        if (i>=N) { mt[0] = mt[N-1]; i=1; }
        if (j>=key_length) j=0;
    }
    for (k=N-1; k; k--) {
        mt[i] = (mt[i] ^ ((mt[i-1] ^ (mt[i-1] >> 30)) * 1566083941UL))
          - i; /* non linear */
        mt[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
        i++;
        if (i>=N) { mt[0] = mt[N-1]; i=1; }
    }

    mt[0] = 0x80000000UL; /* MSB is 1; assuring non-zero initial array */ 
}

/* generates a random number on [0,0xffffffff]-interval */
unsigned long genrand_int32(void)
{
    unsigned long y;
    static unsigned long mag01[2]={0x0UL, MATRIX_A};
    /* mag01[x] = x * MATRIX_A  for x=0,1 */

    if (mti >= N) { /* generate N words at one time */
        int kk;

        if (mti == N+1)   /* if init_genrand() has not been called, */
            init_genrand(5489UL); /* a default initial seed is used */

        for (kk=0;kk<N-M;kk++) {
            y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
            mt[kk] = mt[kk+M] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        for (;kk<N-1;kk++) {
            y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
            mt[kk] = mt[kk+(M-N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        y = (mt[N-1]&UPPER_MASK)|(mt[0]&LOWER_MASK);
        mt[N-1] = mt[M-1] ^ (y >> 1) ^ mag01[y & 0x1UL];

        mti = 0;
    }
  
    y = mt[mti++];

    /* Tempering */
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680UL;
    y ^= (y << 15) & 0xefc60000UL;
    y ^= (y >> 18);

    return y;
}

#define BOARD_INDEX_BIT 23
#define BOARD_INDEX_SIZE (1 << BOARD_INDEX_BIT)
#define BOARD_POOL_BIT 26
#define BOARD_POOL_SIZE (1 << BOARD_POOL_BIT)

struct movestate {
	short depth; // 16bit
	char index; // 8bit
	char is_answer; // 8bit
	struct movestate *next; // 32bit
	unsigned long long hash; // 64bit
};



#define MAXTIMES 300
char $answer[MAXTIMES + 1000 + 1];
int $width;
int $height;
int $maxnum;
unsigned long long *$hash_seed;
int $wall[8 * 8];
int $n_wall;
#define check_wall(x, y) ($wall[(x) + 1 + ((y) + 1) * ($width + 2)] != 0)


struct movestate *$board_index;
struct movestate *$board_pool;
struct movestate *$board_pool_ptr;

struct movestate *$board_hash_initial;
struct movestate *$board_hash_final;

int chr_to_index(char chr) {
	if (chr >= '0' && chr <= '9') {
		return chr - '0';
	} else if (chr >= 'A' && chr <= 'Z') {
		return chr - 'A' + 10;
	}

	printf("E: chr_to_index error %x %c\n", chr, chr);
	exit;
}

unsigned long long hash_from_board(const char board[36 + 1]) {
	unsigned long long hash = 0;
	int max = strlen(board);
	for (int i = 0; i < max; i++) {
		if (board[i] != '=') {
			hash ^= $hash_seed[i * $maxnum + chr_to_index(board[i])];
		}
	}

	return hash;
}

struct movestate *board_hash_set(unsigned long long hash, int depth, int index, int is_answer) {
	int higher_hash = (int)(hash >> (64 - BOARD_INDEX_BIT));
	struct movestate *p;

	int cnt = 0;
	p = &$board_index[higher_hash];
	while (1) {
		if (p->next == NULL) {
			if ($board_pool_ptr >= $board_pool + BOARD_POOL_SIZE) {
				puts("G: giveup board pool is over");
				exit(-1);
			}

			p->depth = depth;
			p->index = index;
			p->hash = hash;
			p->is_answer = is_answer;
			p->next = $board_pool_ptr++;
			break;
		}

		if (p->hash == hash) {
			printf("D: warning hash reset: %d -> %d\n", p->depth, depth);
//			p->depth = depth;
//			p->index = index;
			break;
		}

		p = p->next;
		cnt++;
	} 

//printf("D: set %016llx, %x (%d)\n", hash, higher_hash, cnt);
	return p;
}

struct movestate *board_hash_get(unsigned long long hash) {
	int higher_hash = (int)(hash >> (64 - BOARD_INDEX_BIT));
	struct movestate *p;
//	unsigned long long higher_hash = hash >> (64 - BOARD_INDEX_BIT);

	p = &$board_index[higher_hash];
	while (p ->next != NULL && p->hash != hash) {
		p = p->next;
	}
	return p;
}

unsigned long long move_hash(char board[36 + 1], unsigned long long hash, int index, int foward) {
	hash ^= $hash_seed[index * $maxnum + 0];
	hash ^= $hash_seed[foward * $maxnum + chr_to_index(board[foward])];
	hash ^= $hash_seed[foward * $maxnum + 0];
	hash ^= $hash_seed[index * $maxnum + chr_to_index(board[foward])];
	return hash;
}

void get_answer_r(char board[36 + 1], int depth, unsigned long long hash, int index, int foward, char *answer) {
	while (depth > 0) {
		printf("D: %d => %d [%s]\n", index, foward, board);
		if (index + 1 == foward) {
			strcat(answer, "R");
		} else if (index - 1 == foward) {
			strcat(answer, "L");
		} else if (index + $width == foward) {
			strcat(answer, "D");
		} else if (index - $width == foward) {
			strcat(answer, "U");
		} else {
			puts("E: solving BUGS");
			exit(-1);
		}

		hash = move_hash(board, hash, index, foward);
		board[index] = board[foward];
		board[foward] = '0';

		index = foward;
		struct movestate *p = board_hash_get(hash);
		foward = p->index;
		depth -= 1;

//printf("D: *%d %d\n", p->depth, p->index);
	}
}

struct stack {
	char foward;
	char index;
	unsigned long long hash;
	char board[36 + 1];
};

void move_r(char board_2[36 + 1], int depth, unsigned long long hash, int index, int foward, struct stack *nextstack, int *stackcount) {
//	printf("D: %d:%d %d -> %d [%s]\n", depth, *stackcount, index, foward, board_2);

	char board[36 + 1];
	strcpy(board, board_2);

	hash = move_hash(board, hash, index, foward);
	board[index] = board[foward];
	board[foward] = '0';

	struct movestate *p = board_hash_get(hash);
	if (p == $board_hash_initial) {
		int tmp;
		tmp = index;
		index = foward;
		foward = tmp;

		get_answer_r(board, depth, hash, index, foward, $answer);
		printf("A: %s\n", $answer);
		exit(-1);
	} else if (p->next != 0) {
		return;
	}

	board_hash_set(hash, -depth, index, 1);
	nextstack[*stackcount].foward = foward;
	nextstack[*stackcount].index = index;
	nextstack[*stackcount].hash = hash;
	strcpy(nextstack[*stackcount].board, board);
	(*stackcount)++;
}


#define MAX_FINISH_SIZE (1000 * 1000 * 2)
int create_finish_hash(char final_board[36 + 1], unsigned long long final_hash) {
	int stackcount = 0;
	int nextstackcount = 0;
	int depth = 1;

	struct stack *stack0 = (struct stack *)malloc(sizeof(struct stack) * MAX_FINISH_SIZE);
	struct stack *stack1 = (struct stack *)malloc(sizeof(struct stack) * MAX_FINISH_SIZE);

	if (stack0 == NULL || stack1 == NULL) {
		printf("G: memory allocate error finish_stack\n");
		exit(-1);
	}

	struct stack *stack = stack0;
	struct stack *nextstack = stack1;

	stack[0].foward = $width * $height - 1;
	stack[0].index = -1;
	stack[0].hash = final_hash;
	strcpy(stack[0].board, final_board);
	stackcount++;

	while (stackcount > 0 && depth < MAXTIMES) {
printf("D: %d, %d\n", depth, stackcount);

		for (int i = 0; i < stackcount; i++) {
			if (nextstackcount > MAX_FINISH_SIZE - 10000) {
printf("D: nextstackcount %d\n", nextstackcount);

				nextstackcount = 0;
				depth--;
				break;
			}

			int index = stack[i].foward;
			int prev = stack[i].index;
			unsigned long long hash = stack[i].hash;
			char *board = stack[i].board;

			int x = index % $width;
			int y = index / $width;

			if (index - $width != prev && !check_wall(x, y - 1)) {
				move_r(board, depth, hash, index, index - $width, nextstack, &nextstackcount);
			}
			if (index + $width != prev && !check_wall(x, y + 1)) {
				move_r(board, depth, hash, index, index + $width, nextstack, &nextstackcount);
			}
			if (index - 1 != prev && !check_wall(x - 1, y)) {
				move_r(board, depth, hash, index, index - 1, nextstack, &nextstackcount);
			}
			if (index + 1 != prev && !check_wall(x + 1, y)) {
				move_r(board, depth, hash, index, index + 1, nextstack, &nextstackcount);
			}
		}

		stackcount = nextstackcount;
		if (stack == stack0) {
			stack = stack1;
			nextstack = stack0;
		} else {
			stack = stack0;
			nextstack = stack1;
		}
		nextstackcount = 0;
		depth++;
	}

	free(stack0);
	free(stack1);

	return depth;
}


int $md[36][36];

void make_md(void) {
	int max = $width * $height;

	memset($md, -1, sizeof($md));
//	if ($n_wall > 0) {
	if (1) {
		for (int src = 0; src < max; src++) {
			int src_x = src % $width;
			int src_y = src / $width;
			for (int dst = 0; dst < max; dst++) {
				int dst_x = dst % $width;
				int dst_y = dst / $width;

				$md[src][dst] = abs(src_x - dst_x) + abs(src_y - dst_y);
			}
		}
	} else {
		for (int src = 0; src < max; src++) {
			if ($wall[src])
				continue;

			int n = $n_wall + 1;
			$md[src][src] = 0;
			for (int depth = 0; n < max ; depth++) {
				for (int index = 0; index < max; index++) {
					if ($md[src][index] != depth)
						continue;

					int x = index % $width;
					int y = index / $width;

					if (!check_wall(x, y + 1) && $md[src][index + $width] == -1) {
						$md[src][index + $width] = depth + 1;
						n++;
					}
					if (!check_wall(x + 1, y) && $md[src][index + 1] == -1) {
						$md[src][index + 1] = depth + 1;
						n++;
					}
					if (!check_wall(x, y - 1) && $md[src][index - $width] == -1) {
						$md[src][index - $width] = depth + 1;
						n++;
					}
					if (!check_wall(x - 1, y) && $md[src][index - 1] == -1) {
						$md[src][index - 1] = depth + 1;
						n++;
					}
				}
				
			}
		}
	}
}

int get_md(char block, int index) {
	int src = chr_to_index(block) - 1;
	if (src < 0)
		src = $width * $height - 1;
	return $md[src][index];
}

int $depth;

struct movestate *ida(char board[36 + 1], unsigned long long hash, int index, int prev, int md, int depth);
struct movestate *move_f(char board[36 + 1], unsigned long long prev_hash, int index, int prev, int foward, int md, int depth, char movechar) {
	unsigned long long hash = move_hash(board, prev_hash, index, foward);
	struct movestate *p = board_hash_get(hash);

	$answer[depth - 1] = movechar;

	if (p->is_answer) {
printf("D: *%d %d\n", p->depth, p->index);
printf("D: %d %d %d\n", prev, index, foward);
		board[index] = board[foward];
		board[foward] = '0';

		get_answer_r(board, -p->depth , hash, foward, p->index, $answer + depth);
		printf("D: %d %c\n", depth, movechar);

		return p;
	}
	if (p->hash) {
		if (p->depth != depth) return NULL;
	} else {
		board_hash_set(hash, depth, index, 0);
	}

	md -= get_md(board[index], index);
	md -= get_md(board[foward], foward);

	board[index] = board[foward];
	board[foward] = '0';

	md += get_md(board[index], index);
	md += get_md(board[foward], foward);

	if (depth + md <= $depth) {
		p = ida(board, hash, foward, index, md, depth);
		if (p) {
			printf("D: %d %c\n", depth, movechar);

			return p;
		}
	}

	board[foward] = board[index];
	board[index] = '0';
	return NULL;
}

struct movestate *ida(char board[36 + 1], unsigned long long hash, int index, int prev, int md, int depth) {
	struct movestate *p;

	int x = index % $width;
	int y = index / $width;

	depth++;
	if (index + $width != prev && !check_wall(x, y + 1)) {
		p = move_f(board, hash, index, prev, index + $width, md, depth, 'D');
		if (p) return p;
	}
	if (index + 1 != prev && !check_wall(x + 1, y)) {
		p = move_f(board, hash, index, prev, index + 1, md, depth, 'R');
		if (p) return p;
	}
	if (index - $width != prev &&!check_wall(x, y - 1)) {
		p = move_f(board, hash, index, prev, index - $width, md, depth, 'U');
		if (p) return p;
	}
	if (index - 1 != prev && !check_wall(x - 1, y)) {
		p = move_f(board, hash, index, prev, index - 1, md, depth, 'L');
		if (p) return p;
	}

	return NULL;
}

void solver(const char initial_board[36 + 1], int lower_bound) {
	int md = 0;
	int start = 0;
	for (int index = 0; index < $maxnum; index++) {
		if (initial_board[index] == '=')
			continue;
		if (initial_board[index] == '0')
			start = index;
		md += get_md(initial_board[index], index);
	}

	if (lower_bound < md) {
		lower_bound = md;
	}

	char board[36 + 1];
	strcpy(board, initial_board);
	unsigned long long hash = hash_from_board(initial_board);

	struct movestate *p = NULL;
	for ($depth = lower_bound; $depth < MAXTIMES ;$depth += 2) {
		printf("D: IDA* %d\n", $depth);

		p = ida(board, hash, start, -1, md, 0);
		if (p)
			break;
	}

	if (!p) {
		puts("G: giveup");
		exit(-1);
	}

	printf("D: %d %d %p \n", p->depth, p->index, p->next);

	if (*$answer == '\0') {
		puts("E: error **");
		exit(-1);
	}

	printf("A: %s\n", $answer);
	exit(-1);
}

int main(int argc, char *argv[]) {
	char initial_board[36 + 1];
	char final_board[36 + 1] = "123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	unsigned long long initial_hash;
	unsigned long long final_hash;

	memset($answer, '\0', sizeof($answer));

	init_genrand(time(NULL));

	if (argc != 2) {
		puts("E: invalid ARGV");
		exit(-1);
	}

	$width = argv[1][0] - '0';
	$height = argv[1][2] - '0';

	if (strlen(argv[1] + 4) != $width * $height) {
		puts("E: invalid data");
		exit(-1);
	}

	strcpy(initial_board, argv[1] + 4);
printf("D: initial board [%s]\n", initial_board);

	$maxnum = $width * $height;

	$hash_seed = (unsigned long long *)malloc($width * $height * $maxnum * sizeof(*$hash_seed));
	int max = $width * $height * $maxnum;
	for (int i = 0; i < max; i++) {
		$hash_seed[i] = (unsigned long long)genrand_int32() << 32 | genrand_int32();
	}

	memset($wall, 0, sizeof($wall));
	$n_wall = 0;
	max = strlen(initial_board);

	for (int x = 0; x < $width + 2; x++) {
		$wall[x] = 2;
		$wall[x + ($width + 2) * ($height + 1)] = 2;
	}

	for (int y = 1; y < $height + 1; y++) {
		$wall[($width + 2) * y] = 2;
		$wall[($width + 2) * y + $width + 1] = 2;
	}

	for (int i = 0; i < max; i++) {
		char chr = initial_board[i];
		if (chr == '=') {
			int x = i % $width;
			int y = i / $width;

			$wall[x + 1 + (y + 1) * ($width + 2)] = 1;
			$n_wall++;
		}
	}

	final_board[$width * $height - 1] = '0';
	final_board[$width * $height] = '\0';
	for (int y = 0; y < $height; y++) {
		for (int x = 0; x < $width; x++) {
			if ($wall[x + 1 + (y + 1) * ($width + 2)])
				final_board[x + y * $width] = '=';
		}
	}

	make_md();

	initial_hash = hash_from_board(initial_board);
	final_hash = hash_from_board(final_board);

printf("D: final board [%s]\n", final_board);

	$board_index = (struct movestate *)malloc(sizeof(struct movestate) * BOARD_INDEX_SIZE);
	if ($board_index == NULL) {
		printf("G: board_index memory allocate error\n");
		exit(-1);
	}
	memset($board_index, 0, sizeof(struct movestate) * BOARD_INDEX_SIZE);

	$board_pool = (struct movestate *)malloc(sizeof(struct movestate) * BOARD_POOL_SIZE);
	if ($board_pool == NULL) {
		printf("G: board_pool memory allocate error\n");
		exit(-1);
	}
	memset($board_pool, 0, sizeof(struct movestate) * BOARD_POOL_SIZE);
	$board_pool_ptr = $board_pool;

	$board_hash_initial = board_hash_set(initial_hash, 0, -1, 0);
	$board_hash_final = board_hash_set(final_hash, 0, -1, 1);

	int lower_bound = create_finish_hash(final_board, final_hash);
	solver(initial_board, lower_bound);

puts("G: giveup");
return 1;
}

