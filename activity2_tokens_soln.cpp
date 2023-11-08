#include <iostream>
#include <chrono>
#include <omp.h>

// initialize elements to random integer values 0 to range-1
void initElements (unsigned int range, unsigned int num_els, unsigned int* elements) {
    for (int i=0; i<num_els; i++)  {
        elements[i] = rand() % range;
    }
}

// initialize tokens to search.  again 0 to range-1
// note, we should probably enforce that tokens are unique. not important for performance.
void initTokens (int range, int num_toks, unsigned int* tokens) {
    for (int i=0; i<num_toks; i++)  {
        tokens[i] = rand() % range;
    }
}

// initialize all token counts to zero
void initCounts (int num_toks, unsigned int* token_counts) {
    for (int i=0; i<num_toks; i++)  {
        token_counts[i] = 0;
    }
}

// count the number of appearances of each token in the data
void countTokensElementsFirst (unsigned int num_els, unsigned int num_tokens,
                  unsigned int* elements, unsigned int* tokens, unsigned int* token_counts) {

    /* for all elements in the array */
    for (int el=0; el<num_els; el++) {
        /* for all tokens in the list */
        for (int tok=0; tok<num_tokens; tok++) {
            /* update the count for the token */
            if (elements[el] == tokens[tok]) {
                token_counts[tok]++;
            }
        }
    }
}

// count the number of appearances of each token in the data
void countTokensTokensFirst (unsigned int num_els, unsigned int num_tokens,
                             unsigned int* elements, unsigned int* tokens, unsigned int* token_counts) {

    /* for all tokens in the list */
    for (int tok=0; tok<num_tokens; tok++) {
        /* for all elements in the array */
        for (int el=0; el<num_els; el++) {
            /* update the count for the token */
            if (elements[el] == tokens[tok]) {
                token_counts[tok]++;
            }
        }
    }
}

// count the number of appearances of each token in the data
void omp_countTokensElementsFirst (unsigned int num_els, unsigned int num_tokens,
                  unsigned int* elements, unsigned int* tokens, unsigned int* token_counts) {

    #pragma omp parallel for
    /* for all elements in the array */
    for (int el=0; el<num_els; el++) {
        /* for all tokens in the list */
        for (int tok=0; tok<num_tokens; tok++) {
            /* update the count for the token */
            if (elements[el] == tokens[tok]) {
                token_counts[tok]++;
            }
        }
    }
}

// count the number of appearances of each token in the data
void omp_countTokensTokensFirst (unsigned int num_els, unsigned int num_tokens,
                             unsigned int* elements, unsigned int* tokens, unsigned int* token_counts) {

    #pragma omp parallel for
    /* for all tokens in the list */
    for (int tok=0; tok<num_tokens; tok++) {
        /* for all elements in the array */
        for (int el=0; el<num_els; el++) {
            /* update the count for the token */
            if (elements[el] == tokens[tok]) {
                token_counts[tok]++;
            }
        }
    }
}

// elements first with reduction
void omp_countTokensElementsFirst_reduce (unsigned int num_els, unsigned int num_tokens,
                             unsigned int* elements, unsigned int* tokens, unsigned int* token_counts) {

    #pragma omp parallel for reduction(+:token_counts[:num_tokens])
    /* for all elements in the array */
    for (int el=0; el<num_els; el++) {
        /* for all tokens in the list */
        for (int tok=0; tok<num_tokens; tok++) {
            /* update the count for the token */
            if (elements[el] == tokens[tok]) {
                token_counts[tok]++;
            }
        }
    }
}

// tokens first with reduction
void omp_countTokensTokensFirst_reduce (unsigned int num_els, unsigned int num_tokens,
                             unsigned int* elements, unsigned int* tokens, unsigned int* token_counts) {

    #pragma omp parallel for reduction(+:token_counts[:num_tokens])
    /* for all tokens in the list */
    for (int tok=0; tok<num_tokens; tok++) {
        /* for all elements in the array */
        for (int el=0; el<num_els; el++) {
            /* update the count for the token */
            if (elements[el] == tokens[tok]) {
                token_counts[tok]++;
            }
        }
    }
}


// unroll tokens elements first with reduction
void unroll_omp_countTokensElementsFirst_reduce (unsigned int num_els, unsigned int num_tokens,
                             unsigned int* elements, unsigned int* tokens, unsigned int* token_counts) {

    #pragma omp parallel for reduction(+:token_counts[:num_tokens])
    /* for all elements in the array */
    for (int el=0; el<num_els; el++) {
        /* for all tokens in the list */
        for (int tok=0; tok<num_tokens; tok+=8) {
            /* update the count for the token */
            if (elements[el] == tokens[tok]) {
                token_counts[tok]++;
            }
            if (elements[el] == tokens[tok+1]) {
                token_counts[tok+1]++;
            }
            if (elements[el] == tokens[tok+2]) {
                token_counts[tok+2]++;
            }
            if (elements[el] == tokens[tok+3]) {
                token_counts[tok+3]++;
            }
            if (elements[el] == tokens[tok+4]) {
                token_counts[tok+4]++;
            }
            if (elements[el] == tokens[tok+5]) {
                token_counts[tok+1]++;
            }
            if (elements[el] == tokens[tok+6]) {
                token_counts[tok+2]++;
            }
            if (elements[el] == tokens[tok+7]) {
                token_counts[tok+7]++;
            }
        }
    }
}


int main() {

    const unsigned int range = 4096;
    const unsigned int num_tokens = 128;
    const unsigned int num_elements = 4096*256;
    const unsigned int loop_iterations = 16;

    unsigned int tokens[num_tokens];
    unsigned int elements[num_elements];
    unsigned int token_counts[num_tokens];

    initElements(range, num_elements, elements);
    initTokens(range, num_tokens, tokens);
    initCounts(num_tokens, token_counts);

    omp_set_num_threads(4);

    // run once to warm the cache
    countTokensTokensFirst(num_elements, num_tokens, elements, tokens, token_counts);

    // countTokensTokensFirst
    // Start the timer
    auto start = std::chrono::high_resolution_clock::now();
    for(int j=0; j<loop_iterations; j++) {
        countTokensTokensFirst(num_elements, num_tokens, elements, tokens, token_counts);
    }
    // Stop the timer
    auto end = std::chrono::high_resolution_clock::now();
    // Calculate the duration
    std::chrono::duration<double> duration = end - start;
    // Print the duration in seconds
    std::cout << "Tokens First time: " << duration.count() << " seconds" << std::endl;

    // reset counts only works right if running one loop_iteration
    initCounts(num_tokens, token_counts);

    // run once to warm the cache
    countTokensElementsFirst(num_elements, num_tokens, elements, tokens, token_counts);
  

    // countTokensElementsFirst
    start = std::chrono::high_resolution_clock::now();
    for(int j=0; j<loop_iterations; j++) {
        countTokensElementsFirst(num_elements, num_tokens, elements, tokens, token_counts);
    }
    end = std::chrono::high_resolution_clock::now();
    duration = end - start;
    std::cout << "Elements First time: " << duration.count() << " seconds" << std::endl;

    // reset counts only works right if running one loop_iteration
    initCounts(num_tokens, token_counts);
    	
    
    // omp_countTokensTokensFirst
    start = std::chrono::high_resolution_clock::now();
    for(int j=0; j<loop_iterations; j++) {
        omp_countTokensTokensFirst(num_elements, num_tokens, elements, tokens, token_counts);
    }
    end = std::chrono::high_resolution_clock::now();
    duration = end - start;
    std::cout << "OMP Tokens First time: " << duration.count() << " seconds" << std::endl;

    // reset counts only works right if running one loop_iteration
    initCounts(num_tokens, token_counts);

    
    // omp_countTokensElementsFirst
    start = std::chrono::high_resolution_clock::now();
    for(int j=0; j<loop_iterations; j++) {
        omp_countTokensElementsFirst(num_elements, num_tokens, elements, tokens, token_counts);
    }
    end = std::chrono::high_resolution_clock::now();
    duration = end - start;
    std::cout << "OMP Elements First time: " << duration.count() << " seconds" << std::endl;

    // reset counts only works right if running one loop_iteration
    initCounts(num_tokens, token_counts);

    // omp_countTokensTokensFirst_reduce
    start = std::chrono::high_resolution_clock::now();
    for(int j=0; j<loop_iterations; j++) {
        omp_countTokensTokensFirst_reduce(num_elements, num_tokens, elements, tokens, token_counts);
    }
    end = std::chrono::high_resolution_clock::now();
    duration = end - start;
    std::cout << "OMP Tokens First Reduce time: " << duration.count() << " seconds" << std::endl;

    // omp_countTokensElementsFirst_reduce
    start = std::chrono::high_resolution_clock::now();
    for(int j=0; j<loop_iterations; j++) {
        omp_countTokensElementsFirst_reduce(num_elements, num_tokens, elements, tokens, token_counts);
    }
    end = std::chrono::high_resolution_clock::now();
    duration = end - start;
    std::cout << "OMP Elements First Reduce time.: " << duration.count() << " seconds" << std::endl;

    // reset counts only works right if running one loop_iteration
    initCounts(num_tokens, token_counts);

    // unroll_omp_countTokensElementsFirst_reduce
    start = std::chrono::high_resolution_clock::now();
    for(int j=0; j<loop_iterations; j++) {
        unroll_omp_countTokensElementsFirst_reduce(num_elements, num_tokens, elements, tokens, token_counts);
    }
    end = std::chrono::high_resolution_clock::now();
    duration = end - start;
    std::cout << "Unroll OMP Elements First Reduce time.: " << duration.count() << " seconds" << std::endl;

}
