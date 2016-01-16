#include <cstdio>
#include <algorithm>
#include <cstdint>

#include "soundio.h"
#include "main.h"

int SoundIO::init_sound()
{
    PaStreamParameters outputParameters;
    PaError err;

    // initialize audio
    err = Pa_Initialize();
    if( err != paNoError ) return 1;
    puts("Initialized");

    outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */

    outputParameters.channelCount = 1;
    outputParameters.sampleFormat = paInt16;
    outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;

    err = Pa_OpenStream(
              &stream,
              NULL, /* no input */
              &outputParameters,
              Fs,
              FRAMES_PER_BUFFER,
              paClipOff,
              NULL,
              NULL );
    if( err != paNoError ) return 1;

    err = Pa_StartStream( stream );
    if( err != paNoError ) return 1;
    puts("Stream Opened");


    //////////////////////////

    SF_INFO sfinfo;
    sfinfo.channels = 1;
    sfinfo.samplerate = Fs;
    sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;

    file = sf_open("sound.wav", SFM_WRITE, &sfinfo);

    puts("File Opened");

    return 0;
}

int SoundIO::play_sound(int16_t* data, int N)
{
    PaError err;
    for(int i = 0; i < N; i += FRAMES_PER_BUFFER)
    {
        err = Pa_WriteStream( stream, &data[i], FRAMES_PER_BUFFER );
        if( err != paNoError ) return 1;
    }

    sf_write_short(file, data, N);
    sf_write_sync(file);

    return 0;
}

int SoundIO::end_sound()
{
    PaError err;
    err = Pa_StopStream( stream );
    if( err != paNoError ) return 1;
    puts("Stream Stopped");

    Pa_Terminate();

    sf_close(file);

    return 0;
}

int16_t SoundIO::normalize(const fpoint& x)
{
    return std::min(std::max(int(x * INT16_MAX), INT16_MIN), INT16_MAX);
}
