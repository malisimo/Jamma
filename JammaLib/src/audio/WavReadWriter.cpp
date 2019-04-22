#include "WavReadWriter.h"

using namespace audio;

std::optional<std::tuple<std::vector<float>, unsigned int, unsigned int>>
	WavReadWriter::ReadWavFile(const std::string& fname, unsigned int maxsamps)
{
	struct SoundHeader info;
	struct SoundHeader* pinfo = &info;

	// The variable maxsamps represents the maximum
	// number of 32 bit floats the array can contain.
	if (maxsamps < 1)
		return std::nullopt;

	auto cname = new char[fname.size() + 1];
	strcpy_s(cname, fname.size() + 1, fname.c_str());

	auto fid = OpenSoundIn(cname, pinfo);

	delete cname;

	if (fid == NULL)
		return {};

	size_t numSampsLoaded = maxsamps;

	// Only load the reported number of samples
	// (but prevent loading more than the array can hold)
	if (info.dlength < (long)(maxsamps * 2))
		numSampsLoaded = info.dlength / 2; // Two 16 bit integers per sample (32 bit float)

	if (numSampsLoaded < 1)
	{
		numSampsLoaded = 0;
		fclose(fid);

		return {};
	}

	auto datain = new char[numSampsLoaded * 2];  // Two bytes per wav file sample (16 bit integer)
	auto bytesread = fread(datain, 1, numSampsLoaded * 2, fid);

	if ((bytesread / 2) < numSampsLoaded)
		numSampsLoaded = bytesread / 2;  // Two bytes per wav file sample (16 bit integer)

	fclose(fid);

	std::vector<float> data(numSampsLoaded);

	for (int i = 0; i<numSampsLoaded; i++)
	{
		data[i] = CharToFloat(datain + (i * 2));
	}

	delete datain;

	return std::make_tuple(std::move(data), (unsigned int)numSampsLoaded, info.srate);
}

bool WavReadWriter::WriteWavFile(std::string fname, std::vector<float> data, unsigned int numSamps, unsigned int sampleRate)
{
	char* cname;
	char* dataout;
	struct SoundHeader info;
	struct SoundHeader* pinfo = &info;
	FILE* fid;

	cname = new char[fname.size() + 1];
	strcpy_s(cname, fname.size() + 1, fname.c_str());

	if (numSamps < 1)
	{
		delete cname;
		return false;
	}

	info.srate = sampleRate;
	info.dlength = numSamps * 2;
	info.bits_per_samp = 16;

	FillHeader(info);

	fid = OpenSoundOut(cname, pinfo);

	delete cname;

	if (fid == NULL)
		return false;

	dataout = new char[numSamps * 2]; // Two bytes per wav file sample (16 bit integer)

	for (unsigned int i = 0; i < numSamps; i++)
	{
		FloatToChar(data[i], dataout + (i * 2));
	}

	fwrite(dataout, 1, numSamps * 2, fid);

	fclose(fid);
	delete dataout;

	return true;
}

FILE* WavReadWriter::OpenSoundIn(char *filename, struct SoundHeader *hdr)
{
	FILE *myfile;

	// Check file name for .wav extension
	if (filename[strlen(filename) - 1] == 'v')
	{
		myfile = _fsopen(filename, "rb", _SH_DENYNO);

		if (myfile == NULL)
		{
			std::cout << "Open (Read) wav file error\n";
		}
		else
			fread(hdr, 1, 44, myfile);
	}
	else
	{
		std::cout << "You must specify a valid .wav file name\n";
		myfile = 0;
	}

	return myfile;
}

FILE *WavReadWriter::OpenSoundOut(char *filename, struct SoundHeader *hdr)
{
	FILE *myfile;
	errno_t err;

	// Check file name for .wav extension
	if (filename[strlen(filename) - 1] == 'v')
	{
		err = fopen_s(&myfile, filename, "wb");

		if (err != 0)
		{
			std::cout << "Open (Write) wav file error\n";
			myfile = 0;
		}
		else
			fwrite(hdr, 1, 44, myfile);
	}
	else
	{
		std::cout << "You must specify a valid .wav file name\n";
		myfile = 0;
	}

	return myfile;
}

void WavReadWriter::FillHeader(struct SoundHeader &hdr)
{
	struct SoundHeader myhdr = { "RIF", 88244, "WAV", "fmt", 16, 1, 1,
		44100, 88200, 2, 16, "dat", 88200 };

	myhdr.srate = hdr.srate;
	myhdr.bits_per_samp = hdr.bits_per_samp;
	myhdr.bytes_per_samp = (hdr.bits_per_samp / 8);
	myhdr.dlength = hdr.dlength;
	myhdr.flength = hdr.dlength + 44;
	myhdr.bytes_per_sec = hdr.srate * myhdr.bytes_per_samp;
	myhdr.riff[3] = 'F';
	myhdr.wave[3] = 'E';
	myhdr.fmt[3] = ' ';
	myhdr.data[3] = 'a';
	hdr = myhdr;
}

void WavReadWriter::FloatToChar(float f, char* c)
{
	int tempint = (int)(f*32767.5f);

	*c = (char)(tempint & 0xFF);
	*(c + 1) = (char)((tempint >> 8) & 0xFF);
}

float WavReadWriter::CharToFloat(char* c)
{
	short tempshort = (short)(((((short) *(c + 1)) << 8) & 0xFF00) | (((short)*c) & 0x00FF));

	return (((float)tempshort) / 32768.0f);
}