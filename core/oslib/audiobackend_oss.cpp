#include "audiostream.h"
#ifdef USE_OSS
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>
#include <unistd.h>

static int oss_audio_fd = -1;

static void oss_init()
{
	oss_audio_fd = open("/dev/dsp", O_WRONLY);
	if (oss_audio_fd < 0)
	{
		WARN_LOG(AUDIO, "Couldn't open /dev/dsp.");
	}
	else
	{
		INFO_LOG(AUDIO, "sound enabled, dsp opened for write");
		int tmp=44100;
		int err_ret;
		err_ret=ioctl(oss_audio_fd,SNDCTL_DSP_SPEED,&tmp);
		INFO_LOG(AUDIO, "set Frequency to %i, return %i (rate=%i)", 44100, err_ret, tmp);
		int channels=2;
		err_ret=ioctl(oss_audio_fd, SNDCTL_DSP_CHANNELS, &channels);
		INFO_LOG(AUDIO, "set dsp to stereo (%i => %i)", channels, err_ret);
		int format=AFMT_S16_LE;
		err_ret=ioctl(oss_audio_fd, SNDCTL_DSP_SETFMT, &format);
		INFO_LOG(AUDIO, "set dsp to %s audio (%i/%i => %i)", "16bits signed", AFMT_S16_LE, format, err_ret);
	}
}

static u32 oss_push(const void* frame, u32 samples, bool wait)
{
	write(oss_audio_fd, frame, samples*4);
	return 1;
}

static void oss_term() {
	if(oss_audio_fd >= 0)
	{
		close(oss_audio_fd);
	}
}

static audiobackend_t audiobackend_oss = {
		"oss", // Slug
		"Open Sound System", // Name
		&oss_init,
		&oss_push,
		&oss_term,
		NULL
};

static bool oss = RegisterAudioBackend(&audiobackend_oss);
#endif
