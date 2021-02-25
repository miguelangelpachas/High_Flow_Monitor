/*
 * fio2_sensor_calibration.h
 *
 * Created: 18/02/2021 17:38:38
 *  Author: mtorres
 */ 


#ifndef FIO2_SENSOR_CALIBRATION_H_
#define FIO2_SENSOR_CALIBRATION_H_

void FiO2_Sensor_Calibration_Sequence(void);
void Calibration_Sample_Reference_Point(uint8_t index, uint8_t fio2_ref_val);
void Calibration_Sampling_Sequence(uint8_t index);
void Calibration_Compute_Parameters(void);

#endif /* FIO2_SENSOR_CALIBRATION_H_ */