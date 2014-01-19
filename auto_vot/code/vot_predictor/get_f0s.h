
#include <infra.h>

unsigned int get_f0s_main(infra::vector samples, infra::vector &f0, infra::vector &vuv, infra::vector &rms_speech, infra::vector &acpkp,
								 double frame_step=0.01f, double window_duration=0.0075f, double sampling_rate=16000);