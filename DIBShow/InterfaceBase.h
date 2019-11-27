#pragma once


#define STANDARD_METHOD(type,method)	virtual type STDMETHODCALLTYPE method

#define STDCALL			__stdcall

#define OVEERIDE		override



// TODO
#define OVERRIDE_METHOD(type,method)	type STDMETHODCALLTYPE method override
#undef OVERRIDE_METHOD