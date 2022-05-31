#pragma once

#include <Windows.h>

struct CB_CS_DownScale
{
	UINT nWidth;
	UINT nHeight;
	UINT nTotalPixels;
	UINT nGroupSize;
};

struct CB_CS_Adaptation
{
	UINT nWidth;
	UINT nHeight;
	UINT nTotalPixels;
	UINT nGroupSize;
	float fAdaptation;	// 적응시를 위한 팩터
	UINT pad[3];		// pad
};

struct CB_CS_Bloom
{
	UINT nWidth;
	UINT nHeight;
	UINT nTotalPixels;
	UINT nGroupSize;
	float fAdaptation;
	float fBloomThreshold;
	UINT pad[2];
};

struct CB_CS_Blur
{
	UINT numApproxPasses;
	float fHalfBoxFilterWidth;			// w/2
	float fFracHalfBoxFilterWidth;		// frac(w/2+0.5)
	float fInvFracHalfBoxFilterWidth;	// 1-frac(w/2+0.5)
	float fRcpBoxFilterWidth;			// 1/w
	UINT pad[3];
};

struct CB_SC_FinalPass
{
	float fMiddleGrey;
	float fLumWhiteSqr;
	float fBloomScale;
	UINT pad;
};

struct CB_CS_BloomFilter
{
	float fHuddle;
	float fIncrease;
	UINT pad[2];
};
