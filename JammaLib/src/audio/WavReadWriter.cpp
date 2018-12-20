#include "WavReadWriter.h"

std::optional<std::tuple<std::unique_ptr<float[]>, unsigned int, unsigned int>>
	WavReadWriter::ReadWavFile(const std::string& fname, unsigned int maxsamps)
{
	size_t numsampsloaded;
	unsigned int samplerate;
	unsigned int counter1;
	char* cname;
	char* datain;
	FILE* fid;
	struct SoundHeader info;
	struct SoundHeader* pinfo = &info;

	// The variable maxsamps represents the maximum
	// number of 32 bit floats the array can contain.
	if (maxsamps < 1)
		return std::nullopt;

	cname = new char[fname.size() + 1];
	strcpy_s(cname, fname.size() + 1, fname.c_str());

	fid = OpenSoundIn(cname, pinfo);

	delete cname;

	if (fid == NULL)
		return {};

	numsampsloaded = maxsamps;

	// Only load the reported number of samples
	// (but prevent loading more than the array can hold)
	if (info.dlength < (long)(maxsamps * 2))
		numsampsloaded = info.dlength / 2; // Two 16 bit integers per sample (32 bit float)

	if (numsampsloaded < 1)
	{
		numsampsloaded = 0;
		fclose(fid);

		return {};
	}

	samplerate = info.srate;

	datain = new char[numsampsloaded * 2];  // Two bytes per wav file sample (16 bit integer)

	auto bytesread = fread(datain, 1, numsampsloaded * 2, fid);

	if ((bytesread / 2) < numsampsloaded)
		numsampsloaded = bytesread / 2;  // Two bytes per wav file sample (16 bit integer)

	fclose(fid);

	auto data = std::make_unique<float[]>(numsampsloaded);

	for (counter1 = 0; counter1<numsampsloaded; counter1++)
	{
		data[counter1] = CharToFloat(datain + (counter1 * 2));
	}

	delete datain;

	return std::make_tuple(std::move(data), (unsigned int)numsampsloaded, samplerate);
}

bool WavReadWriter::WriteWavFile(std::string fname, std::shared_ptr<float[]> data, unsigned int numsamps, unsigned int samplerate)
{
	unsigned int counter1;
	char* cname;
	char* dataout;
	struct SoundHeader info;
	struct SoundHeader* pinfo = &info;
	FILE* fid;

	cname = new char[fname.size() + 1];
	strcpy_s(cname, fname.size() + 1, fname.c_str());

	if (numsamps < 1)
	{
		delete cname;
		return false;
	}

	info.srate = samplerate;
	info.dlength = numsamps * 2;
	info.bits_per_samp = 16;

	FillHeader(info);

	fid = OpenSoundOut(cname, pinfo);

	delete cname;

	if (fid == NULL)
		return false;

	dataout = new char[numsamps * 2]; // Two bytes per wav file sample (16 bit integer)

	for (counter1 = 0; counter1<numsamps; counter1++)
	{
		FloatToChar(data[counter1], dataout + (counter1 * 2));
	}

	fwrite(dataout, 1, numsamps * 2, fid);

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