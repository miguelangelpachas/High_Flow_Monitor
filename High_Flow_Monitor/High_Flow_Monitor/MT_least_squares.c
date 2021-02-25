


/* File inclusion */
#include <stdint.h>
#include "MT_least_squares.h"
#include "project_defines.h"

/* Variable definition */

float x_sample_vector[LEAST_SQUARES_SAMPLE_VECTOR_MAX_LENGTH];
float y_sample_vector[LEAST_SQUARES_SAMPLE_VECTOR_MAX_LENGTH];
float xy_sample_vector[LEAST_SQUARES_SAMPLE_VECTOR_MAX_LENGTH];
float x2_sample_vector[LEAST_SQUARES_SAMPLE_VECTOR_MAX_LENGTH];

float sum_x;
float sum_y;
float sum_xy;
float sum_x2;
float sum_2_x;


static float linear_slope_m;
static float linear_offset_y0;


void Least_Squares_X_Sample_Vector_Fill_In(float *origin_buffer, uint32_t initial_pos, uint32_t n_samples){
	
	uint32_t index = 0;
	
	for(index = 0; index < n_samples; index++){
		x_sample_vector[initial_pos + index] = *(origin_buffer + index);
	}
	
}


void Least_Squares_Y_Sample_Vector_Fill_In(float *origin_buffer, uint32_t initial_pos, uint32_t n_samples){
	
	uint32_t index = 0;
	
	for(index = 0; index < n_samples; index++){
		y_sample_vector[initial_pos + index] = *(origin_buffer + index);
	}
	
}


void Least_Squares_XY_Sample_Vector_Fill_In(uint32_t n_samples){
	
	uint32_t index = 0;
	
	for(index = 0; index < n_samples; index++){
		xy_sample_vector[index] = x_sample_vector[index] * y_sample_vector[index];
	}
	
}


void Least_Squares_X2_Sample_Vector_Fill_In(uint32_t n_samples){
	
	uint32_t index = 0;
	
	for(index = 0; index < n_samples; index++){
		x2_sample_vector[index] = x_sample_vector[index] * x_sample_vector[index];
	}
}


void Least_Squares_Sum_X_Calculate(uint32_t n_samples){
	
	uint32_t index = 0;
	
	sum_x = 0;
	
	for(index = 0; index < n_samples; index++){
		sum_x += x_sample_vector[index];
	}
	
}


void Least_Squares_Sum_Y_Calculate(uint32_t n_samples){
	
	uint32_t index = 0;
	
	sum_y = 0;
	
	for(index = 0; index < n_samples; index++){
		sum_y += y_sample_vector[index];
	}
	
}


void Least_Squares_Sum_XY_Calculate(uint32_t n_samples){
	
	uint32_t index = 0;
	
	sum_xy = 0;
	
	for(index = 0; index < n_samples; index++){
		sum_xy += xy_sample_vector[index];
	}
	
}


void Least_Squares_Sum_X2_Calculate(uint32_t n_samples){
	
	uint32_t index = 0;
	
	sum_x2 = 0;
	
	for(index = 0; index < n_samples; index++){
		sum_x2 += x2_sample_vector[index];
	}
	
}


void Least_Squares_Sum_2_X_Calculate(void){
	
	sum_2_x = sum_x * sum_x;
	
}


uint8_t Least_Squares_Linear_Slope_Calculate(uint32_t n_samples){
	
	uint8_t linear_slope_calc_error = 0;
	float numerator;
	float denominator;
	
	denominator = (n_samples * sum_x2) - sum_2_x;
	
	if(denominator != 0){
		numerator = (n_samples * sum_xy) - (sum_x * sum_y);
		linear_slope_m = numerator / denominator;
		}else{
		linear_slope_calc_error = 1;
	}
	
	return linear_slope_calc_error;
}



uint8_t Least_Squares_Linear_Offset_Calculate(uint32_t n_samples){
	
	uint8_t linear_offset_calc_error = 0;
	float numerator;
	float denominator;
	
	denominator = (n_samples * sum_x2) - sum_2_x;
	
	if(denominator != 0){
		numerator = (sum_y * sum_x2) - (sum_x * sum_xy);
		linear_offset_y0 = numerator / denominator;
		}else{
		linear_offset_calc_error = 1;
	}
	
	return linear_offset_calc_error;
	
}


float Least_Squares_Get_Sum_X(void){
	return sum_x;
}

float Least_Squares_Get_Sum_Y(void){
	return sum_y;
}

float Least_Squares_Get_Sum_XY(void){
	return sum_xy;
}

float Least_Squares_Get_Sum_X2(void){
	return sum_x2;
}

float Least_Squares_Get_Sum_2_X(void){
	return sum_2_x;
}




float Least_Squares_Get_Linear_Slope(void){
	return linear_slope_m;
}


float Least_Squares_Get_Linear_Offset(void){
	return linear_offset_y0;
}



