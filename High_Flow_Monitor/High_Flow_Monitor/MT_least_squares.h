
#ifndef MT_LEAST_SQUARES_H_
#define MT_LEAST_SQUARES_H_

/* File inclusion*/
#include <stdint.h>

#define LEAST_SQUARES_SAMPLE_VECTOR_MAX_LENGTH  50

void Least_Squares_X_Sample_Vector_Fill_In(float *origin_buffer, uint32_t initial_pos, uint32_t n_samples);
void Least_Squares_Y_Sample_Vector_Fill_In(float *origin_buffer, uint32_t initial_pos, uint32_t n_samples);
void Least_Squares_XY_Sample_Vector_Fill_In(uint32_t n_samples);
void Least_Squares_X2_Sample_Vector_Fill_In(uint32_t n_samples);

void Least_Squares_Sum_X_Calculate(uint32_t n_samples);
void Least_Squares_Sum_Y_Calculate(uint32_t n_samples);
void Least_Squares_Sum_XY_Calculate(uint32_t n_samples);
void Least_Squares_Sum_X2_Calculate(uint32_t n_samples);
void Least_Squares_Sum_2_X_Calculate(void);

uint8_t Least_Squares_Linear_Slope_Calculate(uint32_t n_samples);
uint8_t Least_Squares_Linear_Offset_Calculate(uint32_t n_samples);

float Least_Squares_Get_Sum_X(void);
float Least_Squares_Get_Sum_Y(void);
float Least_Squares_Get_Sum_XY(void);
float Least_Squares_Get_Sum_X2(void);
float Least_Squares_Get_Sum_2_X(void);
float Least_Squares_Get_Linear_Slope(void);
float Least_Squares_Get_Linear_Offset(void);

#endif