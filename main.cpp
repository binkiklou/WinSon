#include <iostream>
#include <string>
#define NOMINMAX
#include <Windows.h>
#include <Audioclient.h>
#include <mmdeviceapi.h>
#include <functiondiscoverykeys_devpkey.h>
#include <stdio.h>

void fill_sin(signed int freq, float tps, int frame_count, BYTE* buffer)
{
	// Generate sin
	// Number of samples in one sine loop
	int its = ((float)1/(float)tps)*((float)1 / (float)freq);
	float* arr = new float[2*its];
	for (int i = 0; i < 2*its; i+=2) {
		float x = i * tps;
		arr[i] =   sinf(x * 3.1415f * (2 * freq));
		arr[i+1] = sinf(x * 3.1415f * (2 * freq));
	}

	// Fill (double  for two channels)
	int addr = 0;
	while (addr + (2*its*sizeof(float)) <= frame_count*8) {
		memcpy(buffer + addr, arr, 2*its*sizeof(float));
		addr += 2 * its * sizeof(float);
	}
}

int main()
{
	// OASHGSFHGFDSGF
	int f = 0;
	std::string str_f;
	std::cout << "Frequence de la vague  sine ? (Hz)" << std::endl;
	std::cin >> str_f;
	f = std::stoi(str_f);

	// Yes yes
	HRESULT hres;
	//std::cout<<sin()<<std::endl;

	// Initialisation librairie COM
	hres = CoInitialize(nullptr);

	if (hres != S_OK) {
		std::cout << "Erreur initializtion com" << std::endl;
		return 1;
	}
	// Acceder API d'Enumeration Audio
	IMMDeviceEnumerator* ptr_immde = nullptr;
	hres = CoCreateInstance(
		__uuidof(MMDeviceEnumerator),
		NULL, CLSCTX_ALL, 
		__uuidof(IMMDeviceEnumerator),
		(void**)&ptr_immde);
	if (hres != S_OK) {
		std::cout << "Erreur access Interface Enumerateur Audio" << std::endl;
		return 1;
	}
	// Accession "device" de base
	IMMDevice* ptr_dd = nullptr;
	hres = ptr_immde->GetDefaultAudioEndpoint(eRender, eConsole, &ptr_dd);
	if (hres != S_OK) {
		std::cout << "Erreur obtention device endpoint de base" << std::endl;
		return 1;
	}
	IPropertyStore* ptr_ipp = nullptr;
	hres = ptr_dd->OpenPropertyStore(STGM_READ, &ptr_ipp);
	if (hres != S_OK) {
		std::cout << "Erreur truc proprio" << std::endl;
		return 1;
	}
	PROPVARIANT pv;
	ptr_ipp->GetValue(PKEY_Device_FriendlyName, &pv);
	std::wcout << pv.pwszVal << std::endl;
	// Activation du client
	IAudioClient* ptr_ac = nullptr;
	hres = ptr_dd->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&ptr_ac);
	if (hres != S_OK) {
		std::cout << "Erreur activation device audio" << std::endl;
		return 1;
	}
	// Informations du format
	WAVEFORMATEXTENSIBLE* wef = nullptr;
	hres = ptr_ac->GetMixFormat((WAVEFORMATEX**)&wef);
	WAVEFORMATEX* wf = &(wef->Format);
	//hres = ptr_ac->GetMixFormat(&wf);
//	if (wf->wFormatTag == WAVE_FORMAT_EXTENSIBLE) {
//		std::cout << "fuck" << std::endl;
//		return 1;
//	}
	if (hres != S_OK) {
		std::cout << "Erreur de format audio" << std::endl;
		return 1;
	}
	// Initialisation du client
	ptr_ac->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, 10000000, 0, wf, NULL);
	if (hres != S_OK) {
		std::cout << "Erreur initialisation du stream audio" << std::endl;
		return 1;
	}
	// Informations importants
	UINT32 buffer_size;
	hres = ptr_ac->GetBufferSize(&buffer_size);
	if (hres != S_OK) {
		std::cout << "Erreur obtention de la taille du buffer" << std::endl;
		return 1;
	}
	// Creation Render client
	IAudioRenderClient* ptr_arc;
	hres = ptr_ac->GetService(__uuidof(IAudioRenderClient), (void**)&ptr_arc);
	if (hres != S_OK) {
		std::cout << "Erreur creation du render client" << std::endl;
		return 1;
	}
	// Duree du sample en unites par secondes
	BYTE* buffer_data;
	hres = ptr_arc->GetBuffer(buffer_size, &buffer_data);
	if (hres != S_OK) {
		return 1;
	}
	fill_sin(f,((float)1/(float)buffer_size), buffer_size, buffer_data);
	DWORD flags = 0;
	hres = ptr_arc->ReleaseBuffer(buffer_size,flags);
	if (hres != S_OK) {
		return 1;
	}
	REFERENCE_TIME duration = ((REFERENCE_TIME)10000000 * buffer_size)/wf->nSamplesPerSec;
	//Start!
	std::cout << "Client Audio Start" << std::endl;
	hres = ptr_ac->Start();
	if (hres != S_OK) {
		std::cout << "Erreur start client audio" << std::endl;
		return 1;
	}
	UINT32 frames_padding;
	// --- RENDER TIME ! --- 
	while (true) {
		Sleep((DWORD)500);
		ptr_ac->GetCurrentPadding(&frames_padding);
		UINT32 numFramesAvailable = buffer_size - frames_padding;
		ptr_arc->GetBuffer(numFramesAvailable, &buffer_data);
		fill_sin(f, ((float)1/(float)buffer_size), buffer_size, buffer_data);
		ptr_arc->ReleaseBuffer(numFramesAvailable, flags);
	}
	// pas bessoin de stop lol
	ptr_ac->Stop();
	std::cout << "Client Audio Stop" << std::endl;
	return 0;
}