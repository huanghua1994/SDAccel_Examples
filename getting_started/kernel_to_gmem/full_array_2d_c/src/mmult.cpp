/**********
Copyright (c) 2017, Xilinx, Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
may be used to endorse or promote products derived from this software
without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**********/

/*******************************************************************************
Description: 
    C Kernel Example of Matrix Multiplication to demonstrate 2-D full array access
*******************************************************************************/
				      
//Includes 
#include <stdio.h>
#include <string.h>

//define max local buffer size
#define N 256

extern "C" {
    void mmult(int *a, int *b, int *c, int size) {
    #pragma HLS INTERFACE m_axi port=a offset=slave bundle=gmem
    #pragma HLS INTERFACE m_axi port=b offset=slave bundle=gmem
    #pragma HLS INTERFACE m_axi port=c offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=a bundle=control
    #pragma HLS INTERFACE s_axilite port=b bundle=control
    #pragma HLS INTERFACE s_axilite port=c bundle=control
    #pragma HLS INTERFACE s_axilite port=size bundle=control
    #pragma HLS INTERFACE s_axilite port=return bundle=control
    
        int bufa[N][N], bufb[N][N], bufc[N][N];
        int matrix_size = size*size;
        // Read data from global memory and write into local buffer for a, loop pipeline will be automatically inferred
        int x = 0, y = 0;
        read_data_a: for (int i = 0 ; i < matrix_size ; i++){
        #pragma HLS LOOP_TRIPCOUNT min=1 max=65536
            int tmpData_a = a[i];
            bufa[x][y] = tmpData_a;
            if (y == size-1){
                x++;
                y = 0;
            }else{
                y++;
            }
        }
    
        // Read data from global memory and write into local buffer for b, loop pipeline will be automatically inferred
        read_data_b: for (int i = 0, x=0, y=0; i < matrix_size ; i++){
        #pragma HLS LOOP_TRIPCOUNT min=1 max=65536
            int tmpData_b = b[i];
            bufb[x][y] = tmpData_b;
            if (y == size-1){
                x++;
                y = 0;
            }else{
                y++;
            }
        }
    
        // Calculate matrix multiplication using local data buffer based on input size, and write results into local buffer for c
        matrix_mult: for (int row = 0; row < size; row++) {
        #pragma HLS LOOP_TRIPCOUNT min=1 max=256
            for (int col = 0; col < size; col++) {
            #pragma HLS LOOP_TRIPCOUNT min=1 max=256
                int result = 0;
                for (int k = 0; k < size; k++) {
                #pragma HLS LOOP_TRIPCOUNT min=1 max=256
                #pragma HLS pipeline
                    result += bufa[row][k] * bufb[k][col];
                }
                bufc[row][col] = result;
            }
        }
        // Write results from local buffer to global memory for c, loop pipeline will be automatically inferred
        int m = 0, n = 0;
        write_data: for (int i = 0 ; i < matrix_size ; i++){
        #pragma HLS LOOP_TRIPCOUNT min=1 max=65536
            int tmpData_c = bufc[m][n];
            c[i] = tmpData_c;
            if (n == size-1){
                m++;
                n = 0;
            }else{
                n++;
            }
        }
    }
}
