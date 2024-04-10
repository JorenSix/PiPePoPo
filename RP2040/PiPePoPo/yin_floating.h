#include <stddef.h>


#ifndef YIN_FLOATING_H
#define YIN_FLOATING_H


    /**
	 * @struct Yin_Floating
	 *
	 * @brief Contains state information.
	 * 
	 * The memory used should be freed in the  @_destroy@ method. 
	 */
	typedef struct Yin_Floating Yin_Floating;

    /**
	 * Initialize a new @ref Yin_Floating struct according to the given configuration.
	 * @return An initialized @ref Yin_Floating struct or undefined if memory could not be allocated.
	 */
	Yin_Floating * yin_floating_new(float sample_rate,size_t buffer_size);

    float yin_floating_estimate_pitch(Yin_Floating * yin_floating,float* buffer);

    float yin_floating_last_pitch_estimate_probability(Yin_Floating * yin_floating);

    /**
	 * Free memory or other resources.
	 * @param  yin_floating The object to destroy.
	 */ 
	void yin_floating_destroy(Yin_Floating * yin_floating) ;

#endif // YIN_FLOATING_H
