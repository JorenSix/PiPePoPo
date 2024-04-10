#include <stddef.h>
#include <stdint.h>

#include "fixed_point.h"

#ifndef YIN_FIXED_H
#define YIN_FIXED_H

	/**
	 * @struct Yin_Fixed
	 *
	 * @brief Contains state information.
	 * 
	 * The memory used should be freed in the  @_destroy@ method. 
	 */
	typedef struct Yin_Fixed Yin_Fixed;

	/**
	 * Initialize a new @ref Yin_Fixed struct according to the given configuration.
	 * @return An initialized @ref Yin_Fixed struct or undefined if memory could not be allocated.
	 */
	Yin_Fixed * yin_fixed_new(float sample_rate,size_t buffer_size);

	float yin_fixed_estimate_pitch(Yin_Fixed * yin_fixed,int16_t* buffer);

	float yin_fixed_last_pitch_estimate_probability(Yin_Fixed * yin_fixed);

	/**
	 * Free memory or other resources.
	 * @param  yin_fixed The object to destroy.
	 */ 
	void yin_fixed_destroy(Yin_Fixed * yin_fixed) ;

#endif // YIN_FIXED_H
